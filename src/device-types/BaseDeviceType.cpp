#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

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

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        mqtt(),
        _componentName,
        _name,
        DeviceTypeSerializer::AvailabilityTopic
    );
    if (topicSize == 0) {
        return;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        mqtt(),
        topic,
        _componentName,
        _name,
        DeviceTypeSerializer::AvailabilityTopic
    );

    if (strlen(topic) == 0) {
        return;
    }

    mqtt()->publish(
        topic,
        (
            _availability == AvailabilityOnline ?
            DeviceTypeSerializer::Online :
            DeviceTypeSerializer::Offline
        ),
        true
    );
}
