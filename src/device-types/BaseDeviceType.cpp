#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

BaseDeviceType::BaseDeviceType(
    const char* componentName,
    const char* name
) :
    _componentName(componentName),
    _name(name),
    _availability(AvailabilityDefault)
{
    mqtt()->addDeviceType(this);
}

BaseDeviceType::~BaseDeviceType()
{

}

void BaseDeviceType::setAvailability(bool online)
{
    _availability = (online ? AvailabilityOnline : AvailabilityOffline);
    publishAvailability();
}

HAMqtt* BaseDeviceType::mqtt() const
{
    return HAMqtt::instance();
}

void BaseDeviceType::publishAvailability()
{
    if (_availability == AvailabilityDefault ||
            !mqtt()->isConnected() ||
            strlen(_name) == 0 ||
            strlen(_componentName) == 0) {
        return;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        _componentName,
        _name,
        DeviceTypeSerializer::AvailabilityTopic
    );
    if (topicSize == 0) {
        return;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
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
