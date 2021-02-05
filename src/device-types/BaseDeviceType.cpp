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
    DeviceTypeSerializer(this),
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
