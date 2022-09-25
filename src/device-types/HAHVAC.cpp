#include "HAHVAC.h"
#ifndef EX_ARDUINOHA_HVAC

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HAHVAC::HAHVAC(const char* uniqueId, const uint8_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentClimate), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false)
{

}

void HAHVAC::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 13); // 13 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
}

void HAHVAC::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    // to do
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    // to do
}

#endif
