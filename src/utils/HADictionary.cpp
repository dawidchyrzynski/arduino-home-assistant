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
