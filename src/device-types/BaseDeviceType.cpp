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
    _name(nullptr),
    _serializer(nullptr),
    _availability(AvailabilityDefault)
{
    if (mqtt()) {
        mqtt()->addDeviceType(this);
    }
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
    const HADevice* device = mqtt()->getDevice();
    if (
        !device ||
        !uniqueId() ||
        device->isSharedAvailabilityEnabled()
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

bool BaseDeviceType::publishOnDataTopic(
    const char* topicP,
    const char* value,
    bool retained,
    bool isProgmemValue
)
{
    if (!uniqueId() || !topicP || !value) {
        return false;
    }

    const uint16_t topicLength = HASerializer::calculateDataTopicLength(
        uniqueId(),
        topicP,
        true
    );
    if (topicLength == 0) {
        return false;
    }

    char topic[topicLength];
    const uint16_t generatedTopicLength = HASerializer::generateDataTopic(
        topic,
        uniqueId(),
        topicP
    );
    if (generatedTopicLength == 0) {
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
