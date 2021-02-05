#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

const char* BaseDeviceType::ConfigTopic = "config";
const char* BaseDeviceType::EventTopic = "event";
const char* BaseDeviceType::AvailabilityTopic = "avail";
const char* BaseDeviceType::Online = "online";
const char* BaseDeviceType::Offline = "offline";

BaseDeviceType::BaseDeviceType(
    HAMqtt& mqtt,
    const char* componentName,
    const char* name
) :
    _mqtt(mqtt),
    _componentName(componentName),
    _name(name),
    _availability(AvailabilityDefault)
{
    _mqtt.addDeviceType(this);
}

BaseDeviceType::~BaseDeviceType()
{

}

void BaseDeviceType::setAvailability(bool online)
{
    if (_availability == AvailabilityDefault) {
        return;
    }

    _availability = (online ? AvailabilityOnline : AvailabilityOffline);
    publishAvailability();
}

void BaseDeviceType::publishAvailability()
{
    if (_availability == AvailabilityDefault ||
            !_mqtt.isConnected() ||
            strlen(_name) == 0 ||
            strlen(_componentName) == 0) {
        return;
    }

    const uint16_t& topicSize = calculateTopicLength(
        _componentName,
        _name,
        AvailabilityTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        _componentName,
        _name,
        AvailabilityTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    mqtt()->publish(topic, (_availability == AvailabilityOnline ? Online : Offline), true);
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
    if (prefix == nullptr) {
        return 0;
    }

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
