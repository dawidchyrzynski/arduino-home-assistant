#include "HAHVAC.h"
#ifndef EX_ARDUINOHA_HVAC

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HAHVAC::HAHVAC(
    const char* uniqueId,
    const uint8_t features,
    const NumberPrecision precision
) :
    HABaseDeviceType(AHATOFSTR(HAComponentClimate), uniqueId),
    _features(features),
    _precision(precision),
    _icon(nullptr),
    _retain(false),
    _currentTemperature(HAUtils::NumberMax)
{

}

bool HAHVAC::setCurrentTemperature(const float temperature, const bool force)
{
    const HAUtils::Number realTemperature = HAUtils::processFloatValue(temperature, _precision);
    if (!force && realTemperature == _currentTemperature) {
        return true;
    }

    if (publishCurrentTemperature(realTemperature)) {
        _currentTemperature = realTemperature;
        return true;
    }

    return false;
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

    _serializer->topic(AHATOFSTR(HACurrentTemperatureTopic));
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

    if (!_retain) {
        publishCurrentTemperature(_currentTemperature);
    }
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    // to do
}

bool HAHVAC::publishCurrentTemperature(const HAUtils::Number temperature)
{
    if (temperature == HAUtils::NumberMax) {
        return false;
    }

    uint8_t size = HAUtils::calculateNumberSize(temperature, _precision);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    HAUtils::numberToStr(str, temperature, _precision);

    return publishOnDataTopic(
        AHATOFSTR(HACurrentTemperatureTopic),
        str,
        true
    );
}

#endif
