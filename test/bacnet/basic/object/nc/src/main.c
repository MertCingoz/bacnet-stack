/**
 * @file
 * @brief Unit test for object
 * @author Steve Karg <skarg@users.sourceforge.net>
 * @date December 2022
 *
 * SPDX-License-Identifier: MIT
 */
#include <zephyr/ztest.h>
#include <bacnet/bactext.h>
#include <bacnet/rp.h>
#include <bacnet/wp.h>
#include <bacnet/basic/object/nc.h>

/**
 * @addtogroup bacnet_tests
 * @{
 */

/**
 * @brief Test
 */
#if defined(CONFIG_ZTEST_NEW_API)
ZTEST(notification_class_tests, test_Notification_Class)
#else
static void test_Notification_Class(void)
#endif
{
    uint8_t apdu[MAX_APDU] = { 0 };
    int len = 0;
    int test_len = 0;
    BACNET_READ_PROPERTY_DATA rpdata = { 0 };
    BACNET_APPLICATION_DATA_VALUE value = { 0 };
    const int *pRequired = NULL;
    const int *pOptional = NULL;
    const int *pProprietary = NULL;
    const uint32_t instance = 1;
    BACNET_WRITE_PROPERTY_DATA wpdata = { 0 };
    bool status = false;
    unsigned index;

    Notification_Class_Init();
    status = Notification_Class_Valid_Instance(instance);
    zassert_true(status, NULL);
    index = Notification_Class_Instance_To_Index(instance);
    zassert_equal(index, instance, "index=%u", index);

    rpdata.application_data = &apdu[0];
    rpdata.application_data_len = sizeof(apdu);
    rpdata.object_type = OBJECT_NOTIFICATION_CLASS;
    rpdata.object_instance = instance;
    rpdata.object_property = PROP_OBJECT_IDENTIFIER;

    Notification_Class_Property_Lists(&pRequired, &pOptional, &pProprietary);
    while ((*pRequired) >= 0) {
        rpdata.object_property = *pRequired;
        rpdata.array_index = BACNET_ARRAY_ALL;
        len = Notification_Class_Read_Property(&rpdata);
        zassert_not_equal(
            len, BACNET_STATUS_ERROR,
            "property '%s': failed to ReadProperty!\n",
            bactext_property_name(rpdata.object_property));
        if (len >= 0) {
            test_len = bacapp_decode_known_property(
                rpdata.application_data, len, &value, rpdata.object_type,
                rpdata.object_property);
            if ((rpdata.object_property != PROP_PRIORITY) &&
                (rpdata.object_property != PROP_RECIPIENT_LIST)) {
                zassert_equal(
                    len, test_len, "property '%s': failed to decode!\n",
                    bactext_property_name(rpdata.object_property));
            }
            /* check WriteProperty properties */
            wpdata.object_type = rpdata.object_type;
            wpdata.object_instance = rpdata.object_instance;
            wpdata.object_property = rpdata.object_property;
            wpdata.array_index = rpdata.array_index;
            memcpy(&wpdata.application_data, rpdata.application_data, MAX_APDU);
            wpdata.application_data_len = len;
            wpdata.error_code = ERROR_CODE_SUCCESS;
            status = Notification_Class_Write_Property(&wpdata);
            if (!status) {
                /* verify WriteProperty property is known */
                zassert_not_equal(
                    wpdata.error_code, ERROR_CODE_UNKNOWN_PROPERTY,
                    "property '%s': WriteProperty Unknown!\n",
                    bactext_property_name(rpdata.object_property));
            }
        }
        pRequired++;
    }
    while ((*pOptional) != -1) {
        rpdata.object_property = *pOptional;
        rpdata.array_index = BACNET_ARRAY_ALL;
        len = Notification_Class_Read_Property(&rpdata);
        zassert_not_equal(
            len, BACNET_STATUS_ERROR,
            "property '%s': failed to ReadProperty!\n",
            bactext_property_name(rpdata.object_property));
        if (len > 0) {
            test_len = bacapp_decode_application_data(
                rpdata.application_data, (uint8_t)rpdata.application_data_len,
                &value);
            zassert_equal(
                len, test_len, "property '%s': failed to decode!\n",
                bactext_property_name(rpdata.object_property));
            /* check WriteProperty properties */
            wpdata.object_type = rpdata.object_type;
            wpdata.object_instance = rpdata.object_instance;
            wpdata.object_property = rpdata.object_property;
            wpdata.array_index = rpdata.array_index;
            memcpy(&wpdata.application_data, rpdata.application_data, MAX_APDU);
            wpdata.application_data_len = len;
            wpdata.error_code = ERROR_CODE_SUCCESS;
            status = Notification_Class_Write_Property(&wpdata);
            if (!status) {
                /* verify WriteProperty property is known */
                zassert_not_equal(
                    wpdata.error_code, ERROR_CODE_UNKNOWN_PROPERTY,
                    "property '%s': WriteProperty Unknown!\n",
                    bactext_property_name(rpdata.object_property));
            }
        }
        pOptional++;
    }
    rpdata.object_property = PROP_ALL;
    len = Notification_Class_Read_Property(&rpdata);
    zassert_equal(len, BACNET_STATUS_ERROR, NULL);
    wpdata.object_property = PROP_ALL;
    status = Notification_Class_Write_Property(&wpdata);
    zassert_false(status, NULL);

    return;
}
/**
 * @}
 */

#if defined(CONFIG_ZTEST_NEW_API)
ZTEST_SUITE(notification_class_tests, NULL, NULL, NULL, NULL, NULL);
#else
void test_main(void)
{
    ztest_test_suite(
        notification_class_tests, ztest_unit_test(test_Notification_Class));

    ztest_run_test_suite(notification_class_tests);
}
#endif
