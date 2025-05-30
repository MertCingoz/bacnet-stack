/**
 * @file
 * @brief Handler for a BACnet ConfirmedPrivateTransfer Acknowledgment example
 * @author Peter Mc Shane <petermcs@users.sourceforge.net>
 * @date 2005
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* BACnet Stack defines - first */
#include "bacnet/bacdef.h"
/* BACnet Stack API */
#include "bacnet/bacdcode.h"
#include "bacnet/apdu.h"
#include "bacnet/npdu.h"
#include "bacnet/abort.h"
#include "bacnet/ptransfer.h"
#include "bacnet/basic/services.h"
#include "bacnet/basic/tsm/tsm.h"

extern uint8_t IOBufferPT[300]; /* Somewhere to build the encoded result block
                                   for Private Transfers */

static void DecodeBlock(char cBlockNum, uint8_t *pData)
{
    int iLen;
    uint32_t ulTemp;
    int tag_len;
    uint8_t tag_number;
    uint32_t len_value_type;
    BACNET_CHARACTER_STRING bsName;
    DATABLOCK Response;

    iLen = 0;

    if (cBlockNum >= MY_MAX_BLOCK) {
        return;
    }

    tag_len =
        decode_tag_number_and_value(&pData[iLen], &tag_number, &len_value_type);
    iLen += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_UNSIGNED_INT) {
        return;
    }

    iLen += decode_unsigned(&pData[iLen], len_value_type, &ulTemp);
    Response.cMyByte1 = (char)ulTemp;

    tag_len =
        decode_tag_number_and_value(&pData[iLen], &tag_number, &len_value_type);
    iLen += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_UNSIGNED_INT) {
        return;
    }

    iLen += decode_unsigned(&pData[iLen], len_value_type, &ulTemp);
    Response.cMyByte2 = (char)ulTemp;

    tag_len =
        decode_tag_number_and_value(&pData[iLen], &tag_number, &len_value_type);
    iLen += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_REAL) {
        return;
    }

    iLen += decode_real(&pData[iLen], &Response.fMyReal);

    tag_len =
        decode_tag_number_and_value(&pData[iLen], &tag_number, &len_value_type);
    iLen += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_CHARACTER_STRING) {
        return;
    }

    iLen += decode_character_string(&pData[iLen], len_value_type, &bsName);
    snprintf(
        (char *)Response.sMyString, MY_MAX_STR, "%s",
        characterstring_value(&bsName));
    printf("Private Transfer Read Block Response\n");
    printf("Data Block: %d\n", (int)cBlockNum);
    printf("  First Byte  : %d\n", (int)Response.cMyByte1);
    printf("  Second Byte : %d\n", (int)Response.cMyByte2);
    printf("  Real        : %f\n", Response.fMyReal);
    printf("  String      : %s\n\n", Response.sMyString);
}

static void ProcessPTA(BACNET_PRIVATE_TRANSFER_DATA *data)
{
    int iLen; /* Index to current location in data */
    uint32_t uiErrorCode;
    char cBlockNumber;
    uint32_t ulTemp;
    int tag_len;
    uint8_t tag_number;
    uint32_t len_value_type;

    iLen = 0;

    /* Error code is returned for read and write operations */

    tag_len = decode_tag_number_and_value(
        &data->serviceParameters[iLen], &tag_number, &len_value_type);
    iLen += tag_len;
    if (tag_number != BACNET_APPLICATION_TAG_UNSIGNED_INT) {
#if PRINT_ENABLED
        printf("CPTA: Bad Encoding!\n");
#endif
        return;
    }
    iLen += decode_unsigned(
        &data->serviceParameters[iLen], len_value_type, &uiErrorCode);

    if (data->serviceNumber == MY_SVC_READ) { /* Read I/O block so should be
                                                 full block of data or error */
        /* Decode the error type and if necessary block number and then fetch
         * the info */

        if (uiErrorCode == MY_ERR_OK) {
            /* Block Number */
            tag_len = decode_tag_number_and_value(
                &data->serviceParameters[iLen], &tag_number, &len_value_type);
            iLen += tag_len;
            if (tag_number != BACNET_APPLICATION_TAG_UNSIGNED_INT) {
#if PRINT_ENABLED
                printf("CPTA: Bad Encoding!\n");
#endif
                return;
            }

            iLen += decode_unsigned(
                &data->serviceParameters[iLen], len_value_type, &ulTemp);
            cBlockNumber = (char)ulTemp;
            DecodeBlock(cBlockNumber, &data->serviceParameters[iLen]);
        } else { /* Read error */
            printf(
                "Private Transfer read operation returned error code: %lu\n",
                (unsigned long)uiErrorCode);
            return;
        }
    } else { /* Write I/O block - should just be an OK type message */
        printf(
            "Private Transfer write operation returned error code: %lu\n",
            (unsigned long)uiErrorCode);
    }
}

/*
 * This is called when we receive a private transfer packet ack.
 * We parse the response which the remote application generated
 * and decide what to do next...
 */

void handler_conf_private_trans_ack(
    uint8_t *service_request,
    uint16_t service_len,
    BACNET_ADDRESS *src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA *service_data)
{
    BACNET_PRIVATE_TRANSFER_DATA data;
    int len;

    /*
     * Note:
     * We currently don't look at the source address and service data
     * but we probably should to verify that the ack is oneit is what
     * we were expecting. But this is just to silence some compiler
     * warnings from Borland.
     */
    src = src;
    service_data = service_data;

    len = 0;

#if PRINT_ENABLED
    printf("Received Confirmed Private Transfer Ack!\n");
#endif

    len = ptransfer_decode_service_request(
        service_request, service_len,
        &data); /* Same decode for ack as for service request! */
    if (len < 0) {
#if PRINT_ENABLED
        printf("cpta: Bad Encoding!\n");
#endif
    }

    ProcessPTA(&data); /* See what to do with the response */

    return;
}

#if 0
void PTErrorHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code)
{
    /* FIXME: verify src and invoke id */
    (void) src;
    (void) invoke_id;
    printf("BACnet Error: %s: %s\r\n",
        bactext_error_class_name((int) error_class),
        bactext_error_code_name((int) error_code));
    Error_Detected = true;
}
#endif
