/**
 * @file
 * @brief Lookup or Translate BACnet Name Text
 * @author Steve Karg <skarg@users.sourceforge.net>
 * @date 2005
 * @copyright SPDX-License-Identifier: GPL-2.0-or-later WITH GCC-exception-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "bacnet/indtext.h"
#include "bacnet/bacenum.h"
#include "bacnet/bactext.h"

static const char *ASHRAE_Reserved_String = "Reserved for Use by ASHRAE";
static const char *Vendor_Proprietary_String = "Vendor Proprietary Value";

/* Search for a text value first based on the corresponding text list, then by
 * attempting to convert to an integer value. */
static bool bactext_strtol_index(
    INDTEXT_DATA *istring, const char *search_name, unsigned *found_index)
{
    char *endptr;
    long value;

    if (indtext_by_istring(istring, search_name, found_index) == true) {
        return true;
    } else {
        value = strtol(search_name, &endptr, 0);
        if (endptr == search_name) {
            /* No digits found */
            return false;
        } else if (*endptr != '\0') {
            /* Extra text found */
            return false;
        } else {
            *found_index = (unsigned)value;
            return true;
        }
    }
}

INDTEXT_DATA bacnet_confirmed_service_names[] = {
    { SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM, "Acknowledge-Alarm" },
    { SERVICE_CONFIRMED_COV_NOTIFICATION, "COV-Notification" },
    { SERVICE_CONFIRMED_EVENT_NOTIFICATION, "Event-Notification" },
    { SERVICE_CONFIRMED_GET_ALARM_SUMMARY, "Get-Alarm-Summary" },
    { SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY, "Get-Enrollment-Summary" },
    { SERVICE_CONFIRMED_SUBSCRIBE_COV, "Subscribe-COV" },
    { SERVICE_CONFIRMED_ATOMIC_READ_FILE, "Atomic-Read-File" },
    { SERVICE_CONFIRMED_ATOMIC_WRITE_FILE, "Atomic-Write-File" },
    { SERVICE_CONFIRMED_ADD_LIST_ELEMENT, "Add-List-Element" },
    { SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT, "Remove-List-Element" },
    { SERVICE_CONFIRMED_CREATE_OBJECT, "Create-Object" },
    { SERVICE_CONFIRMED_DELETE_OBJECT, "Delete-Object" },
    { SERVICE_CONFIRMED_READ_PROPERTY, "Read-Property" },
    { SERVICE_CONFIRMED_READ_PROP_CONDITIONAL, "Read-Property-Conditional" },
    { SERVICE_CONFIRMED_READ_PROP_MULTIPLE, "Read-Property-Multiple" },
    { SERVICE_CONFIRMED_WRITE_PROPERTY, "Write-Property" },
    { SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE, "Write-Property-Multiple" },
    { SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
      "Device-Communication-Control" },
    { SERVICE_CONFIRMED_PRIVATE_TRANSFER, "Private-Transfer" },
    { SERVICE_CONFIRMED_TEXT_MESSAGE, "Text-Message" },
    { SERVICE_CONFIRMED_REINITIALIZE_DEVICE, "Reinitialize-Device" },
    { SERVICE_CONFIRMED_VT_OPEN, "VT-Open" },
    { SERVICE_CONFIRMED_VT_CLOSE, "VT-Close" },
    { SERVICE_CONFIRMED_VT_DATA, "VT-Data" },
    { SERVICE_CONFIRMED_AUTHENTICATE, "Authenticate" },
    { SERVICE_CONFIRMED_REQUEST_KEY, "Request-Key" },
    { SERVICE_CONFIRMED_READ_RANGE, "Read-Range" },
    { SERVICE_CONFIRMED_LIFE_SAFETY_OPERATION, "Life-Safety_Operation" },
    { SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY, "Subscribe-COV-Property" },
    { SERVICE_CONFIRMED_GET_EVENT_INFORMATION, "Get-Event-Information" },
    { 0, NULL }
};

const char *bactext_confirmed_service_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_confirmed_service_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_unconfirmed_service_names[] = {
    { SERVICE_UNCONFIRMED_I_AM, "I-Am" },
    { SERVICE_UNCONFIRMED_I_HAVE, "I-Have" },
    { SERVICE_UNCONFIRMED_COV_NOTIFICATION, "COV-Notification" },
    { SERVICE_UNCONFIRMED_EVENT_NOTIFICATION, "Event-Notification" },
    { SERVICE_UNCONFIRMED_PRIVATE_TRANSFER, "Private-Transfer" },
    { SERVICE_UNCONFIRMED_TEXT_MESSAGE, "Text-Message" },
    { SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION, "Time-Synchronization" },
    { SERVICE_UNCONFIRMED_WHO_HAS, "Who-Has" },
    { SERVICE_UNCONFIRMED_WHO_IS, "Who-Is" },
    { SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION,
      "UTC-Time-Synchronization" },
    { SERVICE_UNCONFIRMED_WRITE_GROUP, "Write-Group" },
    { 0, NULL }
};

const char *bactext_unconfirmed_service_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_unconfirmed_service_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_application_tag_names[] = {
    { BACNET_APPLICATION_TAG_NULL, "Null" },
    { BACNET_APPLICATION_TAG_BOOLEAN, "Boolean" },
    { BACNET_APPLICATION_TAG_UNSIGNED_INT, "Unsigned Int" },
    { BACNET_APPLICATION_TAG_SIGNED_INT, "Signed Int" },
    { BACNET_APPLICATION_TAG_REAL, "Real" },
    { BACNET_APPLICATION_TAG_DOUBLE, "Double" },
    { BACNET_APPLICATION_TAG_OCTET_STRING, "Octet String" },
    { BACNET_APPLICATION_TAG_CHARACTER_STRING, "Character String" },
    { BACNET_APPLICATION_TAG_BIT_STRING, "Bit String" },
    { BACNET_APPLICATION_TAG_ENUMERATED, "Enumerated" },
    { BACNET_APPLICATION_TAG_DATE, "Date" },
    { BACNET_APPLICATION_TAG_TIME, "Time" },
    { BACNET_APPLICATION_TAG_OBJECT_ID, "Object ID" },
    { BACNET_APPLICATION_TAG_RESERVE1, "Reserved 1" },
    { BACNET_APPLICATION_TAG_RESERVE2, "Reserved 2" },
    { BACNET_APPLICATION_TAG_RESERVE3, "Reserved 3" },
    /* complex data types */
    { BACNET_APPLICATION_TAG_EMPTYLIST, "Empty List" },
    { BACNET_APPLICATION_TAG_WEEKNDAY, "BACnetWeeknday" },
    { BACNET_APPLICATION_TAG_DATERANGE, "BACnetDateRange" },
    { BACNET_APPLICATION_TAG_DATETIME, "BACnetDateTime" },
    { BACNET_APPLICATION_TAG_TIMESTAMP, "BACnetTimeStamp" },
    { BACNET_APPLICATION_TAG_ERROR, "Error" },
    { BACNET_APPLICATION_TAG_DEVICE_OBJECT_PROPERTY_REFERENCE,
      "BACnetDeviceObjectPropertyReference" },
    { BACNET_APPLICATION_TAG_DEVICE_OBJECT_REFERENCE,
      "BACnetDeviceObjectReference" },
    { BACNET_APPLICATION_TAG_DESTINATION, "BACnetDestination" },
    { BACNET_APPLICATION_TAG_RECIPIENT, "BACnetRecipient" },
    { BACNET_APPLICATION_TAG_COV_SUBSCRIPTION, "BACnetCOVSubscription" },
    { BACNET_APPLICATION_TAG_CALENDAR_ENTRY, "BACnetCalendarEntry" },
    { BACNET_APPLICATION_TAG_WEEKLY_SCHEDULE, "BACnetWeeklySchedule" },
    { BACNET_APPLICATION_TAG_SPECIAL_EVENT, "BACnetSpecialEvent" },
    { BACNET_APPLICATION_TAG_READ_ACCESS_SPECIFICATION,
      "BACnetReadAccessSpecification" },
    { BACNET_APPLICATION_TAG_LIGHTING_COMMAND, "BACnetLightingCommand" },
    { BACNET_APPLICATION_TAG_HOST_N_PORT, "BACnetHostNPort" },
    { BACNET_APPLICATION_TAG_XY_COLOR, "BACnetxyColor" },
    { BACNET_APPLICATION_TAG_COLOR_COMMAND, "BACnetColorCommand" },
    { BACNET_APPLICATION_TAG_BDT_ENTRY, "BACnetBDTEntry" },
    { BACNET_APPLICATION_TAG_FDT_ENTRY, "BACnetFDTEntry" },
    { BACNET_APPLICATION_TAG_ACTION_COMMAND, "BACnetActionCommand" },
    { BACNET_APPLICATION_TAG_SCALE, "BACnetScale" },
    { BACNET_APPLICATION_TAG_SHED_LEVEL, "BACnetShedLevel" },
    { BACNET_APPLICATION_TAG_ACCESS_RULE, "BACnetAccessRule" },
    { BACNET_APPLICATION_TAG_CHANNEL_VALUE, "BACnetChannelValue" },
    { 0, NULL }
};

const char *bactext_application_tag_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_application_tag_names, index, ASHRAE_Reserved_String);
}

bool bactext_application_tag_index(
    const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_application_tag_names, search_name, found_index);
}

INDTEXT_DATA bacnet_object_type_names[] = {
    { OBJECT_ANALOG_INPUT, "analog-input" },
    { OBJECT_ANALOG_OUTPUT, "analog-output" },
    { OBJECT_ANALOG_VALUE, "analog-value" },
    { OBJECT_BINARY_INPUT, "binary-input" },
    { OBJECT_BINARY_OUTPUT, "binary-output" },
    { OBJECT_BINARY_VALUE, "binary-value" },
    { OBJECT_CALENDAR, "calendar" },
    { OBJECT_COMMAND, "command" },
    { OBJECT_DEVICE, "device" },
    { OBJECT_EVENT_ENROLLMENT, "event-enrollment" },
    { OBJECT_FILE, "file" },
    { OBJECT_GROUP, "group" },
    { OBJECT_LOOP, "loop" },
    { OBJECT_MULTI_STATE_INPUT, "multi-state-input" },
    { OBJECT_MULTI_STATE_OUTPUT, "multi-state-output" },
    { OBJECT_NOTIFICATION_CLASS, "notification-class" },
    { OBJECT_PROGRAM, "program" },
    { OBJECT_SCHEDULE, "schedule" },
    { OBJECT_AVERAGING, "averaging" },
    { OBJECT_MULTI_STATE_VALUE, "multi-state-value" },
    { OBJECT_TRENDLOG, "trend-log" },
    { OBJECT_LIFE_SAFETY_POINT, "life-safety-point" },
    { OBJECT_LIFE_SAFETY_ZONE, "life-safety-zone" },
    { OBJECT_ACCUMULATOR, "accumulator" },
    { OBJECT_PULSE_CONVERTER, "pulse-converter" },
    { OBJECT_EVENT_LOG, "event-log" },
    { OBJECT_GLOBAL_GROUP, "global-group" },
    { OBJECT_TREND_LOG_MULTIPLE, "trend-log-multiple" },
    { OBJECT_LOAD_CONTROL, "load-control" },
    { OBJECT_STRUCTURED_VIEW, "structured-view" },
    { OBJECT_ACCESS_DOOR, "access-door" },
    { OBJECT_LIGHTING_OUTPUT, "lighting-output" },
    { OBJECT_ACCESS_CREDENTIAL, "access-credential" },
    { OBJECT_ACCESS_POINT, "access-point" },
    { OBJECT_ACCESS_RIGHTS, "access-rights" },
    { OBJECT_ACCESS_USER, "access-user" },
    { OBJECT_ACCESS_ZONE, "access-zone" },
    { OBJECT_CREDENTIAL_DATA_INPUT, "credential-data-input" },
    { OBJECT_NETWORK_SECURITY, "network-security" },
    { OBJECT_BITSTRING_VALUE, "bitstring-value" },
    { OBJECT_CHARACTERSTRING_VALUE, "characterstring-value" },
    { OBJECT_DATE_PATTERN_VALUE, "date-pattern-value" },
    { OBJECT_DATE_VALUE, "date-value" },
    { OBJECT_DATETIME_PATTERN_VALUE, "datetime-pattern-value" },
    { OBJECT_DATETIME_VALUE, "datetime-value" },
    { OBJECT_INTEGER_VALUE, "integer-value" },
    { OBJECT_LARGE_ANALOG_VALUE, "large-analog-value" },
    { OBJECT_OCTETSTRING_VALUE, "octetstring-value" },
    { OBJECT_POSITIVE_INTEGER_VALUE, "positive-integer-value" },
    { OBJECT_TIME_PATTERN_VALUE, "time-pattern-value" },
    { OBJECT_TIME_VALUE, "time-value" },
    { OBJECT_NOTIFICATION_FORWARDER, "notification-forwarder" },
    { OBJECT_ALERT_ENROLLMENT, "alert-enrollment" },
    { OBJECT_CHANNEL, "channel" },
    { OBJECT_LIGHTING_OUTPUT, "lighting-output" },
    { OBJECT_BINARY_LIGHTING_OUTPUT, "binary-lighting-output" },
    { OBJECT_NETWORK_PORT, "network-port" },
    { OBJECT_ELEVATOR_GROUP, "elevator-group" },
    { OBJECT_ESCALATOR, "escalator" },
    { OBJECT_LIFT, "lift" },
    { OBJECT_STAGING, "staging" },
    { OBJECT_AUDIT_LOG, "audit-log" },
    { OBJECT_AUDIT_REPORTER, "audit-reporter" },
    { OBJECT_COLOR, "color" },
    { OBJECT_COLOR_TEMPERATURE, "color-temperature" },
    /* Enumerated values 0-127 are reserved for definition by ASHRAE.
       Enumerated values 128-1023 may be used by others subject to
       the procedures and constraints described in Clause 23. */
    { 0, NULL }
};

const char *bactext_object_type_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_object_type_names, index, OBJECT_PROPRIETARY_MIN,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

bool bactext_object_type_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_object_type_names, search_name, found_index);
}

bool bactext_object_type_strtol(const char *search_name, unsigned *found_index)
{
    return bactext_strtol_index(
        bacnet_object_type_names, search_name, found_index);
}

INDTEXT_DATA bacnet_property_names[] = {
    /* FIXME: use the enumerations from bacenum.h */
    { PROP_ACKED_TRANSITIONS, "acked-transitions" },
    { PROP_ACK_REQUIRED, "ack-required" },
    { PROP_ACTION, "action" },
    { PROP_ACTION_TEXT, "action-text" },
    { PROP_ACTIVE_TEXT, "active-text" },
    { PROP_ACTIVE_VT_SESSIONS, "active-vt-sessions" },
    { PROP_ALARM_VALUE, "alarm-value" },
    { PROP_ALARM_VALUES, "alarm-values" },
    { PROP_ALL, "all" },
    { PROP_ALL_WRITES_SUCCESSFUL, "all-writes-successful" },
    { PROP_APDU_SEGMENT_TIMEOUT, "apdu-segment-timeout" },
    { PROP_APDU_TIMEOUT, "apdu-timeout" },
    { PROP_APPLICATION_SOFTWARE_VERSION, "application-software-version" },
    { PROP_ARCHIVE, "archive" },
    { PROP_BIAS, "bias" },
    { PROP_CHANGE_OF_STATE_COUNT, "change-of-state-count" },
    { PROP_CHANGE_OF_STATE_TIME, "change-of-state-time" },
    { PROP_NOTIFICATION_CLASS, "notification-class" },
    { PROP_BLANK_1, "(deleted in 135-2001)" },
    { PROP_CONTROLLED_VARIABLE_REFERENCE, "controlled-variable-reference" },
    { PROP_CONTROLLED_VARIABLE_UNITS, "controlled-variable-units" },
    { PROP_CONTROLLED_VARIABLE_VALUE, "controlled-variable-value" },
    { PROP_COV_INCREMENT, "cov-increment" },
    { PROP_DATE_LIST, "datelist" },
    { PROP_DAYLIGHT_SAVINGS_STATUS, "daylight-savings-status" },
    { PROP_DEADBAND, "deadband" },
    { PROP_DERIVATIVE_CONSTANT, "derivative-constant" },
    { PROP_DERIVATIVE_CONSTANT_UNITS, "derivative-constant-units" },
    { PROP_DESCRIPTION, "description" },
    { PROP_DESCRIPTION_OF_HALT, "description-of-halt" },
    { PROP_DEVICE_ADDRESS_BINDING, "device-address-binding" },
    { PROP_DEVICE_TYPE, "device-type" },
    { PROP_EFFECTIVE_PERIOD, "effective-period" },
    { PROP_ELAPSED_ACTIVE_TIME, "elapsed-active-time" },
    { PROP_ERROR_LIMIT, "error-limit" },
    { PROP_EVENT_ENABLE, "event-enable" },
    { PROP_EVENT_STATE, "event-state" },
    { PROP_EVENT_TYPE, "event-type" },
    { PROP_EXCEPTION_SCHEDULE, "exception-schedule" },
    { PROP_FAULT_VALUES, "fault-values" },
    { PROP_FEEDBACK_VALUE, "feedback-value" },
    { PROP_FILE_ACCESS_METHOD, "file-access-method" },
    { PROP_FILE_SIZE, "file-size" },
    { PROP_FILE_TYPE, "file-type" },
    { PROP_FIRMWARE_REVISION,
      "firmware-revision" }, /* VTS wants "revision", not "version" */
    { PROP_HIGH_LIMIT, "high-limit" },
    { PROP_INACTIVE_TEXT, "inactive-text" },
    { PROP_IN_PROCESS, "in-process" },
    { PROP_INSTANCE_OF, "instance-of" },
    { PROP_INTEGRAL_CONSTANT, "integral-constant" },
    { PROP_INTEGRAL_CONSTANT_UNITS, "integral-constant-units" },
    { PROP_ISSUE_CONFIRMED_NOTIFICATIONS, "issue-confirmednotifications" },
    { PROP_LIMIT_ENABLE, "limit-enable" },
    { PROP_LIST_OF_GROUP_MEMBERS, "list-of-group-members" },
    { PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES,
      "list-of-object-property-references" },
    { PROP_LIST_OF_SESSION_KEYS, "list-of-session-keys" },
    { PROP_LOCAL_DATE, "local-date" },
    { PROP_LOCAL_TIME, "local-time" },
    { PROP_LOCATION, "location" },
    { PROP_LOW_LIMIT, "low-limit" },
    { PROP_MANIPULATED_VARIABLE_REFERENCE, "manipulated-variable-reference" },
    { PROP_MAXIMUM_OUTPUT, "maximum-output" },
    { PROP_MAX_APDU_LENGTH_ACCEPTED, "max-apdu-length-accepted" },
    { PROP_MAX_INFO_FRAMES, "max-info-frames" },
    { PROP_MAX_MASTER, "max-master" },
    { PROP_MAX_PRES_VALUE, "max-pres-value" },
    { PROP_MINIMUM_OFF_TIME, "minimum-off-time" },
    { PROP_MINIMUM_ON_TIME, "minimum-on-time" },
    { PROP_MINIMUM_OUTPUT, "minimum-output" },
    { PROP_MIN_PRES_VALUE, "min-pres-value" },
    { PROP_MODEL_NAME, "model-name" },
    { PROP_MODIFICATION_DATE, "modification-date" },
    { PROP_NOTIFY_TYPE, "notify-type" },
    { PROP_NUMBER_OF_APDU_RETRIES, "number-of-APDU-retries" },
    { PROP_NUMBER_OF_STATES, "number-of-states" },
    { PROP_OBJECT_IDENTIFIER, "object-identifier" },
    { PROP_OBJECT_LIST, "object-list" },
    { PROP_OBJECT_NAME, "object-name" },
    { PROP_OBJECT_PROPERTY_REFERENCE, "object-property-reference" },
    { PROP_OBJECT_TYPE, "object-type" },
    { PROP_OPTIONAL, "optional" },
    { PROP_OUT_OF_SERVICE, "out-of-service" },
    { PROP_OUTPUT_UNITS, "output-units" },
    { PROP_EVENT_PARAMETERS, "event-parameters" },
    { PROP_POLARITY, "polarity" },
    { PROP_PRESENT_VALUE, "present-value" },
    { PROP_PRIORITY, "priority" },
    { PROP_PRIORITY_ARRAY, "priority-array" },
    { PROP_PRIORITY_FOR_WRITING, "priority-for-writing" },
    { PROP_PROCESS_IDENTIFIER, "process-identifier" },
    { PROP_PROGRAM_CHANGE, "program-change" },
    { PROP_PROGRAM_LOCATION, "program-location" },
    { PROP_PROGRAM_STATE, "program-state" },
    { PROP_PROPORTIONAL_CONSTANT, "proportional-constant" },
    { PROP_PROPORTIONAL_CONSTANT_UNITS, "proportional-constant-units" },
    { PROP_PROTOCOL_CONFORMANCE_CLASS, "protocol-conformance-class" },
    { PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED, "protocol-object-types-supported" },
    { PROP_PROTOCOL_SERVICES_SUPPORTED, "protocol-services-supported" },
    { PROP_PROTOCOL_VERSION, "protocol-version" },
    { PROP_READ_ONLY, "read-only" },
    { PROP_REASON_FOR_HALT, "reason-for-halt" },
    { PROP_RECIPIENT, "recipient" },
    { PROP_RECIPIENT_LIST, "recipient-list" },
    { PROP_RELIABILITY, "reliability" },
    { PROP_RELINQUISH_DEFAULT, "relinquish-default" },
    { PROP_REQUIRED, "required" },
    { PROP_RESOLUTION, "resolution" },
    { PROP_SEGMENTATION_SUPPORTED, "segmentation-supported" },
    { PROP_SETPOINT, "setpoint" },
    { PROP_SETPOINT_REFERENCE, "setpoint-reference" },
    { PROP_STATE_TEXT, "state-text" },
    { PROP_STATUS_FLAGS, "status-flags" },
    { PROP_SYSTEM_STATUS, "system-status" },
    { PROP_TIME_DELAY, "time-delay" },
    { PROP_TIME_OF_ACTIVE_TIME_RESET, "time-of-active-time-reset" },
    { PROP_TIME_OF_STATE_COUNT_RESET, "time-of-state-count-reset" },
    { PROP_TIME_SYNCHRONIZATION_RECIPIENTS, "time-synchronization-recipients" },
    { PROP_UNITS, "units" },
    { PROP_UPDATE_INTERVAL, "update-interval" },
    { PROP_UTC_OFFSET, "utc-offset" },
    { PROP_VENDOR_IDENTIFIER, "vendor-identifier" },
    { PROP_VENDOR_NAME, "vendor-name" },
    { PROP_VT_CLASSES_SUPPORTED, "vt-classes-supported" },
    { PROP_WEEKLY_SCHEDULE, "weekly-schedule" },
    { PROP_ATTEMPTED_SAMPLES, "attempted-samples" },
    { PROP_AVERAGE_VALUE, "average-value" },
    { PROP_BUFFER_SIZE, "buffer-size" },
    { PROP_CLIENT_COV_INCREMENT, "client-cov-increment" },
    { PROP_COV_RESUBSCRIPTION_INTERVAL, "cov-resubscription-interval" },
    { PROP_CURRENT_NOTIFY_TIME, "current-notify-time" },
    { PROP_EVENT_TIME_STAMPS, "event-time-stamps" },
    { PROP_LOG_BUFFER, "log-buffer" },
    { PROP_LOG_DEVICE_OBJECT_PROPERTY, "log-device-object-property" },
    { PROP_ENABLE, "enable" },
    { PROP_LOG_INTERVAL, "log-interval" },
    { PROP_MAXIMUM_VALUE, "maximum-value" },
    { PROP_MINIMUM_VALUE, "minimum-value" },
    { PROP_NOTIFICATION_THRESHOLD, "notification-threshold" },
    { PROP_PREVIOUS_NOTIFY_TIME, "previous-notify-time" },
    { PROP_PROTOCOL_REVISION, "protocol-revision" },
    { PROP_RECORDS_SINCE_NOTIFICATION, "records-since-notification" },
    { PROP_RECORD_COUNT, "record-count" },
    { PROP_START_TIME, "start-time" },
    { PROP_STOP_TIME, "stop-time" },
    { PROP_STOP_WHEN_FULL, "stop-when-full" },
    { PROP_TOTAL_RECORD_COUNT, "total-record-count" },
    { PROP_VALID_SAMPLES, "valid-samples" },
    { PROP_WINDOW_INTERVAL, "window-interval" },
    { PROP_WINDOW_SAMPLES, "window-samples" },
    { PROP_MAXIMUM_VALUE_TIMESTAMP, "maximum-value-timestamp" },
    { PROP_MINIMUM_VALUE_TIMESTAMP, "minimum-value-timestamp" },
    { PROP_VARIANCE_VALUE, "variance-value" },
    { PROP_ACTIVE_COV_SUBSCRIPTIONS, "active-cov-subscriptions" },
    { PROP_BACKUP_FAILURE_TIMEOUT, "backup-failure-timeout" },
    { PROP_CONFIGURATION_FILES, "configuration-files" },
    { PROP_DATABASE_REVISION, "database-revision" },
    { PROP_DIRECT_READING, "direct-reading" },
    { PROP_LAST_RESTORE_TIME, "last-restore-time" },
    { PROP_MAINTENANCE_REQUIRED, "maintenance-required" },
    { PROP_MEMBER_OF, "member-of" },
    { PROP_MODE, "mode" },
    { PROP_OPERATION_EXPECTED, "operation-expected" },
    { PROP_SETTING, "setting" },
    { PROP_SILENCED, "silenced" },
    { PROP_TRACKING_VALUE, "tracking-value" },
    { PROP_ZONE_MEMBERS, "zone-members" },
    { PROP_LIFE_SAFETY_ALARM_VALUES, "life-safety-alarm-values" },
    { PROP_MAX_SEGMENTS_ACCEPTED, "max-segments-accepted" },
    { PROP_PROFILE_NAME, "profile-name" },
    { PROP_AUTO_SLAVE_DISCOVERY, "auto-slave-discovery" },
    { PROP_MANUAL_SLAVE_ADDRESS_BINDING, "manual-slave-address-binding" },
    { PROP_SLAVE_ADDRESS_BINDING, "slave-address-binding" },
    { PROP_SLAVE_PROXY_ENABLE, "slave-proxy-enable" },
    { PROP_LAST_NOTIFY_RECORD, "last-notify-record" },
    { PROP_SCHEDULE_DEFAULT, "schedule-default" },
    { PROP_ACCEPTED_MODES, "accepted-modes" },
    { PROP_ADJUST_VALUE, "adjust-value" },
    { PROP_COUNT, "count" },
    { PROP_COUNT_BEFORE_CHANGE, "count-before-change" },
    { PROP_COUNT_CHANGE_TIME, "count-change-time" },
    { PROP_COV_PERIOD, "COV-period" },
    { PROP_INPUT_REFERENCE, "input-reference" },
    { PROP_LIMIT_MONITORING_INTERVAL, "limit-monitoring-interval" },
    { PROP_LOGGING_OBJECT, "logging-object" },
    { PROP_LOGGING_RECORD, "logging-record" },
    { PROP_PRESCALE, "prescale" },
    { PROP_PULSE_RATE, "pulse-rate" },
    { PROP_SCALE, "scale" },
    { PROP_SCALE_FACTOR, "scale-factor" },
    { PROP_UPDATE_TIME, "update-time" },
    { PROP_VALUE_BEFORE_CHANGE, "value-before-change" },
    { PROP_VALUE_SET, "value-set" },
    { PROP_VALUE_CHANGE_TIME, "value-change-time" },
    { PROP_ALIGN_INTERVALS, "align-intervals" },
    { PROP_INTERVAL_OFFSET, "interval-offset" },
    { PROP_LAST_RESTART_REASON, "last-restart-reason" },
    { PROP_LOGGING_TYPE, "logging-type" },
    { PROP_TIME_OF_DEVICE_RESTART, "time-of-device-restart" },
    { PROP_TIME_SYNCHRONIZATION_INTERVAL, "time-synchronization-interval" },
    { PROP_TRIGGER, "trigger" },
    { PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS,
      "utc-time-synchronization-recipients" },
    { PROP_NODE_SUBTYPE, "node-subtype" },
    { PROP_NODE_TYPE, "node-type" },
    { PROP_STRUCTURED_OBJECT_LIST, "structured-object-list" },
    { PROP_SUBORDINATE_ANNOTATIONS, "subordinate-annotations" },
    { PROP_SUBORDINATE_LIST, "subordinate-list" },
    { PROP_ACTUAL_SHED_LEVEL, "actual-shed-level" },
    { PROP_DUTY_WINDOW, "duty-window" },
    { PROP_EXPECTED_SHED_LEVEL, "expected-shed-level" },
    { PROP_FULL_DUTY_BASELINE, "full-duty-baseline" },
    { PROP_REQUESTED_SHED_LEVEL, "requested-shed-level" },
    { PROP_SHED_DURATION, "shed-duration" },
    { PROP_SHED_LEVEL_DESCRIPTIONS, "shed-level-descriptions" },
    { PROP_SHED_LEVELS, "shed-levels" },
    { PROP_STATE_DESCRIPTION, "state-description" },
    { PROP_DOOR_ALARM_STATE, "door-alarm-state" },
    { PROP_DOOR_EXTENDED_PULSE_TIME, "door-extended-pulse-time" },
    { PROP_DOOR_MEMBERS, "door-members" },
    { PROP_DOOR_OPEN_TOO_LONG_TIME, "door-open-too-long-time" },
    { PROP_DOOR_PULSE_TIME, "door-pulse-time" },
    { PROP_DOOR_STATUS, "door-status" },
    { PROP_DOOR_UNLOCK_DELAY_TIME, "door-unlock-delay-time" },
    { PROP_LOCK_STATUS, "lock-status" },
    { PROP_MASKED_ALARM_VALUES, "masked-alarm-values" },
    { PROP_SECURED_STATUS, "secured-status" },
    { PROP_ABSENTEE_LIMIT, "absentee-limit" },
    { PROP_ACCESS_ALARM_EVENTS, "access-alarm-events" },
    { PROP_ACCESS_DOORS, "access-doors" },
    { PROP_ACCESS_EVENT, "access-event" },
    { PROP_ACCESS_EVENT_AUTHENTICATION_FACTOR,
      "access-event-authentication-factor" },
    { PROP_ACCESS_EVENT_CREDENTIAL, "access-event-credential" },
    { PROP_ACCESS_EVENT_TIME, "access-event-time" },
    { PROP_ACCESS_TRANSACTION_EVENTS, "access-transaction-events" },
    { PROP_ACCOMPANIMENT, "accompaniment" },
    { PROP_ACCOMPANIMENT_TIME, "accompaniment-time" },
    { PROP_ACTIVATION_TIME, "activation-time" },
    { PROP_ACTIVE_AUTHENTICATION_POLICY, "active-authentication-policy" },
    { PROP_ASSIGNED_ACCESS_RIGHTS, "assigned-access-rights" },
    { PROP_AUTHENTICATION_FACTORS, "authentication-factors" },
    { PROP_AUTHENTICATION_POLICY_LIST, "authentication-policy-list" },
    { PROP_AUTHENTICATION_POLICY_NAMES, "authentication-policy-names" },
    { PROP_AUTHENTICATION_STATUS, "authentication-status" },
    { PROP_AUTHORIZATION_MODE, "authorization-mode" },
    { PROP_BELONGS_TO, "belongs-to" },
    { PROP_CREDENTIAL_DISABLE, "credential-disable" },
    { PROP_CREDENTIAL_STATUS, "credential-status" },
    { PROP_CREDENTIALS, "credentials" },
    { PROP_CREDENTIALS_IN_ZONE, "credentials-in-zone" },
    { PROP_DAYS_REMAINING, "days-remaining" },
    { PROP_ENTRY_POINTS, "entry-points" },
    { PROP_EXIT_POINTS, "exit-points" },
    { PROP_EXPIRATION_TIME, "expiration-time" },
    { PROP_EXTENDED_TIME_ENABLE, "extended-time-enable" },
    { PROP_FAILED_ATTEMPT_EVENTS, "failed-attempt-events" },
    { PROP_FAILED_ATTEMPTS, "failed-attempts" },
    { PROP_FAILED_ATTEMPTS_TIME, "failed-attempts-time" },
    { PROP_LAST_ACCESS_EVENT, "last-access-event" },
    { PROP_LAST_ACCESS_POINT, "last-access-point" },
    { PROP_LAST_CREDENTIAL_ADDED, "last-credential-added" },
    { PROP_LAST_CREDENTIAL_ADDED_TIME, "last-credential-added-time" },
    { PROP_LAST_CREDENTIAL_REMOVED, "last-credential-removed" },
    { PROP_LAST_CREDENTIAL_REMOVED_TIME, "last-credential-removed-time" },
    { PROP_LAST_USE_TIME, "last-use-time" },
    { PROP_LOCKOUT, "lockout" },
    { PROP_LOCKOUT_RELINQUISH_TIME, "lockout-relinquish-time" },
    { PROP_MASTER_EXEMPTION, "master-exemption" },
    { PROP_MAX_FAILED_ATTEMPTS, "max-failed-attempts" },
    { PROP_MEMBERS, "members" },
    { PROP_MUSTER_POINT, "muster-point" },
    { PROP_NEGATIVE_ACCESS_RULES, "negative-access-rules" },
    { PROP_NUMBER_OF_AUTHENTICATION_POLICIES,
      "number-of-authentication-policies" },
    { PROP_OCCUPANCY_COUNT, "occupancy-count" },
    { PROP_OCCUPANCY_COUNT_ADJUST, "occupancy-count-adjust" },
    { PROP_OCCUPANCY_COUNT_ENABLE, "occupancy-count-enable" },
    { PROP_OCCUPANCY_EXEMPTION, "occupancy-exemption" },
    { PROP_OCCUPANCY_LOWER_LIMIT, "occupancy-lower-limit" },
    { PROP_OCCUPANCY_LOWER_LIMIT_ENFORCED, "occupancy-lower-limit-enforced" },
    { PROP_OCCUPANCY_STATE, "occupancy-state" },
    { PROP_OCCUPANCY_UPPER_LIMIT, "occupancy-upper-limit" },
    { PROP_OCCUPANCY_UPPER_LIMIT_ENFORCED, "occupancy-upper-limit-enforced" },
    { PROP_PASSBACK_EXEMPTION, "passback-exemption" },
    { PROP_PASSBACK_MODE, "passback-mode" },
    { PROP_PASSBACK_TIMEOUT, "passback-timeout" },
    { PROP_POSITIVE_ACCESS_RULES, "positive-access-rules" },
    { PROP_REASON_FOR_DISABLE, "reason-for-disable" },
    { PROP_SUPPORTED_FORMATS, "supported-formats" },
    { PROP_SUPPORTED_FORMAT_CLASSES, "supported-format-classes" },
    { PROP_THREAT_AUTHORITY, "threat-authority" },
    { PROP_THREAT_LEVEL, "threat-level" },
    { PROP_TRACE_FLAG, "trace-flag" },
    { PROP_TRANSACTION_NOTIFICATION_CLASS, "transaction-notification-class" },
    { PROP_USER_EXTERNAL_IDENTIFIER, "user-external-identifier" },
    { PROP_USER_INFORMATION_REFERENCE, "user-information-reference" },
    { PROP_USER_INFORMATION_REFERENCE, "user-information-reference" },
    { PROP_USER_NAME, "user-name" },
    { PROP_USER_TYPE, "user-type" },
    { PROP_USES_REMAINING, "uses-remaining" },
    { PROP_ZONE_FROM, "zone-from" },
    { PROP_ZONE_TO, "zone-to" },
    { PROP_VERIFICATION_TIME, "verification-time" },
    { PROP_BASE_DEVICE_SECURITY_POLICY, "base-device-security-policy" },
    { PROP_DISTRIBUTION_KEY_REVISION, "distribution-key-revision" },
    { PROP_DO_NOT_HIDE, "do-not-hide" },
    { PROP_KEY_SETS, "key-sets" },
    { PROP_LAST_KEY_SERVER, "last-key-server" },
    { PROP_NETWORK_ACCESS_SECURITY_POLICIES,
      "network-access-security-policies" },
    { PROP_PACKET_REORDER_TIME, "packet-reorder-time" },
    { PROP_SECURITY_PDU_TIMEOUT, "security-pdu-timeout" },
    { PROP_SECURITY_TIME_WINDOW, "security-time-window" },
    { PROP_SUPPORTED_SECURITY_ALGORITHM, "supported-security-algorithm" },
    { PROP_UPDATE_KEY_SET_TIMEOUT, "update-key-set-timeout" },
    { PROP_BACKUP_AND_RESTORE_STATE, "backup-and-restore-state" },
    { PROP_BACKUP_PREPARATION_TIME, "backup-preparation-time" },
    { PROP_RESTORE_COMPLETION_TIME, "restore-completion-time" },
    { PROP_RESTORE_PREPARATION_TIME, "restore-preparation-time" },
    { PROP_BIT_MASK, "bit-mask" },
    { PROP_BIT_TEXT, "bit-text" },
    { PROP_IS_UTC, "is-utc" },
    { PROP_GROUP_MEMBERS, "group-members" },
    { PROP_GROUP_MEMBER_NAMES, "group-member-names" },
    { PROP_MEMBER_STATUS_FLAGS, "member-status-flags" },
    { PROP_REQUESTED_UPDATE_INTERVAL, "requested-update-interval" },
    { PROP_COVU_PERIOD, "covu-period" },
    { PROP_COVU_RECIPIENTS, "covu-recipients" },
    { PROP_EVENT_MESSAGE_TEXTS, "event-message-texts" },
    { PROP_EVENT_MESSAGE_TEXTS_CONFIG, "event-message-texts-config" },
    { PROP_EVENT_DETECTION_ENABLE, "event-detection-enable" },
    { PROP_EVENT_ALGORITHM_INHIBIT, "event-algorithm-inhibit" },
    { PROP_EVENT_ALGORITHM_INHIBIT_REF, "event-algorithm-inhibit-ref" },
    { PROP_TIME_DELAY_NORMAL, "time-delay-normal" },
    { PROP_RELIABILITY_EVALUATION_INHIBIT, "reliability-evaluation-inhibit" },
    { PROP_FAULT_PARAMETERS, "fault-parameters" },
    { PROP_FAULT_TYPE, "fault-type" },
    { PROP_LOCAL_FORWARDING_ONLY, "local-forwarding-only" },
    { PROP_PROCESS_IDENTIFIER_FILTER, "process-identifier-filter" },
    { PROP_SUBSCRIBED_RECIPIENTS, "subscribed-recipients" },
    { PROP_PORT_FILTER, "port-filter" },
    { PROP_AUTHORIZATION_EXEMPTIONS, "authorization-exemptions" },
    { PROP_ALLOW_GROUP_DELAY_INHIBIT, "allow-group-delay-inhibit" },
    { PROP_CHANNEL_NUMBER, "channel-number" },
    { PROP_CONTROL_GROUPS, "control-groups" },
    { PROP_EXECUTION_DELAY, "execution-delay" },
    { PROP_LAST_PRIORITY, "last-priority" },
    { PROP_WRITE_STATUS, "write-status" },
    { PROP_PROPERTY_LIST, "property-list" },
    { PROP_SERIAL_NUMBER, "serial-number" },
    { PROP_BLINK_WARN_ENABLE, "blink-warn-enable" },
    { PROP_DEFAULT_FADE_TIME, "default-fade-time" },
    { PROP_DEFAULT_RAMP_RATE, "default-ramp-rate" },
    { PROP_DEFAULT_STEP_INCREMENT, "default-step-increment" },
    { PROP_EGRESS_TIME, "egress-time" },
    { PROP_IN_PROGRESS, "in-progress" },
    { PROP_INSTANTANEOUS_POWER, "instantaneous-power" },
    { PROP_LIGHTING_COMMAND, "lighting-command" },
    { PROP_LIGHTING_COMMAND_DEFAULT_PRIORITY,
      "lighting-command-default-priority" },
    { PROP_MAX_ACTUAL_VALUE, "max-actual-value" },
    { PROP_MIN_ACTUAL_VALUE, "min-actual-value" },
    { PROP_POWER, "power" },
    { PROP_TRANSITION, "transition" },
    { PROP_EGRESS_ACTIVE, "egress-active" },
    { PROP_INTERFACE_VALUE, "inteface-value" },
    { PROP_FAULT_HIGH_LIMIT, "fault-high-limit" },
    { PROP_FAULT_LOW_LIMIT, "fault-low-limit" },
    { PROP_LOW_DIFF_LIMIT, "low-diff-limit" },
    { PROP_STRIKE_COUNT, "strike-count" },
    { PROP_TIME_OF_STRIKE_COUNT_RESET, "strike-count" },
    { PROP_DEFAULT_TIMEOUT, "default-timeout" },
    { PROP_INITIAL_TIMEOUT, "initial-timeout" },
    { PROP_LAST_STATE_CHANGE, "last-state-change" },
    { PROP_STATE_CHANGE_VALUES, "state-change-values" },
    { PROP_TIMER_RUNNING, "timer-running" },
    { PROP_TIMER_STATE, "timer-state" },
    { PROP_APDU_LENGTH, "apdu-length" },
    { PROP_IP_ADDRESS, "ip-address" },
    { PROP_IP_DEFAULT_GATEWAY, "ip-default-gateway" },
    { PROP_IP_DHCP_ENABLE, "ip-dhcp-enable" },
    { PROP_IP_DHCP_LEASE_TIME, "ip-dhcp-lease-time" },
    { PROP_IP_DHCP_LEASE_TIME_REMAINING, "ip-dhcp-lease-time-remaining" },
    { PROP_IP_DHCP_SERVER, "ip-dhcp-server" },
    { PROP_IP_DNS_SERVER, "ip-dns-server" },
    { PROP_BACNET_IP_GLOBAL_ADDRESS, "bacnet-ip-global-address" },
    { PROP_BACNET_IP_MODE, "bacnet-ip-mode" },
    { PROP_BACNET_IP_MULTICAST_ADDRESS, "bacnet-ip-multicast-address" },
    { PROP_BACNET_IP_NAT_TRAVERSAL, "bacnet-ip-nat-traversal" },
    { PROP_IP_SUBNET_MASK, "ip-subnet-mask" },
    { PROP_BACNET_IP_UDP_PORT, "bacnet-ip-udp-port" },
    { PROP_BBMD_ACCEPT_FD_REGISTRATIONS, "bbmd-accept-fd-registrations" },
    { PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE,
      "bbmd-broadcast-distribution-table" },
    { PROP_BBMD_FOREIGN_DEVICE_TABLE, "bbmd-foreign-device-table" },
    { PROP_CHANGES_PENDING, "changes-pending" },
    { PROP_COMMAND, "command" },
    { PROP_FD_BBMD_ADDRESS, "fd-bbmd-address" },
    { PROP_FD_SUBSCRIPTION_LIFETIME, "fd-subscription-lifetime" },
    { PROP_LINK_SPEED, "link-speed" },
    { PROP_LINK_SPEEDS, "link-speeds" },
    { PROP_LINK_SPEED_AUTONEGOTIATE, "link-speed-autonegotiate" },
    { PROP_MAC_ADDRESS, "mac-address" },
    { PROP_NETWORK_INTERFACE_NAME, "network-interface-name" },
    { PROP_NETWORK_NUMBER, "network-number" },
    { PROP_NETWORK_NUMBER_QUALITY, "network-number-quality" },
    { PROP_NETWORK_TYPE, "network-type" },
    { PROP_ROUTING_TABLE, "routing-table" },
    { PROP_VIRTUAL_MAC_ADDRESS_TABLE, "virtual-mac-address-table" },
    { PROP_COMMAND_TIME_ARRAY, "command-time-array" },
    { PROP_CURRENT_COMMAND_PRIORITY, "current-command-priority" },
    { PROP_LAST_COMMAND_TIME, "last-command-time" },
    { PROP_VALUE_SOURCE, "value-source" },
    { PROP_VALUE_SOURCE_ARRAY, "value-source-array" },
    { PROP_BACNET_IPV6_MODE, "bacnet-ipv6-mode" },
    { PROP_IPV6_ADDRESS, "ipv6-address" },
    { PROP_IPV6_PREFIX_LENGTH, "ipv6-prefix-length" },
    { PROP_BACNET_IPV6_UDP_PORT, "bacnet-ipv6-udp-port" },
    { PROP_IPV6_DEFAULT_GATEWAY, "ipv6-default-gateway" },
    { PROP_BACNET_IPV6_MULTICAST_ADDRESS, "bacnet-ipv6-multicast-address" },
    { PROP_IPV6_DNS_SERVER, "ipv6-dns-server" },
    { PROP_IPV6_AUTO_ADDRESSING_ENABLE, "ipv6-auto-addressing-enable" },
    { PROP_IPV6_DHCP_LEASE_TIME, "ipv6-dhcp-lease-time" },
    { PROP_IPV6_DHCP_LEASE_TIME_REMAINING, "ipv6-dhcp-lease-time-remaining" },
    { PROP_IPV6_DHCP_SERVER, "ipv6-dhcp-server" },
    { PROP_IPV6_ZONE_INDEX, "ipv6-zone-index" },
    { PROP_ASSIGNED_LANDING_CALLS, "assigned-landing-calls" },
    { PROP_CAR_ASSIGNED_DIRECTION, "car-assigned-direction" },
    { PROP_CAR_DOOR_COMMAND, "car-door-command" },
    { PROP_CAR_DOOR_STATUS, "car-door-status" },
    { PROP_CAR_DOOR_TEXT, "car-door-text" },
    { PROP_CAR_DOOR_ZONE, "car-door-zone" },
    { PROP_CAR_DRIVE_STATUS, "car-drive-status" },
    { PROP_CAR_LOAD, "car-load" },
    { PROP_CAR_LOAD_UNITS, "car-load-units" },
    { PROP_CAR_MODE, "car-mode" },
    { PROP_CAR_MOVING_DIRECTION, "car-moving-direction" },
    { PROP_CAR_POSITION, "car-position" },
    { PROP_ELEVATOR_GROUP, "elevator-group" },
    { PROP_ENERGY_METER, "energy-meter" },
    { PROP_ENERGY_METER_REF, "energy-meter-ref" },
    { PROP_ESCALATOR_MODE, "escalator-mode" },
    { PROP_FAULT_SIGNALS, "fault-signals" },
    { PROP_FLOOR_TEXT, "floor-text" },
    { PROP_GROUP_ID, "group-id" },
    { PROP_GROUP_MODE, "group-mode" },
    { PROP_HIGHER_DECK, "higher-deck" },
    { PROP_INSTALLATION_ID, "installation-id" },
    { PROP_LANDING_CALLS, "landing-calls" },
    { PROP_LANDING_CALL_CONTROL, "landing-call-control" },
    { PROP_LANDING_DOOR_STATUS, "landing-door-status" },
    { PROP_LOWER_DECK, "lower-deck" },
    { PROP_MACHINE_ROOM_ID, "machine-room-id" },
    { PROP_MAKING_CAR_CALL, "making-car-call" },
    { PROP_NEXT_STOPPING_FLOOR, "next-stopping-floor" },
    { PROP_OPERATION_DIRECTION, "operation-direction" },
    { PROP_PASSENGER_ALARM, "passenger-alarm" },
    { PROP_POWER_MODE, "power-mode" },
    { PROP_REGISTERED_CAR_CALL, "registered-car-call" },
    { PROP_ACTIVE_COV_MULTIPLE_SUBSCRIPTIONS,
      "active-cov-multiple-subscriptions" },
    { PROP_PROTOCOL_LEVEL, "protocol-level" },
    { PROP_REFERENCE_PORT, "reference-port" },
    { PROP_DEPLOYED_PROFILE_LOCATION, "deployed-profile-location" },
    { PROP_PROFILE_LOCATION, "profile-location" },
    { PROP_TAGS, "tags" },
    { PROP_SUBORDINATE_NODE_TYPES, "subordinate-node-types" },
    { PROP_SUBORDINATE_TAGS, "subordinate-tags" },
    { PROP_SUBORDINATE_RELATIONSHIPS, "subordinate-relationships" },
    { PROP_DEFAULT_SUBORDINATE_RELATIONSHIP,
      "default-subordinate-relationship" },
    { PROP_REPRESENTS, "represents" },
    { PROP_DEFAULT_PRESENT_VALUE, "default-present-value" },
    { PROP_PRESENT_STAGE, "present-stage" },
    { PROP_STAGES, "stages" },
    { PROP_STAGE_NAMES, "stage-names" },
    { PROP_TARGET_REFERENCES, "target-references" },
    { PROP_AUDIT_SOURCE_LEVEL, "audit-source-level" },
    { PROP_AUDIT_LEVEL, "audit-level" },
    { PROP_AUDIT_NOTIFICATION_RECIPIENT, "audit-notification-recipient" },
    { PROP_AUDIT_PRIORITY_FILTER, "audit-priority-filter" },
    { PROP_AUDITABLE_OPERATIONS, "auditable-operations" },
    { PROP_DELETE_ON_FORWARD, "delete-on-forward" },
    { PROP_MAXIMUM_SEND_DELAY, "maximum-send-delay" },
    { PROP_MONITORED_OBJECTS, "monitored-objects" },
    { PROP_SEND_NOW, "send-now" },
    { PROP_FLOOR_NUMBER, "floor-number" },
    { PROP_DEVICE_UUID, "device-uuid" },
    { PROP_ADDITIONAL_REFERENCE_PORTS, "additional-reference-ports" },
    { PROP_CERTIFICATE_SIGNING_REQUEST_FILE,
      "certificate-signing-request-file" },
    { PROP_COMMAND_VALIDATION_RESULT, "command-validation-result" },
    { PROP_ISSUER_CERTIFICATE_FILES, "issuer-certificate-files" },
    { PROP_MAX_BVLC_LENGTH_ACCEPTED, "max-bvlc-length-accepted" },
    { PROP_MAX_NPDU_LENGTH_ACCEPTED, "max-npdu-length-accepted" },
    { PROP_OPERATIONAL_CERTIFICATE_FILE, "operational-certificate-file" },
    { PROP_CURRENT_HEALTH, "current-health" },
    { PROP_SC_CONNECT_WAIT_TIMEOUT, "sc-connect-wait-timeout" },
    { PROP_SC_DIRECT_CONNECT_ACCEPT_ENABLE, "sc-direct-connect-accept-enable" },
    { PROP_SC_DIRECT_CONNECT_ACCEPT_URIS, "sc-direct-connect-accept-uris" },
    { PROP_SC_DIRECT_CONNECT_BINDING, "sc-direct-connect-binding" },
    { PROP_SC_DIRECT_CONNECT_CONNECTION_STATUS,
      "sc-direct-connect-connection-status" },
    { PROP_SC_DIRECT_CONNECT_INITIATE_ENABLE,
      "sc-direct-connect-initiate-enable" },
    { PROP_SC_DISCONNECT_WAIT_TIMEOUT, "sc-disconnect-wait-timeout" },
    { PROP_SC_FAILED_CONNECTION_REQUESTS, "sc-failed-connection-requests" },
    { PROP_SC_FAILOVER_HUB_CONNECTION_STATUS,
      "sc-failover-hub-connection-status" },
    { PROP_SC_FAILOVER_HUB_URI, "sc-failover-hub-uri" },
    { PROP_SC_HUB_CONNECTOR_STATE, "sc-hub-connector-state" },
    { PROP_SC_HUB_FUNCTION_ACCEPT_URIS, "sc-hub-function-accept-uris" },
    { PROP_SC_HUB_FUNCTION_BINDING, "sc-hub-function-binding" },
    { PROP_SC_HUB_FUNCTION_CONNECTION_STATUS,
      "sc-hub-function-connection-status" },
    { PROP_SC_HUB_FUNCTION_ENABLE, "sc-hub-function-enable" },
    { PROP_SC_HEARTBEAT_TIMEOUT, "sc-heartbeat-timeout" },
    { PROP_SC_PRIMARY_HUB_CONNECTION_STATUS,
      "sc-primary-hub-connection-status" },
    { PROP_SC_PRIMARY_HUB_URI, "sc-primary-hub-uri" },
    { PROP_SC_MAXIMUM_RECONNECT_TIME, "sc-maximum-reconnect-time" },
    { PROP_SC_MINIMUM_RECONNECT_TIME, "sc-minimum-reconnect-time" },
    { PROP_COLOR_OVERRIDE, "color-override" },
    { PROP_COLOR_REFERENCE, "color-reference" },
    { PROP_DEFAULT_COLOR, "default-color" },
    { PROP_DEFAULT_COLOR_TEMPERATURE, "default-color-temperature" },
    { PROP_OVERRIDE_COLOR_REFERENCE, "override-color-reference" },
    { PROP_COLOR_COMMAND, "color-command" },
    { PROP_HIGH_END_TRIM, "high-end-trim" },
    { PROP_LOW_END_TRIM, "low-end-trim" },
    { PROP_TRIM_FADE_TIME, "trim-fade-time" },
    { PROP_DEVICE_ADDRESS_PROXY_ENABLE, "device-address-proxy-enable" },
    { PROP_DEVICE_ADDRESS_PROXY_TABLE, "device-address-proxy-table" },
    { PROP_DEVICE_ADDRESS_PROXY_TIMEOUT, "device-address-proxy-timeout" },
    { PROP_DEFAULT_ON_VALUE, "default-on-value" },
    { PROP_LAST_ON_VALUE, "last-on-value" },
    { PROP_AUTHORIZATION_CACHE, "authorization-cache" },
    { PROP_AUTHORIZATION_GROUPS, "authorization-groups" },
    { PROP_AUTHORIZATION_POLICY, "authorization-policy" },
    { PROP_AUTHORIZATION_SCOPE, "authorization-scope" },
    { PROP_AUTHORIZATION_SERVER, "authorization-server" },
    { PROP_AUTHORIZATION_STATUS, "authorization-status" },
    { PROP_MAX_PROXIED_I_AMS_PER_SECOND, "max-proxied-i-ams-per-second" },
    { 0, NULL }
};

bool bactext_property_name_proprietary(unsigned index)
{
    bool status = false;

    if ((index >= PROP_PROPRIETARY_RANGE_MIN) &&
        (index <= PROP_PROPRIETARY_RANGE_MAX)) {
        status = true;
    }

    return status;
}

const char *bactext_property_name(unsigned index)
{
    /* Enumerated values 0-511 are reserved for definition by ASHRAE.
       Enumerated values 512-4194303 may be used by others subject to the
       procedures and constraints described in Clause 23. */
    if (bactext_property_name_proprietary(index)) {
        return Vendor_Proprietary_String;
    } else {
        return indtext_by_index_default(
            bacnet_property_names, index, ASHRAE_Reserved_String);
    }
}

const char *
bactext_property_name_default(unsigned index, const char *default_string)
{
    return indtext_by_index_default(
        bacnet_property_names, index, default_string);
}

unsigned bactext_property_id(const char *name)
{
    return indtext_by_istring_default(bacnet_property_names, name, 0);
}

bool bactext_property_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(bacnet_property_names, search_name, found_index);
}

bool bactext_property_strtol(const char *search_name, unsigned *found_index)
{
    return bactext_strtol_index(
        bacnet_property_names, search_name, found_index);
}

INDTEXT_DATA bacnet_engineering_unit_names[] = {
    { UNITS_SQUARE_METERS, "square-meters" },
    { UNITS_SQUARE_FEET, "square-feet" },
    { UNITS_MILLIAMPERES, "milliamperes" },
    { UNITS_AMPERES, "amperes" },
    { UNITS_OHMS, "ohms" },
    { UNITS_VOLTS, "volts" },
    { UNITS_KILOVOLTS, "kilovolts" },
    { UNITS_MEGAVOLTS, "megavolts" },
    { UNITS_VOLT_AMPERES, "volt-amperes" },
    { UNITS_KILOVOLT_AMPERES, "kilovolt-amperes" },
    { UNITS_MEGAVOLT_AMPERES, "megavolt-amperes" },
    { UNITS_VOLT_AMPERES_REACTIVE, "volt-amperes-reactive" },
    { UNITS_KILOVOLT_AMPERES_REACTIVE, "kilovolt-amperes-reactive" },
    { UNITS_MEGAVOLT_AMPERES_REACTIVE, "megavolt-amperes-reactive" },
    { UNITS_DEGREES_PHASE, "degrees-phase" },
    { UNITS_POWER_FACTOR, "power-factor" },
    { UNITS_JOULES, "joules" },
    { UNITS_KILOJOULES, "kilojoules" },
    { UNITS_WATT_HOURS, "watt-hours" },
    { UNITS_KILOWATT_HOURS, "kilowatt-hours" },
    { UNITS_BTUS, "btus" },
    { UNITS_THERMS, "therms" },
    { UNITS_TON_HOURS, "ton-hours" },
    { UNITS_JOULES_PER_KILOGRAM_DRY_AIR, "joules-per-kilogram-dry-air" },
    { UNITS_BTUS_PER_POUND_DRY_AIR, "btus-per-pound-dry-air" },
    { UNITS_CYCLES_PER_HOUR, "cycles-per-hour" },
    { UNITS_CYCLES_PER_MINUTE, "cycles-per-minute" },
    { UNITS_HERTZ, "hertz" },
    { UNITS_GRAMS_OF_WATER_PER_KILOGRAM_DRY_AIR,
      "grams-of-water-per-kilogram-dry-air" },
    { UNITS_PERCENT_RELATIVE_HUMIDITY, "percent-relative-humidity" },
    { UNITS_MILLIMETERS, "millimeters" },
    { UNITS_METERS, "meters" },
    { UNITS_INCHES, "inches" },
    { UNITS_FEET, "feet" },
    { UNITS_WATTS_PER_SQUARE_FOOT, "watts-per-square-foot" },
    { UNITS_WATTS_PER_SQUARE_METER, "watts-per-square-meter" },
    { UNITS_LUMENS, "lumens" },
    { UNITS_LUXES, "luxes" },
    { UNITS_FOOT_CANDLES, "foot-candles" },
    { UNITS_KILOGRAMS, "kilograms" },
    { UNITS_POUNDS_MASS, "pounds-mass" },
    { UNITS_TONS, "tons" },
    { UNITS_KILOGRAMS_PER_SECOND, "kilograms-per-second" },
    { UNITS_KILOGRAMS_PER_MINUTE, "kilograms-per-minute" },
    { UNITS_KILOGRAMS_PER_HOUR, "kilograms-per-hour" },
    { UNITS_POUNDS_MASS_PER_MINUTE, "pounds-mass-per-minute" },
    { UNITS_POUNDS_MASS_PER_HOUR, "pounds-mass-per-hour" },
    { UNITS_WATTS, "watts" },
    { UNITS_KILOWATTS, "kilowatts" },
    { UNITS_MEGAWATTS, "megawatts" },
    { UNITS_BTUS_PER_HOUR, "btus-per-hour" },
    { UNITS_HORSEPOWER, "horsepower" },
    { UNITS_TONS_REFRIGERATION, "tons-refrigeration" },
    { UNITS_PASCALS, "pascals" },
    { UNITS_KILOPASCALS, "kilopascals" },
    { UNITS_BARS, "bars" },
    { UNITS_POUNDS_FORCE_PER_SQUARE_INCH, "pounds-force-per-square-inch" },
    { UNITS_CENTIMETERS_OF_WATER, "centimeters-of-water" },
    { UNITS_INCHES_OF_WATER, "inches-of-water" },
    { UNITS_MILLIMETERS_OF_MERCURY, "millimeters-of-mercury" },
    { UNITS_CENTIMETERS_OF_MERCURY, "centimeters-of-mercury" },
    { UNITS_INCHES_OF_MERCURY, "inches-of-mercury" },
    { UNITS_DEGREES_CELSIUS, "degrees-celsius" },
    { UNITS_KELVIN, "kelvin" },
    { UNITS_DEGREES_FAHRENHEIT, "degrees-fahrenheit" },
    { UNITS_DEGREE_DAYS_CELSIUS, "degree-days-celsius" },
    { UNITS_DEGREE_DAYS_FAHRENHEIT, "degree-days-fahrenheit" },
    { UNITS_YEARS, "years" },
    { UNITS_MONTHS, "months" },
    { UNITS_WEEKS, "weeks" },
    { UNITS_DAYS, "days" },
    { UNITS_HOURS, "hours" },
    { UNITS_MINUTES, "minutes" },
    { UNITS_SECONDS, "seconds" },
    { UNITS_METERS_PER_SECOND, "meters-per-second" },
    { UNITS_KILOMETERS_PER_HOUR, "kilometers-per-hour" },
    { UNITS_FEET_PER_SECOND, "feet-per-second" },
    { UNITS_FEET_PER_MINUTE, "feet-per-minute" },
    { UNITS_MILES_PER_HOUR, "miles-per-hour" },
    { UNITS_CUBIC_FEET, "cubic-feet" },
    { UNITS_CUBIC_METERS, "cubic-meters" },
    { UNITS_IMPERIAL_GALLONS, "imperial-gallons" },
    { UNITS_LITERS, "liters" },
    { UNITS_US_GALLONS, "us-gallons" },
    { UNITS_CUBIC_FEET_PER_MINUTE, "cubic-feet-per-minute" },
    { UNITS_CUBIC_METERS_PER_SECOND, "cubic-meters-per-second" },
    { UNITS_IMPERIAL_GALLONS_PER_MINUTE, "imperial-gallons-per-minute" },
    { UNITS_LITERS_PER_SECOND, "liters-per-second" },
    { UNITS_LITERS_PER_MINUTE, "liters-per-minute" },
    { UNITS_US_GALLONS_PER_MINUTE, "us-gallons-per-minute" },
    { UNITS_DEGREES_ANGULAR, "degrees-angular" },
    { UNITS_DEGREES_CELSIUS_PER_HOUR, "degrees-celsius-per-hour" },
    { UNITS_DEGREES_CELSIUS_PER_MINUTE, "degrees-celsius-per-minute" },
    { UNITS_DEGREES_FAHRENHEIT_PER_HOUR, "degrees-fahrenheit-per-hour" },
    { UNITS_DEGREES_FAHRENHEIT_PER_MINUTE, "degrees-fahrenheit-per-minute" },
    { UNITS_NO_UNITS, "no-units" },
    { UNITS_PARTS_PER_MILLION, "parts-per-million" },
    { UNITS_PARTS_PER_BILLION, "parts-per-billion" },
    { UNITS_PERCENT, "percent" },
    { UNITS_PERCENT_PER_SECOND, "percent-per-second" },
    { UNITS_PER_MINUTE, "per-minute" },
    { UNITS_PER_SECOND, "per-second" },
    { UNITS_PSI_PER_DEGREE_FAHRENHEIT, "psi-per-degree-fahrenheit" },
    { UNITS_RADIANS, "radians" },
    { UNITS_REVOLUTIONS_PER_MINUTE, "revolutions-per-minute" },
    { UNITS_CURRENCY1, "currency1" },
    { UNITS_CURRENCY2, "currency2" },
    { UNITS_CURRENCY3, "currency3" },
    { UNITS_CURRENCY4, "currency4" },
    { UNITS_CURRENCY5, "currency5" },
    { UNITS_CURRENCY6, "currency6" },
    { UNITS_CURRENCY7, "currency7" },
    { UNITS_CURRENCY8, "currency8" },
    { UNITS_CURRENCY9, "currency9" },
    { UNITS_CURRENCY10, "currency10" },
    { UNITS_SQUARE_INCHES, "square-inches" },
    { UNITS_SQUARE_CENTIMETERS, "square-centimeters" },
    { UNITS_BTUS_PER_POUND, "btus-per-pound" },
    { UNITS_CENTIMETERS, "centimeters" },
    { UNITS_POUNDS_MASS_PER_SECOND, "pounds-mass-per-second" },
    { UNITS_DELTA_DEGREES_FAHRENHEIT, "delta-degrees-fahrenheit" },
    { UNITS_DELTA_KELVIN, "delta-kelvin" },
    { UNITS_KILOHMS, "kilohms" },
    { UNITS_MEGOHMS, "megohms" },
    { UNITS_MILLIVOLTS, "millivolts" },
    { UNITS_KILOJOULES_PER_KILOGRAM, "kilojoules-per-kilogram" },
    { UNITS_MEGAJOULES, "megajoules" },
    { UNITS_JOULES_PER_DEGREE_KELVIN, "joules-per-degree-kelvin" },
    { UNITS_JOULES_PER_KILOGRAM_DEGREE_KELVIN,
      "joules-per-kilogram-degree-kelvin" },
    { UNITS_KILOHERTZ, "kilohertz" },
    { UNITS_MEGAHERTZ, "megahertz" },
    { UNITS_PER_HOUR, "per-hour" },
    { UNITS_MILLIWATTS, "milliwatts" },
    { UNITS_HECTOPASCALS, "hectopascals" },
    { UNITS_MILLIBARS, "millibars" },
    { UNITS_CUBIC_METERS_PER_HOUR, "cubic-meters-per-hour" },
    { UNITS_LITERS_PER_HOUR, "liters-per-hour" },
    { UNITS_KW_HOURS_PER_SQUARE_METER, "kilowatt-hours-per-square-meter" },
    { UNITS_KW_HOURS_PER_SQUARE_FOOT, "kilowatt-hours-per-square-foot" },
    { UNITS_MEGAJOULES_PER_SQUARE_METER, "megajoules-per-square-meter" },
    { UNITS_MEGAJOULES_PER_SQUARE_FOOT, "megajoules-per-square-foot" },
    { UNITS_CUBIC_FEET_PER_SECOND, "cubic-feet-per-second" },
    { UNITS_WATTS_PER_SQUARE_METER_DEGREE_KELVIN,
      "watts-per-square-meter-degree-kelvin" },
    { UNITS_PERCENT_OBSCURATION_PER_FOOT, "percent-obscuration-per-foot" },
    { UNITS_PERCENT_OBSCURATION_PER_METER, "percent-obscuration-per-meter" },
    { UNITS_MILLIOHMS, "milliohms" },
    { UNITS_MEGAWATT_HOURS, "megawatt-hours" },
    { UNITS_KILO_BTUS, "kilo-btus" },
    { UNITS_MEGA_BTUS, "mega-btus" },
    { UNITS_KILOJOULES_PER_KILOGRAM_DRY_AIR,
      "kilojoules-per-kilogram-dry-air" },
    { UNITS_MEGAJOULES_PER_KILOGRAM_DRY_AIR,
      "megajoules-per-kilogram-dry-air" },
    { UNITS_KILOJOULES_PER_DEGREE_KELVIN, "kilojoules-per-degree-kelvin" },
    { UNITS_MEGAJOULES_PER_DEGREE_KELVIN, "megajoules-per-degree-kelvin" },
    { UNITS_NEWTON, "newton" },
    { UNITS_GRAMS_PER_SECOND, "grams-per-second" },
    { UNITS_GRAMS_PER_MINUTE, "grams-per-minute" },
    { UNITS_TONS_PER_HOUR, "tons-per-hour" },
    { UNITS_KILO_BTUS_PER_HOUR, "kilo-btus-per-hour" },
    { UNITS_HUNDREDTHS_SECONDS, "hundredths-seconds" },
    { UNITS_MILLISECONDS, "milliseconds" },
    { UNITS_NEWTON_METERS, "newton-meters" },
    { UNITS_MILLIMETERS_PER_SECOND, "millimeters-per-second" },
    { UNITS_MILLIMETERS_PER_MINUTE, "millimeters-per-minute" },
    { UNITS_METERS_PER_MINUTE, "meters-per-minute" },
    { UNITS_METERS_PER_HOUR, "meters-per-hour" },
    { UNITS_CUBIC_METERS_PER_MINUTE, "cubic-meters-per-minute" },
    { UNITS_METERS_PER_SECOND_PER_SECOND, "meters-per-second-per-second" },
    { UNITS_AMPERES_PER_METER, "amperes-per-meter" },
    { UNITS_AMPERES_PER_SQUARE_METER, "amperes-per-square-meter" },
    { UNITS_AMPERE_SQUARE_METERS, "ampere-square-meters" },
    { UNITS_FARADS, "farads" },
    { UNITS_HENRYS, "henrys" },
    { UNITS_OHM_METERS, "ohm-meters" },
    { UNITS_SIEMENS, "siemens" },
    { UNITS_SIEMENS_PER_METER, "siemens-per-meter" },
    { UNITS_TESLAS, "teslas" },
    { UNITS_VOLTS_PER_DEGREE_KELVIN, "volts-per-degree-kelvin" },
    { UNITS_VOLTS_PER_METER, "volts-per-meter" },
    { UNITS_WEBERS, "webers" },
    { UNITS_CANDELAS, "candelas" },
    { UNITS_CANDELAS_PER_SQUARE_METER, "candelas-per-square-meter" },
    { UNITS_KELVIN_PER_HOUR, "kelvin-per-hour" },
    { UNITS_KELVIN_PER_MINUTE, "kelvin-per-minute" },
    { UNITS_JOULE_SECONDS, "joule-seconds" },
    { UNITS_RADIANS_PER_SECOND, "radians-per-second" },
    { UNITS_SQUARE_METERS_PER_NEWTON, "square-meters-per-newton" },
    { UNITS_KILOGRAMS_PER_CUBIC_METER, "kilograms-per-cubic-meter" },
    { UNITS_NEWTON_SECONDS, "newton-seconds" },
    { UNITS_NEWTONS_PER_METER, "newtons-per-meter" },
    { UNITS_WATTS_PER_METER_PER_DEGREE_KELVIN,
      "watts-per-meter-per-degree-kelvin" },
    { UNITS_MICROSIEMENS, "micro-siemens" },
    { UNITS_CUBIC_FEET_PER_HOUR, "cubic-feet-per-hour" },
    { UNITS_US_GALLONS_PER_HOUR, "us-gallons-per-hour" },
    { UNITS_KILOMETERS, "kilometers" },
    { UNITS_MICROMETERS, "micrometers" },
    { UNITS_GRAMS, "grams" },
    { UNITS_MILLIGRAMS, "milligrams" },
    { UNITS_MILLILITERS, "milliliters" },
    { UNITS_MILLILITERS_PER_SECOND, "milliliters-per-second" },
    { UNITS_DECIBELS, "decibels" },
    { UNITS_DECIBELS_MILLIVOLT, "decibels-millivolt" },
    { UNITS_DECIBELS_VOLT, "decibels-volt" },
    { UNITS_MILLISIEMENS, "millisiemens" },
    { UNITS_WATT_REACTIVE_HOURS, "watt-reactive-hours" },
    { UNITS_KILOWATT_REACTIVE_HOURS, "kilowatt-reactive-hours" },
    { UNITS_MEGAWATT_REACTIVE_HOURS, "megawatt-reactive-hours" },
    { UNITS_MILLIMETERS_OF_WATER, "millimeters-of-water" },
    { UNITS_PER_MILLE, "per-mille" },
    { UNITS_GRAMS_PER_GRAM, "grams-per-gram" },
    { UNITS_KILOGRAMS_PER_KILOGRAM, "kilograms-per-kilogram" },
    { UNITS_GRAMS_PER_KILOGRAM, "grams-per-kilogram" },
    { UNITS_MILLIGRAMS_PER_GRAM, "milligrams-per-gram" },
    { UNITS_MILLIGRAMS_PER_KILOGRAM, "milligrams-per-kilogram" },
    { UNITS_GRAMS_PER_MILLILITER, "grams-per-milliliter" },
    { UNITS_GRAMS_PER_LITER, "grams-per-liter" },
    { UNITS_MILLIGRAMS_PER_LITER, "milligrams-per-liter" },
    { UNITS_MICROGRAMS_PER_LITER, "micrograms-per-liter" },
    { UNITS_GRAMS_PER_CUBIC_METER, "grams-per-cubic-meter" },
    { UNITS_MILLIGRAMS_PER_CUBIC_METER, "milligrams-per-cubic-meter" },
    { UNITS_MICROGRAMS_PER_CUBIC_METER, "micrograms-per-cubic-meter" },
    { UNITS_NANOGRAMS_PER_CUBIC_METER, "nanograms-per-cubic-meter" },
    { UNITS_GRAMS_PER_CUBIC_CENTIMETER, "grams-per-cubic-centimeter" },
    { UNITS_BECQUERELS, "becquerels" },
    { UNITS_KILOBECQUERELS, "kilobecquerels" },
    { UNITS_MEGABECQUERELS, "megabecquerels" },
    { UNITS_GRAY, "gray" },
    { UNITS_MILLIGRAY, "milligray" },
    { UNITS_MICROGRAY, "microgray" },
    { UNITS_SIEVERTS, "sieverts" },
    { UNITS_MILLISIEVERTS, "millisieverts" },
    { UNITS_MICROSIEVERTS, "microsieverts" },
    { UNITS_MICROSIEVERTS_PER_HOUR, "microsieverts-per-hour" },
    { UNITS_DECIBELS_A, "decibels-a" },
    { UNITS_NEPHELOMETRIC_TURBIDITY_UNIT, "nephelometric-turbidity-unit" },
    { UNITS_PH, "pH" },
    { UNITS_GRAMS_PER_SQUARE_METER, "grams-per-square-meter" },
    { UNITS_MINUTES_PER_DEGREE_KELVIN, "minutes-per-degree-kelvin" },
    { UNITS_OHM_METER_SQUARED_PER_METER, "ohm-meter-squared-per-meter" },
    { UNITS_AMPERE_SECONDS, "ampere-seconds" },
    { UNITS_VOLT_AMPERE_HOURS, "volt-ampere-hours" },
    { UNITS_KILOVOLT_AMPERE_HOURS, "kilovolt-ampere-hours" },
    { UNITS_MEGAVOLT_AMPERE_HOURS, "megavolt-ampere-hours" },
    { UNITS_VOLT_AMPERE_REACTIVE_HOURS, "volt-ampere-reactive-hours" },
    { UNITS_KILOVOLT_AMPERE_REACTIVE_HOURS, "kilovolt-ampere-reactive-hours" },
    { UNITS_MEGAVOLT_AMPERE_REACTIVE_HOURS, "megavolt-ampere-reactive-hours" },
    { UNITS_VOLT_SQUARE_HOURS, "volt-square-hours" },
    { UNITS_AMPERE_SQUARE_HOURS, "ampere-square-hours" },
    { UNITS_JOULE_PER_HOURS, "joule-per-hours" },
    { UNITS_CUBIC_FEET_PER_DAY, "cubic-feet-per-day" },
    { UNITS_CUBIC_METERS_PER_DAY, "cubic-meters-per-day" },
    { UNITS_WATT_HOURS_PER_CUBIC_METER, "watt-hours-per-cubic-meter" },
    { UNITS_JOULES_PER_CUBIC_METER, "joules-per-cubic-meter" },
    { UNITS_MOLE_PERCENT, "mole-percent" },
    { UNITS_PASCAL_SECONDS, "pascal-seconds" },
    { UNITS_MILLION_STANDARD_CUBIC_FEET_PER_MINUTE,
      "million-standard-cubic-feet-per-minute" },
    { UNITS_STANDARD_CUBIC_FEET_PER_DAY, "standard-cubic-feet-per-day" },
    { UNITS_MILLION_STANDARD_CUBIC_FEET_PER_DAY,
      "million-standard-cubic-feet-per-day" },
    { UNITS_THOUSAND_CUBIC_FEET_PER_DAY, "thousand-cubic-feet-per-day" },
    { UNITS_THOUSAND_STANDARD_CUBIC_FEET_PER_DAY,
      "thousand-standard-cubic-feet-per-day" },
    { UNITS_POUNDS_MASS_PER_DAY, "pounds-mass-per-day" },
    { UNITS_MILLIREMS, "millirems" },
    { UNITS_MILLIREMS_PER_HOUR, "millirems-per-hour" },
    { UNITS_DEGREES_LOVIBOND, "degrees-lovibond" },
    { UNITS_ALCOHOL_BY_VOLUME, "alcohol-by-volume" },
    { UNITS_INTERNATIONAL_BITTERING_UNITS, "international-bittering-units" },
    { UNITS_EUROPEAN_BITTERNESS_UNITS, "european-bitterness-units" },
    { UNITS_DEGREES_PLATO, "degrees-plato" },
    { UNITS_SPECIFIC_GRAVITY, "specific-gravity" },
    { UNITS_EUROPEAN_BREWING_CONVENTION, "european-brewing-convention" },
    { UNITS_PER_DAY, "per-day" },
    { UNITS_PER_MILLISECOND, "per-millisecond" },
    { UNITS_YARDS, "yards" },
    { UNITS_MILES, "miles" },
    { UNITS_NAUTICAL_MILES, "nautical-miles" },
    { UNITS_NANOGRAMS, "nanograms" },
    { UNITS_MICROGRAMS, "micrograms" },
    { UNITS_METRIC_TONNES, "metric-tonnes" },
    { UNITS_SHORT_TONS, "short-tons" },
    { UNITS_LONG_TONS, "long-tons" },
    { UNITS_GRAMS_PER_HOUR, "grams-per-hour" },
    { UNITS_GRAMS_PER_DAY, "grams-per-day" },
    { UNITS_KILOGRAMS_PER_DAY, "kilograms-per-day" },
    { UNITS_SHORT_TONS_PER_SECOND, "short-tons-per-second" },
    { UNITS_SHORT_TONS_PER_MINUTE, "short-tons-per-minute" },
    { UNITS_SHORT_TONS_PER_HOUR, "short-tons-per-hour" },
    { UNITS_SHORT_TONS_PER_DAY, "short-tons-per-day" },
    { UNITS_METRIC_TONNES_PER_SECOND, "metric-tonnes-per-second" },
    { UNITS_METRIC_TONNES_PER_MINUTE, "metric-tonnes-per-minute" },
    { UNITS_METRIC_TONNES_PER_HOUR, "metric-tonnes-per-hour" },
    { UNITS_METRIC_TONNES_PER_DAY, "metric-tonnes-per-day" },
    { UNITS_LONG_TONS_PER_SECOND, "long-tons-per-second" },
    { UNITS_LONG_TONS_PER_MINUTE, "long-tons-per-minute" },
    { UNITS_LONG_TONS_PER_HOUR, "long-tons-per-hour" },
    { UNITS_LONG_TONS_PER_DAY, "long-tons-per-day" },
    { UNITS_BTUS_PER_SECOND, "btus-per-second" },
    { UNITS_BTUS_PER_MINUTE, "btus-per-minute" },
    { UNITS_BTUS_PER_DAY, "btus-per-day" },
    { UNITS_KILO_BTUS_PER_SECOND, "kilo-btus-per-second" },
    { UNITS_KILO_BTUS_PER_MINUTE, "kilo-btus-per-minute" },
    { UNITS_KILO_BTUS_PER_DAY, "kilo-btus-per-day" },
    { UNITS_MEGA_BTUS_PER_SECOND, "mega-btus-per-second" },
    { UNITS_MEGA_BTUS_PER_MINUTE, "mega-btus-per-minute" },
    { UNITS_MEGA_BTUS_PER_HOUR, "mega-btus-per-hour" },
    { UNITS_MEGA_BTUS_PER_DAY, "mega-btus-per-day" },
    { UNITS_JOULES_PER_SECOND, "joules-per-second" },
    { UNITS_JOULES_PER_MINUTE, "joules-per-minute" },
    { UNITS_JOULES_PER_DAY, "joules-per-day" },
    { UNITS_KILOJOULES_PER_SECOND, "kilojoules-per-second" },
    { UNITS_KILOJOULES_PER_MINUTE, "kilojoules-per-minute" },
    { UNITS_KILOJOULES_PER_HOUR, "kilojoules-per-hour" },
    { UNITS_KILOJOULES_PER_DAY, "kilojoules-per-day" },
    { UNITS_MEGAJOULES_PER_SECOND, "megajoules-per-second" },
    { UNITS_MEGAJOULES_PER_MINUTE, "megajoules-per-minute" },
    { UNITS_MEGAJOULES_PER_HOUR, "megajoules-per-hour" },
    { UNITS_MEGAJOULES_PER_DAY, "megajoules-per-day" },
    { UNITS_DEGREES_CELSIUS_PER_DAY, "degrees-celsius-per-day" },
    { UNITS_KELVIN_PER_DAY, "kelvin-per-day" },
    { UNITS_DEGREES_FAHRENHEIT_PER_DAY, "degrees-fahrenheit-per-day" },
    { UNITS_DELTA_DEGREES_CELSIUS, "delta-degrees-celsius" },
    { UNITS_MILLION_CUBIC_FEET_PER_MINUTE, "million-cubic-feet-per-minute" },
    { UNITS_MILLION_CUBIC_FEET_PER_DAY, "million-cubic-feet-per-day" },
    { UNITS_IMPERIAL_GALLONS_PER_SECOND, "imperial-gallons-per-second" },
    { UNITS_IMPERIAL_GALLONS_PER_HOUR, "imperial-gallons-per-hour" },
    { UNITS_IMPERIAL_GALLONS_PER_DAY, "imperial-gallons-per-day" },
    { UNITS_LITERS_PER_DAY, "liters-per-day" },
    { UNITS_US_GALLONS_PER_SECOND, "us-gallons-per-second" },
    { UNITS_US_GALLONS_PER_DAY, "us-gallons-per-day" },
    { UNITS_PERCENT_PER_MINUTE, "percent-per-minute" },
    { UNITS_PERCENT_PER_HOUR, "percent-per-hour" },
    { UNITS_PERCENT_PER_DAY, "percent-per-day" },
    { UNITS_PER_MILLION, "per-million" },
    { UNITS_PER_BILLION, "per-billion" },
    { UNITS_MICROGRAMS_PER_GRAM, "micrograms-per-gram" },
    { UNITS_NANOGRAMS_PER_GRAM, "nanograms-per-gram" },
    { UNITS_MICROGRAMS_PER_KILOGRAM, "micrograms-per-kilogram" },
    { UNITS_NANOGRAMS_PER_KILOGRAM, "nanograms-per-kilogram" },
    { UNITS_MILLIGRAMS_PER_MILLILITER, "milligrams-per-milliliter" },
    { UNITS_MICROGRAMS_PER_MILLILITER, "micrograms-per-milliliter" },
    { UNITS_NANOGRAMS_PER_MILLILITER, "nanograms-per-milliliter" },
    { UNITS_KILOGRAMS_PER_LITER, "kilograms-per-liter" },
    { UNITS_NANOGRAMS_PER_LITER, "nanograms-per-liter" },
    { UNITS_MILLIGRAMS_PER_CUBIC_CENTIMETER,
      "milligrams-per-cubic-centimeter" },
    { UNITS_MICROGRAMS_PER_CUBIC_CENTIMETER,
      "micrograms-per-cubic-centimeter" },
    { UNITS_NANOGRAMS_PER_CUBIC_CENTIMETER, "nanograms-per-cubic-centimeter" },
    { UNITS_BTU_PER_HOUR_PER_WATT, "btu-per-hour-per-watt" },
    { UNITS_BTU_PER_WATT_HOUR_SEASONAL, "btu-per-watt-hour-seasonal" },
    { UNITS_COEFFICIENT_OF_PERFORMANCE, "coefficient-of-performance" },
    { UNITS_COEFFICIENT_OF_PERFORMANCE_SEASONAL,
      "coefficient-of-performance-seasonal" },
    { UNITS_KILOWATT_PER_TON_REFRIGERATION, "kilowatt-per-ton-refrigeration" },
    { UNITS_LUMENS_PER_WATT, "lumens-per-watt" },
    { UNITS_POUND_FORCE_FEET, "pound-force-feet" },
    { UNITS_POUND_FORCE_INCHES, "pound-force-inches" },
    { UNITS_OUNCE_FORCE_INCHES, "ounce-force-inches" },
    { UNITS_POUNDS_FORCE_PER_SQUARE_INCH_ABSOLUTE,
      "pounds-force-per-square-inch-absolute" },
    { UNITS_POUNDS_FORCE_PER_SQUARE_INCH_GAUGE,
      "pounds-force-per-square-inch-gauge" },
    { UNITS_MICROSIEMENS_PER_CENTIMETER, "microsiemens-per-centimeter" },
    { UNITS_ACTIVE_ENERGY_PULSE_VALUE, "active-energy-pulse-value" },
    { UNITS_MILLISIEMENS_PER_CENTIMETER, "millisiemens-per-centimeter" },
    { UNITS_MILLISIEMENS_PER_METER, "millisiemens-per-meter" },
    { UNITS_MILLIONS_OF_US_GALLONS, "millions-of-us-gallons" },
    { UNITS_MILLIONS_OF_IMPERIAL_GALLONS, "millions-of-imperial-gallons" },
    { UNITS_MILLILITERS_PER_MINUTE, "milliliters-per-minute" },
    { UNITS_MILS_PER_YEAR, "mils-per-year" },
    { UNITS_MILLIMETERS_PER_YEAR, "millimeters-per-year" },
    { UNITS_PULSES_PER_MINUTE, "pulses-per-minute" },
    { UNITS_REACTIVE_ENERGY_PULSE_VALUE, "reactive-energy-pulse-value" },
    { UNITS_APPARENT_ENERGY_PULSE_VALUE, "apparent-energy-pulse-value" },
    { UNITS_VOLT_SQUARED_HOUR_PULSE_VALUE, "volt-squared-hour-pulse-value" },
    { UNITS_AMPERE_SQUARED_HOUR_PULSE_VALUE,
      "ampere-squared-hour-pulse-value" },
    { UNITS_CUBIC_METER_PULSE_VALUE, "cubic-meter-pulse-value" },
    { UNITS_GIGAWATTS, "gigawatts" },
    { UNITS_GIGAJOULES, "gigajoules" },
    { UNITS_TERAJOULES, "terajoules" },
    { UNITS_GIGAWATT_HOURS, "gigawatt-hours" },
    { UNITS_GIGAWATT_REACTIVE_HOURS, "gigawatt-reactive-hours" },
    { UNITS_BITS_PER_SECOND, "bits-per-second" },
    { UNITS_KILOBITS_PER_SECOND, "kilobits-per-second" },
    { UNITS_MEGABITS_PER_SECOND, "megabits-per-second" },
    { UNITS_GIGABITS_PER_SECOND, "gigabits-per-second" },
    { UNITS_BYTES_PER_SECOND, "bytes-per-second" },
    { UNITS_KILOBYTES_PER_SECOND, "kilobytes-per-second" },
    { UNITS_MEGABYTES_PER_SECOND, "megabytes-per-second" },
    { UNITS_GIGABYTES_PER_SECOND, "gigabytes-per-second" },
    { UNITS_VOLUME1, "volume1" },
    { UNITS_VOLUME2, "volume2" },
    { UNITS_VOLUME3, "volume3" },
    { UNITS_VOLUME4, "volume4" },
    { UNITS_VOLUME5, "volume5" },
    { UNITS_VOLUME6, "volume6" },
    { UNITS_VOLUME7, "volume7" },
    { UNITS_VOLUME8, "volume8" },
    { UNITS_VOLUME9, "volume9" },
    { UNITS_VOLUME10, "volume10" },
    { UNITS_VOLUMETRIC_FLOW1, "volumetric-flow1" },
    { UNITS_VOLUMETRIC_FLOW2, "volumetric-flow2" },
    { UNITS_VOLUMETRIC_FLOW3, "volumetric-flow3" },
    { UNITS_VOLUMETRIC_FLOW4, "volumetric-flow4" },
    { UNITS_VOLUMETRIC_FLOW5, "volumetric-flow5" },
    { UNITS_VOLUMETRIC_FLOW6, "volumetric-flow6" },
    { UNITS_VOLUMETRIC_FLOW7, "volumetric-flow7" },
    { UNITS_VOLUMETRIC_FLOW8, "volumetric-flow8" },
    { UNITS_VOLUMETRIC_FLOW9, "volumetric-flow9" },
    { UNITS_VOLUMETRIC_FLOW10, "volumetric-flow10" },
    { UNITS_SITE_UNIT1, "site-unit1" },
    { UNITS_SITE_UNIT2, "site-unit2" },
    { UNITS_SITE_UNIT3, "site-unit3" },
    { UNITS_SITE_UNIT4, "site-unit4" },
    { UNITS_SITE_UNIT5, "site-unit5" },
    { UNITS_SITE_UNIT6, "site-unit6" },
    { UNITS_SITE_UNIT7, "site-unit7" },
    { UNITS_SITE_UNIT8, "site-unit8" },
    { UNITS_SITE_UNIT9, "site-unit9" },
    { UNITS_SITE_UNIT10, "site-unit10" },
    { UNITS_GRAINS_OF_WATER_PER_POUND_DRY_AIR,
      "grains-of-water-per-pound-dry-air" },
    { UNITS_DEGREE_HOURS_CELSIUS, "degree-hours-celsius" },
    { UNITS_DEGREE_HOURS_FAHRENHEIT, "degree-hours-fahrenheit" },
    { UNITS_DEGREE_MINUTES_CELSIUS, "degree-minutes-celsius" },
    { UNITS_DEGREE_MINUTES_FAHRENHEIT, "degree-minutes-fahrenheit" },
    { UNITS_DEGREE_SECONDS_CELSIUS, "degree-seconds-celsius" },
    { UNITS_DEGREE_SECONDS_FAHRENHEIT, "degree-seconds-fahrenheit" },
    { UNITS_MICROSECONDS, "microseconds" },
    { UNITS_NANOSECONDS, "nanoseconds" },
    { UNITS_PICOSECONDS, "picoseconds" },
    { UNITS_PARTICLES_PER_CUBIC_FOOT, "particles-per-cubic-foot" },
    { UNITS_PARTICLES_PER_CUBIC_METER, "particles-per-cubic-meter" },
    { UNITS_PICOCURIES_PER_LITER, "picocuries-per-liter" },
    { UNITS_BECQUERELS_PER_CUBIC_METER, "becquerels-per-cubic-meter" },
    { 0, NULL }
    /* Enumerated values 0-255 and 47808-49999 are reserved for definition by
       ASHRAE. Enumerated values 256-47807 and 50000-65535 may be used by others
       subject to the procedures and constraints described in Clause 23. */
};

bool bactext_engineering_unit_name_proprietary(unsigned index)
{
    bool status = false;

    if ((index >= UNITS_PROPRIETARY_RANGE_MIN) &&
        (index <= UNITS_PROPRIETARY_RANGE_MAX)) {
        status = true;
    } else if (
        (index >= UNITS_PROPRIETARY_RANGE_MIN2) &&
        (index <= UNITS_PROPRIETARY_RANGE_MAX2)) {
        status = true;
    }

    return status;
}

const char *bactext_engineering_unit_name(unsigned index)
{
    if (bactext_engineering_unit_name_proprietary(index)) {
        return Vendor_Proprietary_String;
    } else if (index <= UNITS_RESERVED_RANGE_MAX2) {
        return indtext_by_index_default(
            bacnet_engineering_unit_names, index, ASHRAE_Reserved_String);
    }

    return ASHRAE_Reserved_String;
}

bool bactext_engineering_unit_index(
    const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_engineering_unit_names, search_name, found_index);
}

INDTEXT_DATA bacnet_reject_reason_names[] = {
    { REJECT_REASON_OTHER, "Other" },
    { REJECT_REASON_BUFFER_OVERFLOW, "Buffer Overflow" },
    { REJECT_REASON_INCONSISTENT_PARAMETERS, "Inconsistent Parameters" },
    { REJECT_REASON_INVALID_PARAMETER_DATA_TYPE,
      "Invalid Parameter Data Type" },
    { REJECT_REASON_INVALID_TAG, "Invalid Tag" },
    { REJECT_REASON_MISSING_REQUIRED_PARAMETER, "Missing Required Parameter" },
    { REJECT_REASON_PARAMETER_OUT_OF_RANGE, "Parameter Out of Range" },
    { REJECT_REASON_TOO_MANY_ARGUMENTS, "Too Many Arguments" },
    { REJECT_REASON_UNDEFINED_ENUMERATION, "Undefined Enumeration" },
    { REJECT_REASON_UNRECOGNIZED_SERVICE, "Unrecognized Service" },
    { REJECT_REASON_INVALID_DATA_ENCODING, "invalid-data-encoding" },
    { REJECT_REASON_PROPRIETARY_FIRST, "Proprietary" },
    { 0, NULL }
};

const char *bactext_reject_reason_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_reject_reason_names, index, REJECT_REASON_PROPRIETARY_FIRST,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

INDTEXT_DATA bacnet_abort_reason_names[] = {
    { ABORT_REASON_OTHER, "Other" },
    { ABORT_REASON_BUFFER_OVERFLOW, "Buffer Overflow" },
    { ABORT_REASON_INVALID_APDU_IN_THIS_STATE, "Invalid APDU in this State" },
    { ABORT_REASON_PREEMPTED_BY_HIGHER_PRIORITY_TASK,
      "Preempted by Higher Priority Task" },
    { ABORT_REASON_SEGMENTATION_NOT_SUPPORTED, "Segmentation Not Supported" },
    { ABORT_REASON_SECURITY_ERROR, "Security Error" },
    { ABORT_REASON_INSUFFICIENT_SECURITY, "Insufficient Security" },
    { ABORT_REASON_WINDOW_SIZE_OUT_OF_RANGE, "window-size-out-of-range" },
    { ABORT_REASON_APPLICATION_EXCEEDED_REPLY_TIME,
      "application-exceeded-reply-time" },
    { ABORT_REASON_OUT_OF_RESOURCES, "out-of-resources" },
    { ABORT_REASON_TSM_TIMEOUT, "tsm-timeout" },
    { ABORT_REASON_APDU_TOO_LONG, "apdu-too-long" },
    { ABORT_REASON_PROPRIETARY_FIRST, "Proprietary" },
    { 0, NULL }
};

const char *bactext_abort_reason_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_abort_reason_names, index, ABORT_REASON_PROPRIETARY_FIRST,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

INDTEXT_DATA bacnet_error_class_names[] = {
    { ERROR_CLASS_DEVICE, "device" },
    { ERROR_CLASS_OBJECT, "object" },
    { ERROR_CLASS_PROPERTY, "property" },
    { ERROR_CLASS_RESOURCES, "resources" },
    { ERROR_CLASS_SECURITY, "security" },
    { ERROR_CLASS_SERVICES, "services" },
    { ERROR_CLASS_VT, "vt" },
    { ERROR_CLASS_COMMUNICATION, "communication" },
    { 0, NULL }
};

const char *bactext_error_class_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_error_class_names, index, ERROR_CLASS_PROPRIETARY_FIRST,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

INDTEXT_DATA bacnet_error_code_names[] = {
    { ERROR_CODE_OTHER, "other" },
    { ERROR_CODE_AUTHENTICATION_FAILED, "authentication-failed" },
    { ERROR_CODE_CHARACTER_SET_NOT_SUPPORTED, "character-set-not-supported" },
    { ERROR_CODE_CONFIGURATION_IN_PROGRESS, "configuration-in-progress" },
    { ERROR_CODE_DATATYPE_NOT_SUPPORTED, "datatype-not-supported" },
    { ERROR_CODE_DEVICE_BUSY, "device-busy" },
    { ERROR_CODE_DUPLICATE_NAME, "duplicate-name" },
    { ERROR_CODE_DUPLICATE_OBJECT_ID, "duplicate-object-id" },
    { ERROR_CODE_DYNAMIC_CREATION_NOT_SUPPORTED,
      "dynamic-creation-not-supported" },
    { ERROR_CODE_FILE_ACCESS_DENIED, "file-access-denied" },
    { ERROR_CODE_INCOMPATIBLE_SECURITY_LEVELS, "incompatible-security-levels" },
    { ERROR_CODE_INCONSISTENT_PARAMETERS, "inconsistent-parameters" },
    { ERROR_CODE_INCONSISTENT_SELECTION_CRITERION,
      "inconsistent-selection-criterion" },
    { ERROR_CODE_INVALID_ARRAY_INDEX, "invalid-array-index" },
    { ERROR_CODE_INVALID_CONFIGURATION_DATA, "invalid-configuration-data" },
    { ERROR_CODE_INVALID_DATA_TYPE, "invalid-data-type" },
    { ERROR_CODE_INVALID_FILE_ACCESS_METHOD, "invalid-file-access-method" },
    { ERROR_CODE_INVALID_FILE_START_POSITION,
      "error-code-invalid-file-start-position" },
    { ERROR_CODE_INVALID_OPERATOR_NAME, "invalid-operator-name" },
    { ERROR_CODE_INVALID_PARAMETER_DATA_TYPE, "invalid-parameter-data-type" },
    { ERROR_CODE_INVALID_TIME_STAMP, "invalid-time-stamp" },
    { ERROR_CODE_KEY_GENERATION_ERROR, "key-generation-error" },
    { ERROR_CODE_MISSING_REQUIRED_PARAMETER, "missing-required-parameter" },
    { ERROR_CODE_NO_OBJECTS_OF_SPECIFIED_TYPE, "no-objects-of-specified-type" },
    { ERROR_CODE_NO_SPACE_FOR_OBJECT, "no-space-for-object" },
    { ERROR_CODE_NO_SPACE_TO_ADD_LIST_ELEMENT, "no-space-to-add-list-element" },
    { ERROR_CODE_NO_SPACE_TO_WRITE_PROPERTY, "no-space-to-write-property" },
    { ERROR_CODE_NO_VT_SESSIONS_AVAILABLE, "no-vt-sessions-available" },
    { ERROR_CODE_OBJECT_DELETION_NOT_PERMITTED,
      "object-deletion-not-permitted" },
    { ERROR_CODE_OBJECT_IDENTIFIER_ALREADY_EXISTS,
      "object-identifier-already-exists" },
    { ERROR_CODE_OPERATIONAL_PROBLEM, "operational-problem" },
    { ERROR_CODE_OPTIONAL_FUNCTIONALITY_NOT_SUPPORTED,
      "optional-functionality-not-supported" },
    { ERROR_CODE_PASSWORD_FAILURE, "password-failure" },
    { ERROR_CODE_PROPERTY_IS_NOT_A_LIST, "property-is-not-a-list" },
    { ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY, "property-is-not-an-array" },
    { ERROR_CODE_READ_ACCESS_DENIED, "read-access-denied" },
    { ERROR_CODE_SECURITY_NOT_SUPPORTED, "security-not-supported" },
    { ERROR_CODE_SERVICE_REQUEST_DENIED, "service-request-denied" },
    { ERROR_CODE_TIMEOUT, "timeout" },
    { ERROR_CODE_UNKNOWN_OBJECT, "unknown-object" },
    { ERROR_CODE_UNKNOWN_PROPERTY, "unknown-property" },
    { ERROR_CODE_RESERVED1, "reserved1" },
    { ERROR_CODE_UNKNOWN_VT_CLASS, "unknown-vt-class" },
    { ERROR_CODE_UNKNOWN_VT_SESSION, "unknown-vt-session" },
    { ERROR_CODE_UNSUPPORTED_OBJECT_TYPE, "unsupported-object-type" },
    { ERROR_CODE_VALUE_OUT_OF_RANGE, "value-out-of-range" },
    { ERROR_CODE_VT_SESSION_ALREADY_CLOSED, "vt-session-already-closed" },
    { ERROR_CODE_VT_SESSION_TERMINATION_FAILURE,
      "vt-session-termination-failure" },
    { ERROR_CODE_WRITE_ACCESS_DENIED, "write-access-denied" },
    { ERROR_CODE_COV_SUBSCRIPTION_FAILED, "cov-subscription-failed" },
    { ERROR_CODE_NOT_COV_PROPERTY, "not-cov-property" },
    { ERROR_CODE_ABORT_BUFFER_OVERFLOW, "abort-buffer-overflow" },
    { ERROR_CODE_ABORT_INVALID_APDU_IN_THIS_STATE,
      "abort-invalid-apdu-in-this-state" },
    { ERROR_CODE_ABORT_PREEMPTED_BY_HIGHER_PRIORITY_TASK,
      "abort-preempted-by-higher-priority-task" },
    { ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED,
      "abort-segmentation-not-supported" },
    { ERROR_CODE_ABORT_PROPRIETARY, "abort-proprietary" },
    { ERROR_CODE_ABORT_OTHER, "abort-other" },
    { ERROR_CODE_INVALID_TAG, "invalid-tag" },
    { ERROR_CODE_NETWORK_DOWN, "network-down" },
    { ERROR_CODE_REJECT_BUFFER_OVERFLOW, "reject-buffer-overflow" },
    { ERROR_CODE_REJECT_INCONSISTENT_PARAMETERS,
      "reject-inconsistent-parameters" },
    { ERROR_CODE_REJECT_INVALID_PARAMETER_DATA_TYPE,
      "reject-invalid-parameter-data-type" },
    { ERROR_CODE_REJECT_INVALID_TAG, "reject-invalid-tag" },
    { ERROR_CODE_REJECT_MISSING_REQUIRED_PARAMETER,
      "reject-missing-required-parameter" },
    { ERROR_CODE_REJECT_PARAMETER_OUT_OF_RANGE,
      "reject-parameter-out-of-range" },
    { ERROR_CODE_REJECT_TOO_MANY_ARGUMENTS, "reject-too-many-arguments" },
    { ERROR_CODE_REJECT_UNDEFINED_ENUMERATION, "reject-undefined-enumeration" },
    { ERROR_CODE_REJECT_UNRECOGNIZED_SERVICE, "reject-unrecognized-service" },
    { ERROR_CODE_REJECT_PROPRIETARY, "reject-proprietary" },
    { ERROR_CODE_REJECT_OTHER, "reject-other" },
    { ERROR_CODE_UNKNOWN_DEVICE, "unknown-device" },
    { ERROR_CODE_UNKNOWN_ROUTE, "unknown-route" },
    { ERROR_CODE_VALUE_NOT_INITIALIZED, "value-not-initialized" },
    { ERROR_CODE_INVALID_EVENT_STATE, "invalid-event-state" },
    { ERROR_CODE_NO_ALARM_CONFIGURED, "no-alarm-configured" },
    { ERROR_CODE_LOG_BUFFER_FULL, "log-buffer-full" },
    { ERROR_CODE_LOGGED_VALUE_PURGED, "logged-value-purged" },
    { ERROR_CODE_NO_PROPERTY_SPECIFIED, "no-property-specified" },
    { ERROR_CODE_NOT_CONFIGURED_FOR_TRIGGERED_LOGGING,
      "not-configured-for-triggered-logging" },
    { ERROR_CODE_UNKNOWN_SUBSCRIPTION, "unknown-subscription" },
    { ERROR_CODE_PARAMETER_OUT_OF_RANGE, "parameter-out-of-range" },
    { ERROR_CODE_LIST_ELEMENT_NOT_FOUND, "list-element-not-found" },
    { ERROR_CODE_BUSY, "busy" },
    { ERROR_CODE_COMMUNICATION_DISABLED, "communication-disabled" },
    { ERROR_CODE_COMMUNICATION_DISABLED, "access-denied" },
    { ERROR_CODE_SUCCESS, "success" },
    { ERROR_CODE_ACCESS_DENIED, "access-denied" },
    { ERROR_CODE_BAD_DESTINATION_ADDRESS, "bad-destination-address" },
    { ERROR_CODE_BAD_DESTINATION_DEVICE_ID, "bad-destination-device-id" },
    { ERROR_CODE_BAD_SIGNATURE, "bad-signature" },
    { ERROR_CODE_BAD_SOURCE_ADDRESS, "bad-source-address" },
    { ERROR_CODE_BAD_TIMESTAMP, "bad-timestamp" },
    { ERROR_CODE_CANNOT_USE_KEY, "cannot-use-key" },
    { ERROR_CODE_CANNOT_VERIFY_MESSAGE_ID, "cannot-verify-message-id" },
    { ERROR_CODE_CORRECT_KEY_REVISION, "correct-key-revision" },
    { ERROR_CODE_DESTINATION_DEVICE_ID_REQUIRED,
      "destination-device-id-required" },
    { ERROR_CODE_DUPLICATE_MESSAGE, "duplicate-message" },
    { ERROR_CODE_ENCRYPTION_NOT_CONFIGURED, "encryption-not-configured" },
    { ERROR_CODE_ENCRYPTION_REQUIRED, "encryption-required" },
    { ERROR_CODE_INCORRECT_KEY, "incorrect-key" },
    { ERROR_CODE_INVALID_KEY_DATA, "invalid-key-data" },
    { ERROR_CODE_KEY_UPDATE_IN_PROGRESS, "key-update-in-progress" },
    { ERROR_CODE_MALFORMED_MESSAGE, "malformed-message" },
    { ERROR_CODE_NOT_KEY_SERVER, "not-key-server" },
    { ERROR_CODE_SECURITY_NOT_CONFIGURED, "security-not-configured" },
    { ERROR_CODE_SOURCE_SECURITY_REQUIRED, "source-security-required" },
    { ERROR_CODE_TOO_MANY_KEYS, "too-many-keys" },
    { ERROR_CODE_UNKNOWN_AUTHENTICATION_TYPE, "unknown-authentication-type" },
    { ERROR_CODE_UNKNOWN_KEY, "unknown-key" },
    { ERROR_CODE_UNKNOWN_KEY_REVISION, "unknown-key-revision" },
    { ERROR_CODE_UNKNOWN_SOURCE_MESSAGE, "unknown-source-message" },
    { ERROR_CODE_NOT_ROUTER_TO_DNET, "not-router-to-dnet" },
    { ERROR_CODE_ROUTER_BUSY, "router-busy" },
    { ERROR_CODE_UNKNOWN_NETWORK_MESSAGE, "unknown-network-message" },
    { ERROR_CODE_MESSAGE_TOO_LONG, "message-too-long" },
    { ERROR_CODE_SECURITY_ERROR, "security-error" },
    { ERROR_CODE_ADDRESSING_ERROR, "addressing-error" },
    { ERROR_CODE_WRITE_BDT_FAILED, "write-bdt-failed" },
    { ERROR_CODE_READ_BDT_FAILED, "read-bdt-failed" },
    { ERROR_CODE_REGISTER_FOREIGN_DEVICE_FAILED,
      "register-foreign-device-failed" },
    { ERROR_CODE_READ_FDT_FAILED, "read-fdt-failed" },
    { ERROR_CODE_DELETE_FDT_ENTRY_FAILED, "delete-fdt-entry-failed" },
    { ERROR_CODE_DISTRIBUTE_BROADCAST_FAILED, "distribute-broadcast-failed" },
    { ERROR_CODE_UNKNOWN_FILE_SIZE, "unknown-file-size" },
    { ERROR_CODE_ABORT_APDU_TOO_LONG, "abort-apdu-too-long" },
    { ERROR_CODE_ABORT_APPLICATION_EXCEEDED_REPLY_TIME,
      "abort-application-exceeded-reply-time" },
    { ERROR_CODE_ABORT_OUT_OF_RESOURCES, "abort-out-of-resources" },
    { ERROR_CODE_ABORT_TSM_TIMEOUT, "abort-tsm-timeout" },
    { ERROR_CODE_ABORT_WINDOW_SIZE_OUT_OF_RANGE,
      "abort-window-size-out-of-range" },
    { ERROR_CODE_FILE_FULL, "file-full" },
    { ERROR_CODE_INCONSISTENT_CONFIGURATION, "inconsistent-configuration" },
    { ERROR_CODE_INCONSISTENT_OBJECT_TYPE, "inconsistent-object-type" },
    { ERROR_CODE_INTERNAL_ERROR, "internal-error" },
    { ERROR_CODE_NOT_CONFIGURED, "not-configured" },
    { ERROR_CODE_OUT_OF_MEMORY, "out-of-memory" },
    { ERROR_CODE_VALUE_TOO_LONG, "value-too-long" },
    { ERROR_CODE_ABORT_INSUFFICIENT_SECURITY, "abort-insufficient-security" },
    { ERROR_CODE_ABORT_SECURITY_ERROR, "abort-security-error" },
    { ERROR_CODE_DUPLICATE_ENTRY, "duplicate-entry" },
    { ERROR_CODE_INVALID_VALUE_IN_THIS_STATE, "invalid-value-in-this-state" },
    { ERROR_CODE_INVALID_OPERATION_IN_THIS_STATE,
      "invalid-operation-in-this-state" },
    { ERROR_CODE_LIST_ITEM_NOT_NUMBERED, "list-item-not-numbered" },
    { ERROR_CODE_LIST_ITEM_NOT_TIMESTAMPED, "list-item-not-timestamped" },
    { ERROR_CODE_INVALID_DATA_ENCODING, "invalid-data-encoding" },
    { ERROR_CODE_BVLC_FUNCTION_UNKNOWN, "bvlc-function-unknown" },
    { ERROR_CODE_BVLC_PROPRIETARY_FUNCTION_UNKNOWN,
      "bvlc-proprietary-function-unknown" },
    { ERROR_CODE_HEADER_ENCODING_ERROR, "header-encoding-error" },
    { ERROR_CODE_HEADER_NOT_UNDERSTOOD, "header-not-understood" },
    { ERROR_CODE_MESSAGE_INCOMPLETE, "message-incomplete" },
    { ERROR_CODE_NOT_A_BACNET_SC_HUB, "not-a-bacnet-sc-hub" },
    { ERROR_CODE_PAYLOAD_EXPECTED, "payload-expected" },
    { ERROR_CODE_UNEXPECTED_DATA, "unexpected-data" },
    { ERROR_CODE_NODE_DUPLICATE_VMAC, "node-duplicate-vmac" },
    { ERROR_CODE_HTTP_UNEXPECTED_RESPONSE_CODE,
      "http-unexpected-response-code" },
    { ERROR_CODE_HTTP_NO_UPGRADE, "http-no-upgrade" },
    { ERROR_CODE_HTTP_RESOURCE_NOT_LOCAL, "http-resource-not-local" },
    { ERROR_CODE_HTTP_PROXY_AUTHENTICATION_FAILED,
      "http-proxy-authentication-failed" },
    { ERROR_CODE_HTTP_RESPONSE_TIMEOUT, "http-response-timeout" },
    { ERROR_CODE_HTTP_RESPONSE_SYNTAX_ERROR, "http-response-syntax-error" },
    { ERROR_CODE_HTTP_RESPONSE_VALUE_ERROR, "http-response-value-error" },
    { ERROR_CODE_HTTP_RESPONSE_MISSING_HEADER, "http-response-missing-header" },
    { ERROR_CODE_HTTP_WEBSOCKET_HEADER_ERROR, "http-websocket-header-error" },
    { ERROR_CODE_HTTP_UPGRADE_REQUIRED, "http-upgrade-required" },
    { ERROR_CODE_HTTP_UPGRADE_ERROR, "http-upgrade-error" },
    { ERROR_CODE_HTTP_TEMPORARY_UNAVAILABLE, "http-temporary-unavailable" },
    { ERROR_CODE_HTTP_NOT_A_SERVER, "http-not-a-server" },
    { ERROR_CODE_HTTP_ERROR, "http-error" },
    { ERROR_CODE_WEBSOCKET_SCHEME_NOT_SUPPORTED,
      "websocket-scheme-not-supported" },
    { ERROR_CODE_WEBSOCKET_UNKNOWN_CONTROL_MESSAGE,
      "websocket-unknown-control-message" },
    { ERROR_CODE_WEBSOCKET_CLOSE_ERROR, "websocket-close-error" },
    { ERROR_CODE_WEBSOCKET_CLOSED_BY_PEER, "websocket-closed-by-peer" },
    { ERROR_CODE_WEBSOCKET_ENDPOINT_LEAVES, "websocket-endpoint-leaves" },
    { ERROR_CODE_WEBSOCKET_PROTOCOL_ERROR, "websocket-protocol-error" },
    { ERROR_CODE_WEBSOCKET_DATA_NOT_ACCEPTED, "websocket-data-not-accepted" },
    { ERROR_CODE_WEBSOCKET_CLOSED_ABNORMALLY, "websocket-closed-abnormally" },
    { ERROR_CODE_WEBSOCKET_DATA_INCONSISTENT, "websocket-data-inconsistent" },
    { ERROR_CODE_WEBSOCKET_DATA_AGAINST_POLICY,
      "websocket-data-against-policy" },
    { ERROR_CODE_WEBSOCKET_FRAME_TOO_LONG, "websocket-frame-too-long" },
    { ERROR_CODE_WEBSOCKET_EXTENSION_MISSING, "websocket-extension-missing" },
    { ERROR_CODE_WEBSOCKET_REQUEST_UNAVAILABLE,
      "websocket-request-unavailable" },
    { ERROR_CODE_WEBSOCKET_ERROR, "websocket-error" },
    { ERROR_CODE_TLS_CLIENT_CERTIFICATE_ERROR, "tls-client-certificate-error" },
    { ERROR_CODE_TLS_SERVER_CERTIFICATE_ERROR, "tls-server-certificate-error" },
    { ERROR_CODE_TLS_CLIENT_AUTHENTICATION_FAILED,
      "tls-client-authentication-failed" },
    { ERROR_CODE_TLS_SERVER_AUTHENTICATION_FAILED,
      "tls-server-authentication-failed" },
    { ERROR_CODE_TLS_CLIENT_CERTIFICATE_EXPIRED,
      "tls-client-certificate-expired" },
    { ERROR_CODE_TLS_SERVER_CERTIFICATE_EXPIRED,
      "tls-server-certificate-expired" },
    { ERROR_CODE_TLS_CLIENT_CERTIFICATE_REVOKED,
      "tls-client-certificate-revoked" },
    { ERROR_CODE_TLS_SERVER_CERTIFICATE_REVOKED,
      "tls-server-certificate-revoked" },
    { ERROR_CODE_TLS_ERROR, "tls-error" },
    { ERROR_CODE_DNS_UNAVAILABLE, "dns-unavailable" },
    { ERROR_CODE_DNS_NAME_RESOLUTION_FAILED, "dns-name-resolution-failed" },
    { ERROR_CODE_DNS_RESOLVER_FAILURE, "dns-resolver-failure" },
    { ERROR_CODE_DNS_ERROR, "dns-error" },
    { ERROR_CODE_TCP_CONNECT_TIMEOUT, "tcp-connect-timeout" },
    { ERROR_CODE_TCP_CONNECTION_REFUSED, "tcp-connection-refused" },
    { ERROR_CODE_TCP_CLOSED_BY_LOCAL, "tcp-closed-by-local" },
    { ERROR_CODE_TCP_CLOSED_OTHER, "tcp-closed-other" },
    { ERROR_CODE_TCP_ERROR, "tcp-error" },
    { ERROR_CODE_IP_ADDRESS_NOT_REACHABLE, "ip-address-not-reachable" },
    { ERROR_CODE_IP_ERROR, "ip-error" },
    { ERROR_CODE_CERTIFICATE_EXPIRED, "certificate-expired" },
    { ERROR_CODE_CERTIFICATE_INVALID, "certificate-invalid" },
    { ERROR_CODE_CERTIFICATE_MALFORMED, "certificate-malformed" },
    { ERROR_CODE_CERTIFICATE_REVOKED, "certificate-revoked" },
    { ERROR_CODE_UNKNOWN_SECURITY_KEY, "unknown-security-key" },
    { ERROR_CODE_REFERENCED_PORT_IN_ERROR, "referenced-port-in-error" },
    { ERROR_CODE_NOT_ENABLED, "not-enabled" },
    { ERROR_CODE_ADJUST_SCOPE_REQUIRED, "adjust-scope-required" },
    { ERROR_CODE_AUTH_SCOPE_REQUIRED, "auth-scope-required" },
    { ERROR_CODE_BIND_SCOPE_REQUIRED, "bind-scope-required" },
    { ERROR_CODE_CONFIG_SCOPE_REQUIRED, "config-scope-required" },
    { ERROR_CODE_CONTROL_SCOPE_REQUIRED, "control-scope-required" },
    { ERROR_CODE_EXTENDED_SCOPE_REQUIRED, "extended-scope-required" },
    { ERROR_CODE_INCORRECT_CLIENT, "incorrect-client" },
    { ERROR_CODE_INSTALL_SCOPE_REQUIRED, "install-scope-required" },
    { ERROR_CODE_INSUFFICIENT_SCOPE, "insufficient-scope" },
    { ERROR_CODE_NO_DEFAULT_SCOPE, "no-default-scope" },
    { ERROR_CODE_NO_POLICY, "no-policy" },
    { ERROR_CODE_REVOKED_TOKEN, "revoked-token" },
    { ERROR_CODE_OVERRIDE_SCOPE_REQUIRED, "override-scope-required" },
    { ERROR_CODE_INACTIVE_TOKEN, "inactive-token" },
    { ERROR_CODE_UNKNOWN_AUDIENCE, "unknown-audience" },
    { ERROR_CODE_UNKNOWN_CLIENT, "unknown-client" },
    { ERROR_CODE_UNKNOWN_SCOPE, "unknown-scope" },
    { ERROR_CODE_VIEW_SCOPE_REQUIRED, "view-scope-required" },
    { ERROR_CODE_INCORRECT_AUDIENCE, "incorrect-audience" },
    { ERROR_CODE_INCORRECT_CLIENT_ORIGIN, "incorrect-client-origin" },
    { ERROR_CODE_INVALID_ARRAY_SIZE, "invalid-array-size" },
    { ERROR_CODE_INCORRECT_ISSUER, "incorrect-issuer" },
    { ERROR_CODE_INVALID_TOKEN, "invalid-token" },
    /* Enumerated values 256-65535 may be used by others subject to */
    /* the procedures and constraints described in Clause 23. */
    { 0, NULL }
};

const char *bactext_error_code_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_error_code_names, index, ERROR_CODE_PROPRIETARY_FIRST,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

INDTEXT_DATA bacnet_month_names[] = {
    { 1, "January" },     { 2, "February" },     { 3, "March" },
    { 4, "April" },       { 5, "May" },          { 6, "June" },
    { 7, "July" },        { 8, "August" },       { 9, "September" },
    { 10, "October" },    { 11, "November" },    { 12, "December" },
    { 13, "Odd Months" }, { 14, "Even Months" }, { 255, "Any Month" },
    { 0, NULL }
};

const char *bactext_month_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_month_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_week_of_month_names[] = {
    { 1, "days numbered 1-7" },        { 2, "days numbered 8-14" },
    { 3, "days numbered 15-21" },      { 4, "days numbered 22-28" },
    { 5, "days numbered 29-31" },      { 6, "last 7 days of this month" },
    { 255, "any week of this month" }, { 0, NULL }
};

const char *bactext_week_of_month_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_week_of_month_names, index, ASHRAE_Reserved_String);
}

/* note: different than DaysOfWeek bit string where 0=monday */
INDTEXT_DATA bacnet_day_of_week_names[] = {
    { 1, "Monday" },    { 2, "Tuesday" },
    { 3, "Wednesday" }, { 4, "Thursday" },
    { 5, "Friday" },    { 6, "Saturday" },
    { 7, "Sunday" },    { 255, "any day of week" },
    { 0, NULL }
};

const char *bactext_day_of_week_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_day_of_week_names, index, ASHRAE_Reserved_String);
}

/* note: different than DayOfWeek bit string where 1=monday */
INDTEXT_DATA bacnet_days_of_week_names[] = {
    { BACNET_DAYS_OF_WEEK_MONDAY, "Monday" },
    { BACNET_DAYS_OF_WEEK_TUESDAY, "Tuesday" },
    { BACNET_DAYS_OF_WEEK_WEDNESDAY, "Wednesday" },
    { BACNET_DAYS_OF_WEEK_THURSDAY, "Thursday" },
    { BACNET_DAYS_OF_WEEK_FRIDAY, "Friday" },
    { BACNET_DAYS_OF_WEEK_SATURDAY, "Saturday" },
    { BACNET_DAYS_OF_WEEK_SUNDAY, "Sunday" },
    { 0, NULL }
};

const char *bactext_days_of_week_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_days_of_week_names, index, ASHRAE_Reserved_String);
}

bool bactext_days_of_week_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_days_of_week_names, search_name, found_index);
}

INDTEXT_DATA bacnet_notify_type_names[] = { { NOTIFY_ALARM, "alarm" },
                                            { NOTIFY_EVENT, "event" },
                                            { NOTIFY_ACK_NOTIFICATION,
                                              "ack-notification" },
                                            { 0, NULL } };

const char *bactext_notify_type_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_notify_type_names, index, ASHRAE_Reserved_String);
}

bool bactext_notify_type_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_notify_type_names, search_name, found_index);
}

INDTEXT_DATA bacnet_event_transition_names[] = {
    { TRANSITION_TO_OFFNORMAL, "offnormal" },
    { TRANSITION_TO_NORMAL, "normal" },
    { TRANSITION_TO_FAULT, "fault" },
    { 0, NULL }
};

const char *bactext_event_transition_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_event_transition_names, index, ASHRAE_Reserved_String);
}

bool bactext_event_transition_index(
    const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_event_transition_names, search_name, found_index);
}

INDTEXT_DATA bacnet_event_state_names[] = {
    { EVENT_STATE_NORMAL, "normal" },
    { EVENT_STATE_FAULT, "fault" },
    { EVENT_STATE_OFFNORMAL, "offnormal" },
    { EVENT_STATE_HIGH_LIMIT, "high-limit" },
    { EVENT_STATE_LOW_LIMIT, "low-limit" },
    { EVENT_STATE_LIFE_SAFETY_ALARM, "life-safety-alarm" },
    { 0, NULL }
};

const char *bactext_event_state_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_event_state_names, index, ASHRAE_Reserved_String);
}

bool bactext_event_state_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_event_state_names, search_name, found_index);
}

bool bactext_event_state_strtol(const char *search_name, unsigned *found_index)
{
    return bactext_strtol_index(
        bacnet_event_state_names, search_name, found_index);
}

INDTEXT_DATA bacnet_event_type_names[] = {
    { EVENT_CHANGE_OF_BITSTRING, "change-of-bitstring" },
    { EVENT_CHANGE_OF_STATE, "change-of-state" },
    { EVENT_CHANGE_OF_VALUE, "change-of-value" },
    { EVENT_COMMAND_FAILURE, "command-failure" },
    { EVENT_FLOATING_LIMIT, "floating-limit" },
    { EVENT_OUT_OF_RANGE, "out-of-range" },
    { EVENT_CHANGE_OF_LIFE_SAFETY, "change-of-life-safety" },
    { EVENT_EXTENDED, "extended" },
    { EVENT_BUFFER_READY, "buffer-ready" },
    { EVENT_UNSIGNED_RANGE, "unsigned-range" },
    { EVENT_ACCESS_EVENT, "access-event" },
    { EVENT_DOUBLE_OUT_OF_RANGE, "double-out-of-range" },
    { EVENT_SIGNED_OUT_OF_RANGE, "signed-out-of-range" },
    { EVENT_UNSIGNED_OUT_OF_RANGE, "unsigned-out-of-range" },
    { EVENT_CHANGE_OF_CHARACTERSTRING, "change-of-characterstring" },
    { EVENT_CHANGE_OF_STATUS_FLAGS, "change-of-status-flags" },
    { EVENT_CHANGE_OF_RELIABILITY, "change-of-reliability" },
    { EVENT_NONE, "none" },
    { EVENT_CHANGE_OF_DISCRETE_VALUE, "change-of-discrete-value" },
    { EVENT_CHANGE_OF_TIMER, "change-of-timer" },
    { 0, NULL }
};

const char *bactext_event_type_name(unsigned index)
{
    return indtext_by_index_split_default(
        bacnet_event_type_names, index, EVENT_PROPRIETARY_MIN,
        ASHRAE_Reserved_String, Vendor_Proprietary_String);
}

bool bactext_event_type_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_event_type_names, search_name, found_index);
}

INDTEXT_DATA bacnet_binary_present_value_names[] = {
    { BINARY_INACTIVE, "inactive" }, { BINARY_ACTIVE, "active" }, { 0, NULL }
};

const char *bactext_binary_present_value_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_binary_present_value_names, index, ASHRAE_Reserved_String);
}

bool bactext_binary_present_value_index(
    const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_binary_present_value_names, search_name, found_index);
}

INDTEXT_DATA bacnet_binary_polarity_names[] = { { POLARITY_NORMAL, "normal" },
                                                { POLARITY_REVERSE, "reverse" },
                                                { 0, NULL } };

const char *bactext_binary_polarity_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_binary_polarity_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_reliability_names[] = {
    { RELIABILITY_NO_FAULT_DETECTED, "no-fault-detected" },
    { RELIABILITY_NO_SENSOR, "no-sensor" },
    { RELIABILITY_OVER_RANGE, "over-range" },
    { RELIABILITY_UNDER_RANGE, "under-range" },
    { RELIABILITY_OPEN_LOOP, "open-loop" },
    { RELIABILITY_SHORTED_LOOP, "shorted-loop" },
    { RELIABILITY_NO_OUTPUT, "no-output" },
    { RELIABILITY_UNRELIABLE_OTHER, "unreliable-other" },
    { RELIABILITY_PROCESS_ERROR, "process-error" },
    { RELIABILITY_MULTI_STATE_FAULT, "mult-state-fault" },
    { RELIABILITY_CONFIGURATION_ERROR, "configuration-error" },
    { RELIABILITY_COMMUNICATION_FAILURE, "communication-failure" },
    { RELIABILITY_MEMBER_FAULT, "member-fault" },
    { RELIABILITY_MONITORED_OBJECT_FAULT, "monitored-object-fault" },
    { RELIABILITY_TRIPPED, "tripped" },
    { RELIABILITY_LAMP_FAILURE, "lamp-failure" },
    { RELIABILITY_ACTIVATION_FAILURE, "activation-failure" },
    { RELIABILITY_RENEW_DHCP_FAILURE, "renew-dhcp-failure" },
    { RELIABILITY_RENEW_FD_REGISTRATION_FAILURE,
      "renew-fd-registration-failure" },
    { RELIABILITY_RESTART_AUTO_NEGOTIATION_FAILURE,
      "restart-auto-negotiation-failure" },
    { RELIABILITY_RESTART_FAILURE, "restart-failure" },
    { RELIABILITY_PROPRIETARY_COMMAND_FAILURE, "proprietary-command-failure" },
    { RELIABILITY_FAULTS_LISTED, "faults-listed" },
    { RELIABILITY_REFERENCED_OBJECT_FAULT, "referenced-object-fault" },
    { RELIABILITY_MULTI_STATE_OUT_OF_RANGE, "multi-state-out-of-range" },
    { 0, NULL }
};

const char *bactext_reliability_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_reliability_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_device_status_names[] = {
    { STATUS_OPERATIONAL, "operational" },
    { STATUS_OPERATIONAL_READ_ONLY, "operational-read-only" },
    { STATUS_DOWNLOAD_REQUIRED, "download-required" },
    { STATUS_DOWNLOAD_IN_PROGRESS, "download-in-progress" },
    { STATUS_NON_OPERATIONAL, "non-operational" },
    { STATUS_BACKUP_IN_PROGRESS, "backup-in-progress" },
    { 0, NULL }
};

const char *bactext_device_status_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_device_status_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_segmentation_names[] = {
    { SEGMENTATION_BOTH, "segmented-both" },
    { SEGMENTATION_TRANSMIT, "segmented-transmit" },
    { SEGMENTATION_RECEIVE, "segmented-receive" },
    { SEGMENTATION_NONE, "no-segmentation" },
    { 0, NULL }
};

const char *bactext_segmentation_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_segmentation_names, index, ASHRAE_Reserved_String);
}

bool bactext_segmentation_index(const char *search_name, unsigned *found_index)
{
    return indtext_by_istring(
        bacnet_segmentation_names, search_name, found_index);
}

INDTEXT_DATA bacnet_node_type_names[] = {
    { BACNET_NODE_UNKNOWN, "unknown" },
    { BACNET_NODE_SYSTEM, "system" },
    { BACNET_NODE_NETWORK, "network" },
    { BACNET_NODE_DEVICE, "device" },
    { BACNET_NODE_ORGANIZATIONAL, "organizational" },
    { BACNET_NODE_AREA, "area" },
    { BACNET_NODE_EQUIPMENT, "equipment" },
    { BACNET_NODE_POINT, "point" },
    { BACNET_NODE_COLLECTION, "collection" },
    { BACNET_NODE_PROPERTY, "property" },
    { BACNET_NODE_FUNCTIONAL, "functional" },
    { BACNET_NODE_OTHER, "other" },
    { 0, NULL }
};

const char *bactext_node_type_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_node_type_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA network_layer_msg_names[] = {
    { NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK, "Who-Is-Router-To-Network" },
    { NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK, "I-Am-Router-To-Network" },
    { NETWORK_MESSAGE_I_COULD_BE_ROUTER_TO_NETWORK,
      "I-Could-Be-Router-To-Network" },
    { NETWORK_MESSAGE_REJECT_MESSAGE_TO_NETWORK, "Reject-Message-to-Network" },
    { NETWORK_MESSAGE_ROUTER_BUSY_TO_NETWORK, "Router-Busy-To-Network" },
    { NETWORK_MESSAGE_ROUTER_AVAILABLE_TO_NETWORK,
      "Router-Available-To-Network" },
    { NETWORK_MESSAGE_INIT_RT_TABLE, "Initialize-Routing-Table" },
    { NETWORK_MESSAGE_INIT_RT_TABLE_ACK, "Initialize-Routing-Table-Ack" },
    { NETWORK_MESSAGE_ESTABLISH_CONNECTION_TO_NETWORK,
      "Est-Conn-Ntwk" }, /* Terse since unused */
    { NETWORK_MESSAGE_DISCONNECT_CONNECTION_TO_NETWORK, "Dsc-Conn-Ntwk" },
    { 0, NULL }
};

const char *bactext_network_layer_msg_name(unsigned index)
{
    if (index <= 0x7F) {
        return indtext_by_index_default(
            network_layer_msg_names, index, ASHRAE_Reserved_String);
    } else if (index < NETWORK_MESSAGE_INVALID) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid Network Layer Message";
    }
}

INDTEXT_DATA bactext_life_safety_mode_names[] = {
    { LIFE_SAFETY_MODE_OFF, "off" },
    { LIFE_SAFETY_MODE_ON, "on" },
    { LIFE_SAFETY_MODE_TEST, "test" },
    { LIFE_SAFETY_MODE_MANNED, "manned" },
    { LIFE_SAFETY_MODE_UNMANNED, "unmanned" },
    { LIFE_SAFETY_MODE_ARMED, "armed" },
    { LIFE_SAFETY_MODE_DISARMED, "disarmed" },
    { LIFE_SAFETY_MODE_PREARMED, "prearmed" },
    { LIFE_SAFETY_MODE_SLOW, "slow" },
    { LIFE_SAFETY_MODE_FAST, "fast" },
    { LIFE_SAFETY_MODE_DISCONNECTED, "disconnected" },
    { LIFE_SAFETY_MODE_ENABLED, "enabled" },
    { LIFE_SAFETY_MODE_DISABLED, "disabled" },
    { LIFE_SAFETY_MODE_AUTOMATIC_RELEASE_DISABLED,
      "automatic-release-disabled" },
    { LIFE_SAFETY_MODE_DEFAULT, "default" },
    { LIFE_SAFETY_MODE_ACTIVATED_OEO_ALARM, "activated-oeo-alarm" },
    { LIFE_SAFETY_MODE_ACTIVATED_OEO_EVACUATE, "activated-oeo-evactuate" },
    { LIFE_SAFETY_MODE_ACTIVATED_OEO_PHASE1_RECALL,
      "activated-oeo-phase1-recall" },
    { LIFE_SAFETY_MODE_ACTIVATED_OEO_UNAVAILABLE, "activated-oeo-unavailable" },
    { LIFE_SAFETY_MODE_DEACTIVATED, "deactivated" },
    { 0, NULL }
};

const char *bactext_life_safety_mode_name(unsigned index)
{
    if (index < LIFE_SAFETY_MODE_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bactext_life_safety_mode_names, index, ASHRAE_Reserved_String);
    } else if (index <= LIFE_SAFETY_MODE_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetLifeSafetyMode";
    }
}

INDTEXT_DATA bactext_life_safety_operation_names[] = {
    { LIFE_SAFETY_OP_NONE, "none" },
    { LIFE_SAFETY_OP_SILENCE, "silence" },
    { LIFE_SAFETY_OP_SILENCE_AUDIBLE, "silence-audible" },
    { LIFE_SAFETY_OP_SILENCE_VISUAL, "silence-visual" },
    { LIFE_SAFETY_OP_RESET, "reset" },
    { LIFE_SAFETY_OP_RESET_ALARM, "reset-alarm" },
    { LIFE_SAFETY_OP_RESET_FAULT, "reset-fault" },
    { LIFE_SAFETY_OP_UNSILENCE, "unsilence" },
    { LIFE_SAFETY_OP_UNSILENCE_AUDIBLE, "unsilence-audible" },
    { LIFE_SAFETY_OP_UNSILENCE_VISUAL, "unsilence-visual" },
    { 0, NULL }
};

const char *bactext_life_safety_operation_name(unsigned index)
{
    if (index < LIFE_SAFETY_OP_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bactext_life_safety_operation_names, index, ASHRAE_Reserved_String);
    } else if (index <= LIFE_SAFETY_OP_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetLifeSafetyOperation";
    }
}

INDTEXT_DATA life_safety_state_names[] = {
    { LIFE_SAFETY_STATE_QUIET, "quiet" },
    { LIFE_SAFETY_STATE_PRE_ALARM, "pre-alarm" },
    { LIFE_SAFETY_STATE_ALARM, "alarm" },
    { LIFE_SAFETY_STATE_FAULT, "fault" },
    { LIFE_SAFETY_STATE_FAULT_PRE_ALARM, "fault-pre-alarm" },
    { LIFE_SAFETY_STATE_FAULT_ALARM, "fault-alarm" },
    { LIFE_SAFETY_STATE_NOT_READY, "not-ready" },
    { LIFE_SAFETY_STATE_ACTIVE, "active" },
    { LIFE_SAFETY_STATE_TAMPER, "tamper" },
    { LIFE_SAFETY_STATE_TEST_ALARM, "test-alarm" },
    { LIFE_SAFETY_STATE_TEST_ACTIVE, "test-active" },
    { LIFE_SAFETY_STATE_TEST_FAULT, "test-fault" },
    { LIFE_SAFETY_STATE_TEST_FAULT_ALARM, "fault-alarm" },
    { LIFE_SAFETY_STATE_HOLDUP, "holdupt" },
    { LIFE_SAFETY_STATE_DURESS, "duress" },
    { LIFE_SAFETY_STATE_TAMPER_ALARM, "tamper-alarm" },
    { LIFE_SAFETY_STATE_ABNORMAL, "abnormal" },
    { LIFE_SAFETY_STATE_EMERGENCY_POWER, "emergency-power" },
    { LIFE_SAFETY_STATE_DELAYED, "delayed" },
    { LIFE_SAFETY_STATE_BLOCKED, "blocked" },
    { LIFE_SAFETY_STATE_LOCAL_ALARM, "local-alarm" },
    { LIFE_SAFETY_STATE_GENERAL_ALARM, "general-alarm" },
    { LIFE_SAFETY_STATE_SUPERVISORY, "supervisory" },
    { LIFE_SAFETY_STATE_TEST_SUPERVISORY, "test-supervisory" },
    { LIFE_SAFETY_STATE_NON_DEFAULT_MODE, "non-default-mode" },
    { LIFE_SAFETY_STATE_OEO_UNAVAILABLE, "oeo-unavailable" },
    { LIFE_SAFETY_STATE_OEO_ALARM, "oeo-alarm" },
    { LIFE_SAFETY_STATE_OEO_PHASE1_RECALL, "oeo-phase1-recall" },
    { LIFE_SAFETY_STATE_OEO_EVACUATE, "oeo-evacuate" },
    { LIFE_SAFETY_STATE_OEO_UNAFFECTED, "oeo-unaffected" },
    { LIFE_SAFETY_STATE_TEST_OEO_UNAVAILABLE, "test-oeo-unavailable" },
    { LIFE_SAFETY_STATE_TEST_OEO_ALARM, "test-oeo-alarm" },
    { LIFE_SAFETY_STATE_TEST_OEO_PHASE1_RECALL, "test-oeo-phase1-recall" },
    { LIFE_SAFETY_STATE_TEST_OEO_EVACUATE, "test-oeo-evacuate" },
    { LIFE_SAFETY_STATE_TEST_OEO_UNAFFECTED, "test-oeo-unaffected" },
    { 0, NULL }
};

const char *bactext_life_safety_state_name(unsigned index)
{
    if (index < LIFE_SAFETY_STATE_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            life_safety_state_names, index, ASHRAE_Reserved_String);
    } else if (index <= LIFE_SAFETY_STATE_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetLifeSafetyState";
    }
}

INDTEXT_DATA lighting_in_progress[] = {
    { BACNET_LIGHTING_IDLE, "idle" },
    { BACNET_LIGHTING_FADE_ACTIVE, "fade" },
    { BACNET_LIGHTING_RAMP_ACTIVE, "ramp" },
    { BACNET_LIGHTING_NOT_CONTROLLED, "not-controlled" },
    { BACNET_LIGHTING_OTHER, "other" },
    { BACNET_LIGHTING_TRIM_ACTIVE, "trim-active" },
    { 0, NULL }
};

const char *bactext_lighting_in_progress(unsigned index)
{
    if (index < MAX_BACNET_LIGHTING_IN_PROGRESS) {
        return indtext_by_index_default(
            lighting_in_progress, index, ASHRAE_Reserved_String);
    } else {
        return "Invalid BACnetLightingInProgress";
    }
}

INDTEXT_DATA lighting_transition[] = {
    { BACNET_LIGHTING_TRANSITION_NONE, "none" },
    { BACNET_LIGHTING_TRANSITION_FADE, "fade" },
    { BACNET_LIGHTING_TRANSITION_RAMP, "ramp" },
    { 0, NULL }
};

const char *bactext_lighting_transition(unsigned index)
{
    if (index < BACNET_LIGHTING_TRANSITION_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            lighting_transition, index, ASHRAE_Reserved_String);
    } else if (index <= BACNET_LIGHTING_TRANSITION_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetLightingTransition";
    }
}

INDTEXT_DATA bacnet_lighting_operation_names[] = {
    { BACNET_LIGHTS_NONE, "none" },
    { BACNET_LIGHTS_FADE_TO, "fade-to" },
    { BACNET_LIGHTS_RAMP_TO, "ramp-to" },
    { BACNET_LIGHTS_STEP_UP, "step-up" },
    { BACNET_LIGHTS_STEP_DOWN, "step-down" },
    { BACNET_LIGHTS_STEP_ON, "step-on" },
    { BACNET_LIGHTS_STEP_OFF, "step-off" },
    { BACNET_LIGHTS_WARN, "warn" },
    { BACNET_LIGHTS_WARN_OFF, "warn-off" },
    { BACNET_LIGHTS_WARN_RELINQUISH, "warn-relinquish" },
    { BACNET_LIGHTS_STOP, "stop" },
    { 0, NULL }
};

const char *bactext_lighting_operation_name(unsigned index)
{
    if (index < BACNET_LIGHTS_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bacnet_lighting_operation_names, index, ASHRAE_Reserved_String);
    } else if (index <= BACNET_LIGHTS_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetLightingOperation";
    }
}

bool bactext_lighting_operation_strtol(
    const char *search_name, unsigned *found_index)
{
    return bactext_strtol_index(
        bacnet_lighting_operation_names, search_name, found_index);
}

INDTEXT_DATA bacnet_binary_lighting_pv_names[] = {
    { BINARY_LIGHTING_PV_OFF, "off" },
    { BINARY_LIGHTING_PV_ON, "on" },
    { BINARY_LIGHTING_PV_WARN, "warn" },
    { BINARY_LIGHTING_PV_WARN_OFF, "warn-off" },
    { BINARY_LIGHTING_PV_WARN_RELINQUISH, "warn-relinquish" },
    { BINARY_LIGHTING_PV_STOP, "stop" },
    { BINARY_LIGHTING_PV_TOGGLE, "toggle" },
    { 0, NULL }
};

const char *bactext_binary_lighting_pv_name(unsigned index)
{
    if (index < BINARY_LIGHTING_PV_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bacnet_binary_lighting_pv_names, index, ASHRAE_Reserved_String);
    } else if (index <= BINARY_LIGHTING_PV_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetBinaryLightingPV";
    }
}

bool bactext_binary_lighting_pv_names_strtol(
    const char *search_name, unsigned *found_index)
{
    return bactext_strtol_index(
        bacnet_binary_lighting_pv_names, search_name, found_index);
}

INDTEXT_DATA bacnet_color_operation_names[] = {
    { BACNET_COLOR_OPERATION_NONE, "none" },
    { BACNET_COLOR_OPERATION_FADE_TO_COLOR, "fade-to-color" },
    { BACNET_COLOR_OPERATION_FADE_TO_CCT, "fade-to-cct" },
    { BACNET_COLOR_OPERATION_RAMP_TO_CCT, "ramp-to-cct" },
    { BACNET_COLOR_OPERATION_STEP_UP_CCT, "step-up-cct" },
    { BACNET_COLOR_OPERATION_STEP_DOWN_CCT, "step-down-cct" },
    { BACNET_COLOR_OPERATION_STOP, "stop" },
    { 0, NULL }
};

const char *bactext_color_operation_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_color_operation_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_device_communications_names[] = {
    { COMMUNICATION_ENABLE, "enabled" },
    { COMMUNICATION_DISABLE, "disabled" },
    { COMMUNICATION_DISABLE_INITIATION, "initiation disabled" },
    { 0, NULL }
};

const char *bactext_device_communications_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_device_communications_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_shed_state_names[] = {
    { BACNET_SHED_INACTIVE, "shed-inactive" },
    { BACNET_SHED_REQUEST_PENDING, "shed-request-pending" },
    { BACNET_SHED_COMPLIANT, "shed-compliant" },
    { BACNET_SHED_NON_COMPLIANT, "shed-non-compliant" },
    { 0, NULL }
};

const char *bactext_shed_state_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_shed_state_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_shed_level_type_names[] = {
    { BACNET_SHED_TYPE_PERCENT, "percent" },
    { BACNET_SHED_TYPE_LEVEL, "level" },
    { BACNET_SHED_TYPE_AMOUNT, "amount" },
    { 0, NULL }
};

const char *bactext_shed_level_type_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_shed_level_type_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_log_datum_names[] = {
    { BACNET_LOG_DATUM_STATUS, "status" },
    { BACNET_LOG_DATUM_BOOLEAN, "boolean" },
    { BACNET_LOG_DATUM_REAL, "real" },
    { BACNET_LOG_DATUM_ENUMERATED, "enumerated" },
    { BACNET_LOG_DATUM_UNSIGNED, "unsigned" },
    { BACNET_LOG_DATUM_SIGNED, "signed" },
    { BACNET_LOG_DATUM_BITSTRING, "bitstring" },
    { BACNET_LOG_DATUM_NULL, "null" },
    { BACNET_LOG_DATUM_FAILURE, "failure" },
    { BACNET_LOG_DATUM_TIME_CHANGE, "time-change" },
    { BACNET_LOG_DATUM_ANY, "any" },
    { 0, NULL }
};

const char *bactext_log_datum_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_log_datum_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_restart_reason_names[] = {
    { RESTART_REASON_UNKNOWN, "unknown" },
    { RESTART_REASON_COLDSTART, "coldstart" },
    { RESTART_REASON_WARMSTART, "warmstart" },
    { RESTART_REASON_DETECTED_POWER_LOST, "detected-power-lost" },
    { RESTART_REASON_DETECTED_POWER_OFF, "detected-power-off" },
    { RESTART_REASON_HARDWARE_WATCHDOG, "hardware-watchdog" },
    { RESTART_REASON_SOFTWARE_WATCHDOG, "software-watchdog" },
    { RESTART_REASON_SUSPENDED, "suspended" },
    { RESTART_REASON_ACTIVATE_CHANGES, "activate-changes" },
    { 0, NULL }
};

const char *bactext_restart_reason_name(unsigned index)
{
    if (index < RESTART_REASON_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bactext_restart_reason_names, index, ASHRAE_Reserved_String);
    } else if (index <= RESTART_REASON_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetRestartReason";
    }
}

INDTEXT_DATA bactext_network_port_type_names[] = {
    { PORT_TYPE_ETHERNET, "ethernet" },
    { PORT_TYPE_ARCNET, "arcnet" },
    { PORT_TYPE_MSTP, "mstp" },
    { PORT_TYPE_PTP, "ptp" },
    { PORT_TYPE_LONTALK, "lontalk" },
    { PORT_TYPE_BIP, "ipv4" },
    { PORT_TYPE_ZIGBEE, "zigbee" },
    { PORT_TYPE_VIRTUAL, "virtual" },
    { PORT_TYPE_NON_BACNET, "non-bacnet" },
    { PORT_TYPE_BIP6, "ipv6" },
    { PORT_TYPE_SERIAL, "serial" },
    { PORT_TYPE_BSC, "secure-connect" },
    { PORT_TYPE_BSC_INTERIM, "secure-connect-interim" },
    { 0, NULL }
};

const char *bactext_network_port_type_name(unsigned index)
{
    if (index <= PORT_TYPE_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bactext_network_port_type_names, index, ASHRAE_Reserved_String);
    } else if (index <= PORT_TYPE_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetNetworkType";
    }
}

INDTEXT_DATA bactext_network_number_quality_names[] = {
    { PORT_QUALITY_UNKNOWN, "unknown" },
    { PORT_QUALITY_LEARNED, "learned" },
    { PORT_QUALITY_LEARNED_CONFIGURED, "learned-configured" },
    { PORT_QUALITY_CONFIGURED, "configured" },
    { 0, NULL }
};

const char *bactext_network_number_quality_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_network_number_quality_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_network_port_command_names[] = {
    { PORT_COMMAND_IDLE, "idle" },
    { PORT_COMMAND_DISCARD_CHANGES, "discard-changes" },
    { PORT_COMMAND_RENEW_FD_REGISTRATION, "renew-fd-registration" },
    { PORT_COMMAND_RESTART_SLAVE_DISCOVERY, "restart-slave-discovery" },
    { PORT_COMMAND_RENEW_DHCP, "renew-dhcp" },
    { PORT_COMMAND_RESTART_AUTONEGOTIATION, "restart-autonegotiation" },
    { PORT_COMMAND_DISCONNECT, "disconnect" },
    { PORT_COMMAND_RESTART_PORT, "restart-port" },
    { PORT_COMMAND_RESTART_DISCOVERY, "restart-discovery" },
    { PORT_COMMAND_GENERATE_CSR_FILE, "generate-csr-file" },
    { PORT_COMMAND_VALIDATE_CHANGES, "validate-changes" },
    { 0, NULL }
};

const char *bactext_network_port_command_name(unsigned index)
{
    if (index < PORT_COMMAND_PROPRIETARY_MIN) {
        return indtext_by_index_default(
            bactext_network_port_command_names, index, ASHRAE_Reserved_String);
    } else if (index <= PORT_COMMAND_PROPRIETARY_MAX) {
        return Vendor_Proprietary_String;
    } else {
        return "Invalid BACnetNetworkPortCommand";
    }
}

INDTEXT_DATA bactext_authentication_decision_names[] = {
    { BACNET_AUTHENTICATION_DECISION_ALLOW_MATCH, "allow-match" },
    { BACNET_AUTHENTICATION_DECISION_DENY_MISMATCH, "deny-mismatch" },
    { BACNET_AUTHENTICATION_DECISION_DENY_NON_RELAY, "deny-non-relay" },
    { 0, NULL }
};

const char *bactext_authentication_decision_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_authentication_decision_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_authorization_posture_names[] = {
    { BACNET_AUTHORIZATION_POSTURE_OPEN, "open" },
    { BACNET_AUTHORIZATION_POSTURE_PROPRIETARY, "proprietary" },
    { BACNET_AUTHORIZATION_POSTURE_CONFIGURED, "configured" },
    { BACNET_AUTHORIZATION_POSTURE_MISCONFIGURED_PARTIAL,
      "misconfigured-partial" },
    { BACNET_AUTHORIZATION_POSTURE_MISCONFIGURED_TOTAL, "misconfigured-total" },
    { 0, NULL }
};

const char *bactext_authorization_posture_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_authorization_posture_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_fault_type_names[] = {
    { BACNET_FAULT_TYPE_NONE, "none" },
    { BACNET_FAULT_TYPE_CHARACTERSTRING, "characterstring" },
    { BACNET_FAULT_TYPE_EXTENDED, "extended" },
    { BACNET_FAULT_TYPE_LIFE_SAFETY, "life-safety" },
    { BACNET_FAULT_TYPE_STATE, "state" },
    { BACNET_FAULT_TYPE_STATUS_FLAGS, "status-flags" },
    { BACNET_FAULT_TYPE_OUT_OF_RANGE, "out-of-range" },
    { BACNET_FAULT_TYPE_LISTED, "listed" },
    { 0, NULL }
};

const char *bactext_fault_type_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_fault_type_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bacnet_priority_filter_names[] = {
    { BACNET_PRIORITY_FILTER_MANUAL_LIFE_SAFETY, "manual-life-safety" },
    { BACNET_PRIORITY_FILTER_AUTOMATIC_LIFE_SAFETY, "automatic-life-safety" },
    { BACNET_PRIORITY_FILTER_PRIORITY_3, "priority-3" },
    { BACNET_PRIORITY_FILTER_PRIORITY_4, "priority-5" },
    { BACNET_PRIORITY_FILTER_CRITICAL_EQUIPMENT_CONTROLS,
      "critical-equipment-controls" },
    { BACNET_PRIORITY_FILTER_MINIMUM_ON_OFF, "minimum-on-off" },
    { BACNET_PRIORITY_FILTER_PRIORITY_7, "priority-7" },
    { BACNET_PRIORITY_FILTER_MANUAL_OPERATOR, "manual-operator" },
    { BACNET_PRIORITY_FILTER_PRIORITY_9, "priority-9" },
    { BACNET_PRIORITY_FILTER_PRIORITY_10, "priority-10" },
    { BACNET_PRIORITY_FILTER_PRIORITY_11, "priority-11" },
    { BACNET_PRIORITY_FILTER_PRIORITY_12, "priority-12" },
    { BACNET_PRIORITY_FILTER_PRIORITY_13, "priority-13" },
    { BACNET_PRIORITY_FILTER_PRIORITY_14, "priority-14" },
    { BACNET_PRIORITY_FILTER_PRIORITY_15, "priority-15" },
    { BACNET_PRIORITY_FILTER_PRIORITY_16, "priority-16" },
    { 0, NULL }
};

const char *bacnet_priority_filter_name(unsigned index)
{
    return indtext_by_index_default(
        bacnet_priority_filter_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_result_flags_names[] = {
    { RESULT_FLAG_FIRST_ITEM, "first-item" },
    { RESULT_FLAG_LAST_ITEM, "last-item" },
    { RESULT_FLAG_MORE_ITEMS, "more-items" },
    { 0, NULL }
};

const char *bactext_result_flags_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_result_flags_names, index, ASHRAE_Reserved_String);
}

INDTEXT_DATA bactext_success_filter_names[] = {
    { BACNET_SUCCESS_FILTER_ALL, "all" },
    { BACNET_SUCCESS_FILTER_SUCCESS_ONLY, "success-only" },
    { BACNET_SUCCESS_FILTER_FAILURES_ONLY, "failures-only" },
    { 0, NULL }
};

const char *bactext_success_filter_name(unsigned index)
{
    return indtext_by_index_default(
        bactext_success_filter_names, index, ASHRAE_Reserved_String);
}
