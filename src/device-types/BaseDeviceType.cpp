#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

const char* BaseDeviceType::ConfigTopic = "config"; // todo: move to progmem
const char* BaseDeviceType::EventTopic = "event";

BaseDeviceType::BaseDeviceType(HAMqtt& mqtt) :
    _mqtt(mqtt)
{
    _mqtt.addDeviceType(this);
}

BaseDeviceType::~BaseDeviceType()
{
    _mqtt.removeDeviceType(this);
}

uint16_t BaseDeviceType::calculateTopicLength(
    const char* component,
    const char* objectId,
    const char* suffix,
    bool includeNullTerminator
) const
{
    // [discovery prefix]/[namespace]/[device id - optional]/subtype_type/[suffix]
    const char* prefix = _mqtt.getDiscoveryPrefix();
    uint16_t size =
        strlen(prefix) + 1 + // with slash
        strlen(component) + 1 + // with slash
        strlen(suffix); // with null terminator

    if (objectId != nullptr) {
        size += strlen(objectId) + 1; // with slash
    } else {
        size += 1; // slash
    }

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
    const char* component,
    const char* objectId,
    const char* suffix
) const
{
    static const char Slash[] PROGMEM = {"/"};

    const char* prefix = _mqtt.getDiscoveryPrefix();
    strcpy(output, prefix);
    strcat_P(output, Slash);
    strcat(output, component);
    strcat_P(output, Slash);

    if (_mqtt.getDevice() != nullptr) {
        strcat(output, _mqtt.getDevice()->getUniqueId());
        strcat_P(output, Slash);
    }

    strcat(output, objectId);
    strcat_P(output, Slash);
    strcat(output, suffix);
    return strlen(output) + 1; // size with null terminator
}
