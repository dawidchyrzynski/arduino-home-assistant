#include "HABaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"
#include "../utils/HASerializer.h"

HABaseDeviceType::HABaseDeviceType(
    const char* componentName,
    const char* uniqueId
) :
    _componentName(componentName),
    _uniqueId(uniqueId),
    _name(nullptr),
    _serializer(nullptr),
    _availability(AvailabilityDefault)
{
    if (mqtt()) {
        mqtt()->addDeviceType(this);
    }
}

HABaseDeviceType::~HABaseDeviceType()
{

}

void HABaseDeviceType::setAvailability(bool online)
{
    _availability = (online ? AvailabilityOnline : AvailabilityOffline);
    publishAvailability();
}

HAMqtt* HABaseDeviceType::mqtt() const
{
    return HAMqtt::instance();
}

void HABaseDeviceType::subscribeTopic(
    const char* uniqueId,
    const char* topicP
)
{
    const uint16_t topicLength = HASerializer::calculateDataTopicLength(
        uniqueId,
        topicP
    );
    if (topicLength == 0) {
        return;
    }

    char topic[topicLength];
    if (!HASerializer::generateDataTopic(
        topic,
        uniqueId,
        topicP
    )) {
        return;
    }

    HAMqtt::instance()->subscribe(topic);
}

void HABaseDeviceType::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    (void)topic;
    (void)payload;
    (void)length;
}

void HABaseDeviceType::destroySerializer()
{
    if (_serializer) {
        delete _serializer;
        _serializer = nullptr;
    }
}

void HABaseDeviceType::publishConfig()
{
    buildSerializer();

    const uint16_t topicLength = HASerializer::calculateConfigTopicLength(
        componentName(),
        uniqueId()
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

void HABaseDeviceType::publishAvailability()
{
    const HADevice* device = mqtt()->getDevice();
    if (
        !device ||
        device->isSharedAvailabilityEnabled() ||
        !isAvailabilityConfigured()
    ) {
        return;
    }

    publishOnDataTopic(
        HAAvailabilityTopic,
        _availability == AvailabilityOnline
            ? HAOnline
            : HAOffline,
        true,
        true
    );
}

bool HABaseDeviceType::publishOnDataTopic(
    const char* topicP,
    const char* value,
    bool retained,
    bool isProgmemValue
)
{
    if (!value) {
        return false;
    }

    const uint16_t topicLength = HASerializer::calculateDataTopicLength(
        uniqueId(),
        topicP
    );
    if (topicLength == 0) {
        return false;
    }

    char topic[topicLength];
    if (!HASerializer::generateDataTopic(
        topic,
        uniqueId(),
        topicP
    )) {
        return false;
    }

    const uint16_t valueLength = isProgmemValue
        ? strlen_P(value)
        : strlen(value);

    if (mqtt()->beginPublish(topic, valueLength, retained)) {
        if (isProgmemValue) {
            mqtt()->writePayload_P(value);
        } else {
            mqtt()->writePayload(value, valueLength);
        }

        return mqtt()->endPublish();
    }

    return false;
}
