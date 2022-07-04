#include "HACamera.h"
#ifndef EX_ARDUINOHA_CAMERA

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HACamera::HACamera(const char* uniqueId) :
    HABaseDeviceType("camera", uniqueId),
    _encoding(EncodingBinary),
    _icon(nullptr)
{

}

bool HACamera::publishImage(const char* data)
{
    if (!data) {
        return false;
    }

    return publishOnDataTopic(HATopic, data, true);
}

void HACamera::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 7); // 7 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HAIconProperty, _icon);
    _serializer->set(
        HAEncodingProperty,
        getEncodingProperty(),
        HASerializer::ProgmemPropertyValue
    );
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HATopic);
}

void HACamera::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
}

const char* HACamera::getEncodingProperty() const
{
    switch (_encoding) {
    case EncodingBase64:
        return HAEncodingBase64;

    default:
        return nullptr;
    }
}

#endif
