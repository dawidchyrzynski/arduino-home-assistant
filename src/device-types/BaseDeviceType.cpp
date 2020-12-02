#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

const char* BaseDeviceType::ConfigTopic = "config";
const char* BaseDeviceType::EventTopic = "event";

BaseDeviceType::BaseDeviceType(const char* uniqueId, HAMqtt& mqtt) :
    _uniqueId(uniqueId),
    _mqtt(mqtt)
{
    _mqtt.addDeviceType(this);
}

BaseDeviceType::~BaseDeviceType()
{
    _mqtt.removeDeviceType(this);
}

uint16_t BaseDeviceType::calculateTopicLength(
    const char* haNamespace,
    const char* type,
    const char* subtype,
    const char* suffix,
    bool includeNullTerminator
) const
{
    // [discovery prefix]/[namespace]/[device id - optional]/subtype_type/[suffix]
    const char* prefix = _mqtt.getDiscoveryPrefix();
    uint16_t size =
        strlen(prefix) + 1 + // with slash
        strlen(haNamespace) + 1 + // with slash
        strlen(type) + 1 + // with underscore
        strlen(subtype) + 1 + // with slash
        strlen(suffix); // with null terminator

    if (_mqtt.getDevice() != nullptr) {
        size += strlen(_mqtt.getDevice()->getUniqueId()) + 1; // with slash
    }

    if (includeNullTerminator) {
        size += 1;
    }

    return size;
}

uint16_t BaseDeviceType::generateTopic(
    char* output,
    const char* haNamespace,
    const char* type,
    const char* subtype,
    const char* suffix
) const
{
    // prevent from wasting RAM
    static const char Slash[] PROGMEM = {"/"};
    static const char Underscore[] PROGMEM = {"_"};

    const char* prefix = _mqtt.getDiscoveryPrefix();
    strcpy(output, prefix);
    strcat_P(output, Slash);
    strcat(output, haNamespace);
    strcat_P(output, Slash);

    if (_mqtt.getDevice() != nullptr) {
        strcat(output, _mqtt.getDevice()->getUniqueId());
        strcat_P(output, Slash);
    }

    strcat(output, subtype);
    strcat_P(output, Underscore);
    strcat(output, type);
    strcat_P(output, Slash);
    strcat(output, suffix);
    return strlen(output) + 1; // size with null terminator
}
