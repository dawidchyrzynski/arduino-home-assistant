#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

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

void BaseDeviceType::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    (void)topic;
    (void)payload;
    (void)length;
}

void BaseDeviceType::publishConfig()
{
    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return;
    }

    const uint16_t& deviceLength = device->calculateSerializedLength();
    if (deviceLength == 0) {
        return;
    }

    char serializedDevice[deviceLength];
    if (device->serialize(serializedDevice) == 0) {
        return;
    }

    const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic,
        true,
        true
    );
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    DeviceTypeSerializer::generateTopic(
        topic,
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic,
        true
    );

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedData(serializedDevice);
        mqtt()->endPublish();
    }
}

void BaseDeviceType::publishAvailability()
{
    const HADevice* device = HAMqtt::instance()->getDevice();
    if (device == nullptr) {
        return;
    }

    if (_availability == AvailabilityDefault ||
            !mqtt()->isConnected() ||
            strlen(_name) == 0 ||
            strlen(_componentName) == 0 ||
            device->isSharedAvailabilityEnabled()) {
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

bool BaseDeviceType::isMyTopic(const char* topic, const char* expectedTopic)
{
    if (topic == nullptr || expectedTopic == nullptr) {
        return false;
    }

    if (strlen(name()) == 0) {
        return false;
    }

    static const char Slash[] PROGMEM = {"/"};

    // name + cmd topic + two slashes + null terminator
    uint8_t suffixLength = strlen(name()) + strlen(expectedTopic) + 3;
    char suffix[suffixLength];

    strcpy_P(suffix, Slash);
    strcat(suffix, name());
    strcat_P(suffix, Slash);
    strcat(suffix, expectedTopic);

    return HAUtils::endsWith(topic, suffix);
}
