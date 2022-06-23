#include <Arduino.h>
#include "HADictionary.h"

// decorators
const char HASerializerSlash[] PROGMEM = {"/"};
const char HASerializerJsonDataPrefix[] PROGMEM = {"{"};
const char HASerializerJsonDataSuffix[] PROGMEM = {"}"};
const char HASerializerJsonPropertyPrefix[] PROGMEM = {"\""};
const char HASerializerJsonPropertySuffix[] PROGMEM = {"\":"};
const char HASerializerJsonEscapeChar[] PROGMEM = {"\""};
const char HASerializerJsonPropertiesSeparator[] PROGMEM = {","};
const char HASerializerJsonArrayPrefix[] PROGMEM = {"["};
const char HASerializerJsonArraySuffix[] PROGMEM = {"]"};
const char HASerializerUnderscore[] PROGMEM = {"_"};

// properties
const char HADeviceIdentifiersProperty[] PROGMEM = {"ids"};
const char HADeviceManufacturerProperty[] PROGMEM = {"mf"};
const char HADeviceModelProperty[] PROGMEM = {"mdl"};
const char HADeviceSoftwareVersionProperty[] PROGMEM = {"sw"};
const char HANameProperty[] PROGMEM = {"name"};
const char HAUniqueIdProperty[] PROGMEM = {"uniq_id"};
const char HADeviceProperty[] PROGMEM = {"dev"};
const char HADeviceClassProperty[] PROGMEM = {"dev_cla"};
const char HAIconProperty[] PROGMEM = {"ic"};
const char HARetainProperty[] PROGMEM = {"ret"};
const char HASourceTypeProperty[] PROGMEM = {"src_type"};
const char HAEncodingProperty[] PROGMEM = {"e"};
const char HAOptimisticProperty[] PROGMEM = {"opt"};
const char HAAutomationTypeProperty[] PROGMEM = {"atype"};
const char HATypeProperty[] PROGMEM = {"type"};
const char HASubtypeProperty[] PROGMEM = {"stype"};
const char HAForceUpdateProperty[] PROGMEM = {"frc_upd"};
const char HAUnitOfMeasurementProperty[] PROGMEM = {"unit_of_meas"};
const char HAValueTemplateProperty[] PROGMEM = {"val_tpl"};

// topics
const char HAConfigTopic[] PROGMEM = {"config"};
const char HAAvailabilityTopic[] PROGMEM = {"avty_t"};
const char HATopic[] PROGMEM = {"t"};
const char HAStateTopic[] PROGMEM = {"stat_t"};
const char HACommandTopic[] PROGMEM = {"cmd_t"};
const char HAPositionTopic[] PROGMEM = {"pos_t"};

// misc
const char HAOnline[] PROGMEM = {"online"};
const char HAOffline[] PROGMEM = {"offline"};
const char HAStateOn[] PROGMEM = {"on"};
const char HAStateOff[] PROGMEM = {"off"};
const char HAStateLocked[] PROGMEM = {"locked"};
const char HAStateUnlocked[] PROGMEM = {"unlocked"};
const char HATrue[] PROGMEM = {"true"};
const char HAFalse[] PROGMEM = {"false"};
const char HAHome[] PROGMEM = {"home"};
const char HANotHome[] PROGMEM = {"not_home"};
const char HATrigger[] PROGMEM = {"trigger"};

// covers
const char HAClosedState[] PROGMEM = {"closed"};
const char HAClosingState[] PROGMEM = {"closing"};
const char HAOpenState[] PROGMEM = {"open"};
const char HAOpeningState[] PROGMEM = {"opening"};
const char HAStoppedState[] PROGMEM = {"stopped"};

// commands
const char HAOpenCommand[] PROGMEM = {"OPEN"};
const char HACloseCommand[] PROGMEM = {"CLOSE"};
const char HAStopCommand[] PROGMEM = {"STOP"};
const char HALockCommand[] PROGMEM = {"LOCK"};
const char HAUnlockCommand[] PROGMEM = {"UNLOCK"};

// device tracker
const char HAGPSType[] PROGMEM = {"gps"};
const char HARouterType[] PROGMEM = {"router"};
const char HABluetoothType[] PROGMEM = {"bluetooth"};
const char HABluetoothLEType[] PROGMEM = {"bluetooth_le"};

// camera
const char HAEncodingBase64[] PROGMEM = {"b64"};

// trigger
const char HAButtonShortPressType[] PROGMEM = {"button_short_press"};
const char HAButtonShortReleaseType[] PROGMEM = {"button_short_release"};
const char HAButtonLongPressType[] PROGMEM = {"button_long_press"};
const char HAButtonLongReleaseType[] PROGMEM = {"button_long_release"};
const char HAButtonDoublePressType[] PROGMEM = {"button_double_press"};
const char HAButtonTriplePressType[] PROGMEM = {"button_triple_press"};
const char HAButtonQuadruplePressType[] PROGMEM = {"button_quadruple_press"};
const char HAButtonQuintuplePressType[] PROGMEM = {"button_quintuple_press"};
const char HATurnOnSubtype[] PROGMEM = {"turn_on"};
const char HATurnOffSubtype[] PROGMEM = {"turn_off"};
const char HAButton1Subtype[] PROGMEM = {"button_1"};
const char HAButton2Subtype[] PROGMEM = {"button_2"};
const char HAButton3Subtype[] PROGMEM = {"button_3"};
const char HAButton4Subtype[] PROGMEM = {"button_4"};
const char HAButton5Subtype[] PROGMEM = {"button_5"};
const char HAButton6Subtype[] PROGMEM = {"button_6"};

// value templates
const char HAValueTemplateFloatP1[] PROGMEM = {"{{float(value)/10**1}}"};
const char HAValueTemplateFloatP2[] PROGMEM = {"{{float(value)/10**2}}"};
const char HAValueTemplateFloatP3[] PROGMEM = {"{{float(value)/10**3}}"};
const char HAValueTemplateFloatP4[] PROGMEM = {"{{float(value)/10**4}}"};
