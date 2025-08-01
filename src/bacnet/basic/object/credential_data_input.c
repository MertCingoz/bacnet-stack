/**
 * @file
 * @author Nikola Jelic <nikola.jelic@euroicc.com>
 * @date 2015
 * @brief Credential Data Input Objects - customize for your use
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* BACnet Stack defines - first */
#include "bacnet/bacdef.h"
/* BACnet Stack API */
#include "bacnet/bacdcode.h"
#include "bacnet/bacapp.h"
#include "bacnet/wp.h"
#include "bacnet/basic/services.h"
/* me! */
#include "bacnet/basic/object/credential_data_input.h"

static bool Credential_Data_Input_Initialized = false;

static CREDENTIAL_DATA_INPUT_DESCR cdi_descr[MAX_CREDENTIAL_DATA_INPUTS];

/* These three arrays are used by the ReadPropertyMultiple handler */
static const int Properties_Required[] = {
    /* unordered list of required properties */
    PROP_OBJECT_IDENTIFIER, PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,       PROP_PRESENT_VALUE,
    PROP_STATUS_FLAGS,      PROP_RELIABILITY,
    PROP_OUT_OF_SERVICE,    PROP_SUPPORTED_FORMATS,
    PROP_UPDATE_TIME,       -1
};

static const int Properties_Optional[] = { -1 };

static const int Properties_Proprietary[] = { -1 };

void Credential_Data_Input_Property_Lists(
    const int **pRequired, const int **pOptional, const int **pProprietary)
{
    if (pRequired) {
        *pRequired = Properties_Required;
    }
    if (pOptional) {
        *pOptional = Properties_Optional;
    }
    if (pProprietary) {
        *pProprietary = Properties_Proprietary;
    }

    return;
}

void Credential_Data_Input_Init(void)
{
    unsigned i;

    if (!Credential_Data_Input_Initialized) {
        Credential_Data_Input_Initialized = true;

        for (i = 0; i < MAX_CREDENTIAL_DATA_INPUTS; i++) {
            /* there should be a meaningful setup for present value */
            cdi_descr[i].present_value.format_type =
                AUTHENTICATION_FACTOR_UNDEFINED;
            cdi_descr[i].present_value.format_class = 0;
            octetstring_init(&cdi_descr[i].present_value.value, NULL, 0);
            cdi_descr[i].reliability = RELIABILITY_NO_FAULT_DETECTED;
            cdi_descr[i].out_of_service = false;
            /* set supported formats */
            cdi_descr[i].supported_formats_count = 0;
            /* timestamp uninitialized */
        }
    }

    return;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need validate that the */
/* given instance exists */
bool Credential_Data_Input_Valid_Instance(uint32_t object_instance)
{
    if (object_instance < MAX_CREDENTIAL_DATA_INPUTS) {
        return true;
    }

    return false;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then count how many you have */
unsigned Credential_Data_Input_Count(void)
{
    return MAX_CREDENTIAL_DATA_INPUTS;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the instance */
/* that correlates to the correct index */
uint32_t Credential_Data_Input_Index_To_Instance(unsigned index)
{
    return index;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the index */
/* that correlates to the correct instance number */
unsigned Credential_Data_Input_Instance_To_Index(uint32_t object_instance)
{
    unsigned index = MAX_CREDENTIAL_DATA_INPUTS;

    if (object_instance < MAX_CREDENTIAL_DATA_INPUTS) {
        index = object_instance;
    }

    return index;
}

/* note: the object name must be unique within this device */
bool Credential_Data_Input_Object_Name(
    uint32_t object_instance, BACNET_CHARACTER_STRING *object_name)
{
    char text[32] = "";
    bool status = false;

    if (object_instance < MAX_CREDENTIAL_DATA_INPUTS) {
        snprintf(
            text, sizeof(text), "CREDENTIAL DATA INPUT %lu",
            (unsigned long)object_instance);
        status = characterstring_init_ansi(object_name, text);
    }

    return status;
}

bool Credential_Data_Input_Out_Of_Service(uint32_t instance)
{
    unsigned index = 0;
    bool oos_flag = false;

    index = Credential_Data_Input_Instance_To_Index(instance);
    if (index < MAX_CREDENTIAL_DATA_INPUTS) {
        oos_flag = cdi_descr[index].out_of_service;
    }

    return oos_flag;
}

void Credential_Data_Input_Out_Of_Service_Set(uint32_t instance, bool oos_flag)
{
    unsigned index = 0;

    index = Credential_Data_Input_Instance_To_Index(instance);
    if (index < MAX_CREDENTIAL_DATA_INPUTS) {
        cdi_descr[index].out_of_service = oos_flag;
    }
}

/* return apdu len, or BACNET_STATUS_ERROR on error */
int Credential_Data_Input_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata)
{
    int len = 0;
    int apdu_len = 0; /* return value */
    BACNET_BIT_STRING bit_string;
    BACNET_CHARACTER_STRING char_string;
    unsigned object_index = 0;
    unsigned i = 0;
    bool state = false;
    uint8_t *apdu = NULL;

    if ((rpdata == NULL) || (rpdata->application_data == NULL) ||
        (rpdata->application_data_len == 0)) {
        return 0;
    }
    apdu = rpdata->application_data;
    object_index =
        Credential_Data_Input_Instance_To_Index(rpdata->object_instance);
    switch (rpdata->object_property) {
        case PROP_OBJECT_IDENTIFIER:
            apdu_len = encode_application_object_id(
                &apdu[0], OBJECT_CREDENTIAL_DATA_INPUT,
                rpdata->object_instance);
            break;
        case PROP_OBJECT_NAME:
            Credential_Data_Input_Object_Name(
                rpdata->object_instance, &char_string);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);
            break;
        case PROP_OBJECT_TYPE:
            apdu_len = encode_application_enumerated(
                &apdu[0], OBJECT_CREDENTIAL_DATA_INPUT);
            break;
        case PROP_PRESENT_VALUE:
            apdu_len = bacapp_encode_authentication_factor(
                &apdu[apdu_len], &cdi_descr[object_index].present_value);
            break;
        case PROP_STATUS_FLAGS:
            bitstring_init(&bit_string);
            bitstring_set_bit(&bit_string, STATUS_FLAG_IN_ALARM, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_FAULT, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_OVERRIDDEN, false);
            state =
                Credential_Data_Input_Out_Of_Service(rpdata->object_instance);
            bitstring_set_bit(&bit_string, STATUS_FLAG_OUT_OF_SERVICE, state);
            apdu_len = encode_application_bitstring(&apdu[0], &bit_string);
            break;
        case PROP_RELIABILITY:
            apdu_len = encode_application_enumerated(
                &apdu[0], cdi_descr[object_index].reliability);
            break;
        case PROP_OUT_OF_SERVICE:
            state =
                Credential_Data_Input_Out_Of_Service(rpdata->object_instance);
            apdu_len = encode_application_boolean(&apdu[0], state);
            break;
        case PROP_SUPPORTED_FORMATS:
            if (rpdata->array_index == 0) {
                apdu_len = encode_application_unsigned(
                    &apdu[0], cdi_descr[object_index].supported_formats_count);
            } else if (rpdata->array_index == BACNET_ARRAY_ALL) {
                for (i = 0; i < cdi_descr[object_index].supported_formats_count;
                     i++) {
                    len = bacapp_encode_authentication_factor_format(
                        &apdu[0],
                        &cdi_descr[object_index].supported_formats[i]);
                    if (apdu_len + len < MAX_APDU) {
                        apdu_len += len;
                    } else {
                        rpdata->error_code =
                            ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
                        apdu_len = BACNET_STATUS_ABORT;
                        break;
                    }
                }
            } else {
                if (rpdata->array_index <=
                    cdi_descr[object_index].supported_formats_count) {
                    apdu_len = bacapp_encode_authentication_factor_format(
                        &apdu[0],
                        &cdi_descr[object_index]
                             .supported_formats[rpdata->array_index - 1]);
                } else {
                    rpdata->error_class = ERROR_CLASS_PROPERTY;
                    rpdata->error_code = ERROR_CODE_INVALID_ARRAY_INDEX;
                    apdu_len = BACNET_STATUS_ERROR;
                }
            }

            break;
        case PROP_UPDATE_TIME:
            apdu_len = bacapp_encode_timestamp(
                &apdu[0], &cdi_descr[object_index].timestamp);
            break;
        default:
            rpdata->error_class = ERROR_CLASS_PROPERTY;
            rpdata->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            apdu_len = BACNET_STATUS_ERROR;
            break;
    }

    return apdu_len;
}

/* returns true if successful */
bool Credential_Data_Input_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data)
{
    bool status = false; /* return value */
    int len = 0;
    BACNET_APPLICATION_DATA_VALUE value = { 0 };
    unsigned object_index = 0;

    /* decode the some of the request */
    len = bacapp_decode_application_data(
        wp_data->application_data, wp_data->application_data_len, &value);
    /* FIXME: len < application_data_len: more data? */
    if (len < 0) {
        /* error while decoding - a value larger than we can handle */
        wp_data->error_class = ERROR_CLASS_PROPERTY;
        wp_data->error_code = ERROR_CODE_VALUE_OUT_OF_RANGE;
        return false;
    }
    object_index =
        Credential_Data_Input_Instance_To_Index(wp_data->object_instance);
    switch (wp_data->object_property) {
        case PROP_PRESENT_VALUE:
            if (Credential_Data_Input_Out_Of_Service(
                    wp_data->object_instance)) {
                BACNET_AUTHENTICATION_FACTOR tmp;
                len = bacapp_decode_authentication_factor(
                    wp_data->application_data, &tmp);
                if (len > 0) {
                    memcpy(
                        &cdi_descr[object_index].present_value, &tmp,
                        sizeof(BACNET_AUTHENTICATION_FACTOR));
                } else {
                    wp_data->error_class = ERROR_CLASS_PROPERTY;
                    wp_data->error_code = ERROR_CODE_INVALID_DATA_TYPE;
                }
            } else {
                wp_data->error_class = ERROR_CLASS_PROPERTY;
                wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            }
            break;
        case PROP_RELIABILITY:
            if (Credential_Data_Input_Out_Of_Service(
                    wp_data->object_instance)) {
                status = write_property_type_valid(
                    wp_data, &value, BACNET_APPLICATION_TAG_ENUMERATED);
                if (status) {
                    cdi_descr[object_index].reliability =
                        (BACNET_RELIABILITY)value.type.Enumerated;
                }
            } else {
                wp_data->error_class = ERROR_CLASS_PROPERTY;
                wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            }
            break;
        case PROP_OBJECT_IDENTIFIER:
        case PROP_OBJECT_NAME:
        case PROP_OBJECT_TYPE:
        case PROP_STATUS_FLAGS:
        case PROP_OUT_OF_SERVICE:
        case PROP_SUPPORTED_FORMATS:
        case PROP_UPDATE_TIME:
            wp_data->error_class = ERROR_CLASS_PROPERTY;
            wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            break;
        default:
            wp_data->error_class = ERROR_CLASS_PROPERTY;
            wp_data->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            break;
    }

    return status;
}
