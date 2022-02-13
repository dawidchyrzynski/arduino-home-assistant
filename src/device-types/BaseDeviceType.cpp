#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"
#include "../utils/HASerializer.h"

BaseDeviceType::BaseDeviceType(
    const char* componentName,
    const char* uniqueId
) :
    _componentName(componentName),
    _uniqueId(uniqueId),
    _serializer(nullptr),
    _name(nullptr),
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

void BaseDeviceType::destroySerializer()
{
    if (_serializer) {
        delete _serializer;
        _serializer = nullptr;
    }
}

void BaseDeviceType::publishConfig()
{
    buildSerializer();

    const uint16_t topicLength = HASerializer::calculateConfigTopicLength(
        componentName(),
        uniqueId(),
        true
    );
    const uint16_t dataLength = _serializer->calculateSize();

    if (topicLength == 0 || dataLength == 0) {
        destroySerializer();
        return;
    }

    char topic[topicLength];
    HASerializer::generateConfigTopic(
        topic,
        componentName(),
        uniqueId()
    );

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        _serializer->flush();
        mqtt()->endPublish();
    }

    destroySerializer();
}

void BaseDeviceType::publishAvailability()
{
    /* const HADevice* device = HAMqtt::instance()->getDevice();
    if (device == nullptr) {
        return;
    }

    if (_availability == AvailabilityDefault ||
            !mqtt()->isConnected() ||
            strlen(uniqueId()) == 0 ||
            strlen(componentName()) == 0 ||
            device->isSharedAvailabilityEnabled()) {
        return;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        uniqueId(),
        DeviceTypeSerializer::AvailabilityTopic
    );
    if (topicSize == 0) {
        return;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        topic,
        componentName(),
        uniqueId(),
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
    ); */
}

bool BaseDeviceType::compareTopics(const char* topic, const char* expectedTopic)
{
    if (!topic || !expectedTopic) {
        return false;
    }

    if (strlen(uniqueId()) == 0) {
        return false;
    }

    static const char Slash[] PROGMEM = {"/"};

    // unique ID + cmd topic + two slashes + null terminator
    uint8_t suffixLength = strlen(uniqueId()) + strlen(expectedTopic) + 3;
    char suffix[suffixLength];

    strcpy_P(suffix, Slash);
    strcat(suffix, uniqueId());
    strcat_P(suffix, Slash);
    strcat(suffix, expectedTopic);

    return HAUtils::endsWith(topic, suffix);
}
