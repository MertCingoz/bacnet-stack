/**************************************************************************
 *
 * Copyright (C) 2005 Steve Karg <skarg@users.sourceforge.net>
 *
 * SPDX-License-Identifier: MIT
 *
 *********************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* BACnet Stack defines - first */
#include "bacnet/bacdef.h"
/* BACnet Stack API */
#include "bacnet/bacdcode.h"
#include "bacnet/bacerror.h"
#include "bacnet/apdu.h"
#include "bacnet/npdu.h"
#include "bacnet/abort.h"
#include "bacnet/reject.h"
#include "bacnet/wp.h"
/* basic objects, services, TSM, and datalink */
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/tsm/tsm.h"
#include "bacnet/basic/services.h"
#include "bacnet/basic/sys/debug.h"
#include "bacnet/datalink/datalink.h"

/** @file h_wp.c  Handles Write Property requests. */

/** Handler for a WriteProperty Service request.
 * @ingroup DSWP
 * This handler will be invoked by apdu_handler() if it has been enabled
 * by a call to apdu_set_confirmed_handler().
 * This handler builds a response packet, which is
 * - an Abort if
 *   - the message is segmented
 *   - if decoding fails
 * - an ACK if Device_Write_Property() succeeds
 * - an Error if Device_Write_Property() fails
 *   or there isn't enough room in the APDU to fit the data.
 *
 * @param service_request [in] The contents of the service request.
 * @param service_len [in] The length of the service_request.
 * @param src [in] BACNET_ADDRESS of the source of the message
 * @param service_data [in] The BACNET_CONFIRMED_SERVICE_DATA information
 *                          decoded from the APDU header of this message.
 */
void handler_write_property(
    uint8_t *service_request,
    uint16_t service_len,
    BACNET_ADDRESS *src,
    BACNET_CONFIRMED_SERVICE_DATA *service_data)
{
    BACNET_WRITE_PROPERTY_DATA wp_data;
    int len = 0;
    bool bcontinue = true;
    bool success;
    int pdu_len = 0;
    BACNET_NPDU_DATA npdu_data;
    int bytes_sent = 0;
    BACNET_ADDRESS my_address;

    /* encode the NPDU portion of the packet */
    datalink_get_my_address(&my_address);
    npdu_encode_npdu_data(&npdu_data, false, service_data->priority);
    pdu_len = npdu_encode_pdu(
        &Handler_Transmit_Buffer[0], src, &my_address, &npdu_data);
    debug_print("WP: Received Request!\n");
    if (service_len == 0) {
        len = reject_encode_apdu(
            &Handler_Transmit_Buffer[pdu_len], service_data->invoke_id,
            REJECT_REASON_MISSING_REQUIRED_PARAMETER);
        debug_print("WP: Missing Required Parameter. Sending Reject!\n");
        bcontinue = false;
    } else if (service_data->segmented_message) {
        len = abort_encode_apdu(
            &Handler_Transmit_Buffer[pdu_len], service_data->invoke_id,
            ABORT_REASON_SEGMENTATION_NOT_SUPPORTED, true);
        debug_print("WP: Segmented message.  Sending Abort!\n");
        bcontinue = false;
    }
    if (bcontinue) {
        /* decode the service request only */
        len = wp_decode_service_request(service_request, service_len, &wp_data);
        if (len > 0) {
            debug_fprintf(
                stderr,
                "WP: type=%lu instance=%lu property=%lu priority=%lu "
                "index=%ld\n",
                (unsigned long)wp_data.object_type,
                (unsigned long)wp_data.object_instance,
                (unsigned long)wp_data.object_property,
                (unsigned long)wp_data.priority, (long)wp_data.array_index);
        } else {
            debug_print("WP: Unable to decode Request!\n");
        }
        /* bad decoding or something we didn't understand - send an abort */
        if (len <= 0) {
            len = abort_encode_apdu(
                &Handler_Transmit_Buffer[pdu_len], service_data->invoke_id,
                ABORT_REASON_OTHER, true);
            debug_print("WP: Bad Encoding. Sending Abort!\n");
            bcontinue = false;
        }
        if (bcontinue) {
            success = write_property_bacnet_array_valid(&wp_data);
            if (success) {
                success = Device_Write_Property(&wp_data);
            }
            if (success) {
                len = encode_simple_ack(
                    &Handler_Transmit_Buffer[pdu_len], service_data->invoke_id,
                    SERVICE_CONFIRMED_WRITE_PROPERTY);
                debug_print("WP: Sending Simple Ack!\n");
            } else {
                len = bacerror_encode_apdu(
                    &Handler_Transmit_Buffer[pdu_len], service_data->invoke_id,
                    SERVICE_CONFIRMED_WRITE_PROPERTY, wp_data.error_class,
                    wp_data.error_code);
                debug_print("WP: Sending Error!\n");
            }
        }
    }
    /* Send PDU */
    pdu_len += len;
    bytes_sent = datalink_send_pdu(
        src, &npdu_data, &Handler_Transmit_Buffer[0], pdu_len);
    if (bytes_sent <= 0) {
        debug_perror("WP: Failed to send PDU");
    }

    return;
}

/**
 * @brief Perform basic validation of Write Property argument based on
 * the assumption that it is a string. Check for correct data type,
 * correct encoding (fixed here as ANSI X34),correct length, and
 * finally if it is allowed to be empty.
 *
 * @param pValue  Pointer to the application data value representing the string.
 * @param iMaxLen  Maximum string length allowed.
 * @param bEmptyAllowed  true, if empty strings shall be allowed.
 * @param pErrorClass  Pointer to a variable taking the error class.
 * @param pErrorCode  Pointer to a variable taking the error code.
 *
 * @return True on success, false otherwise.
 */
bool WPValidateString(
    const BACNET_APPLICATION_DATA_VALUE *pValue,
    int iMaxLen,
    bool bEmptyAllowed,
    BACNET_ERROR_CLASS *pErrorClass,
    BACNET_ERROR_CODE *pErrorCode)
{
    bool bResult;

    /* Save on a bit of code duplication by pre selecting the most
     * common outcomes from the tests (not necessarily the most likely
     * outcome of the tests).
     */
    bResult = false;
    *pErrorClass = ERROR_CLASS_PROPERTY;

    if (pValue->tag == BACNET_APPLICATION_TAG_CHARACTER_STRING) {
        if (characterstring_encoding(&pValue->type.Character_String) ==
            CHARACTER_ANSI_X34) {
            if ((bEmptyAllowed == false) &&
                (characterstring_length(&pValue->type.Character_String) == 0)) {
                *pErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
            } else if (
                (bEmptyAllowed == false) &&
                (!characterstring_printable(&pValue->type.Character_String))) {
                /* assumption: non-empty also means must be "printable" */
                *pErrorCode = ERROR_CODE_VALUE_OUT_OF_RANGE;
            } else if (
                characterstring_length(&pValue->type.Character_String) >
                (uint16_t)iMaxLen) {
                *pErrorClass = ERROR_CLASS_RESOURCES;
                *pErrorCode = ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY;
            } else {
                bResult = true; /* It's all good! */
            }
        } else {
            *pErrorCode = ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED;
        }
    } else {
        *pErrorCode = ERROR_CODE_INVALID_DATA_TYPE;
    }

    return (bResult);
}

/** Perform simple validation of type of Write Property argument based
 * the expected type vs the actual. Set up error response if the
 * validation fails. Cuts out reams of repeated code in the object code.
 */

bool WPValidateArgType(
    const BACNET_APPLICATION_DATA_VALUE *pValue,
    uint8_t ucExpectedTag,
    BACNET_ERROR_CLASS *pErrorClass,
    BACNET_ERROR_CODE *pErrorCode)
{
    bool bResult;

    /*
     * start out assuming success and only set up error
     * response if validation fails.
     */
    bResult = true;
    if (pValue->tag != ucExpectedTag) {
        bResult = false;
        *pErrorClass = ERROR_CLASS_PROPERTY;
        *pErrorCode = ERROR_CODE_INVALID_DATA_TYPE;
    }

    return (bResult);
}
