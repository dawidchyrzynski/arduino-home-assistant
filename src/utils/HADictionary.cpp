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

// properties
const char HADeviceIdentifiersProperty[] PROGMEM = {"ids"};
const char HADeviceManufacturerProperty[] PROGMEM = {"mf"};
const char HADeviceModelProperty[] PROGMEM = {"mdl"};
const char HADeviceSoftwareVersionProperty[] PROGMEM = {"sw"};
const char HANameProperty[] PROGMEM = {"name"};
const char HAUniqueIdProperty[] PROGMEM = {"uniq_id"};
const char HADeviceProperty[] PROGMEM = {"dev"};
const char HADeviceClassProperty[] PROGMEM = {"dev_cla"};

// topics
const char HAAvailabilityTopic[] PROGMEM = {"avty_t"};
const char HAStateTopic[] PROGMEM = {"stat_t"};