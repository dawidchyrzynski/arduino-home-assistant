#include "HACamera.h"
#ifndef EX_ARDUINOHA_CAMERA

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HACamera::HACamera(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentCamera), uniqueId),
    _encoding(EncodingBinary),
    _icon(nullptr)
{

}

bool HACamera::publishImage(const uint8_t* data, const uint16_t length)
{
    if (!data) {
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HATopic), data, length, true);
}

void HACamera::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 8); // 8 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAObjectIdProperty), _objectId);
    _serializer->set(HASerializer::WithUniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(
        AHATOFSTR(HAEncodingProperty),
        getEncodingProperty(),
        HASerializer::ProgmemPropertyValue
    );
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HATopic));
}

void HACamera::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
}

const __FlashStringHelper* HACamera::getEncodingProperty() const
{
    switch (_encoding) {
    case EncodingBase64:
        return AHATOFSTR(HAEncodingBase64);

    default:
        return nullptr;
    }
}

#endif
