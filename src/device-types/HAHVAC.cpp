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
    _currentTemperature(HAUtils::NumberMax),
    _action(UnknownAction),
    _temperatureUnit(DefaultUnit)
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

bool HAHVAC::setAction(const Action action, const bool force)
{
    if (!force && action == _action) {
        return true;
    }

    if (publishAction(action)) {
        _action = action;
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

    if (_features & ActionFeature) {
        _serializer->topic(AHATOFSTR(HAActionTopic));
    }

    if (_temperatureUnit != DefaultUnit) {
        const __FlashStringHelper *unitStr = _temperatureUnit == CelsiusUnit
            ? AHATOFSTR(HATemperatureUnitC)
            : AHATOFSTR(HATemperatureUnitF);

        _serializer->set(
            AHATOFSTR(HATemperatureUnitProperty),
            unitStr,
            HASerializer::ProgmemPropertyValue
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
        publishAction(_action);
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

bool HAHVAC::publishAction(const Action action)
{
    if (action == UnknownAction || !(_features & ActionFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (action) {
    case OffAction:
        stateStr = AHATOFSTR(HAActionOff);
        break;

    case HeatingAction:
        stateStr = AHATOFSTR(HAActionHeating);
        break;

    case CoolingAction:
        stateStr = AHATOFSTR(HAActionCooling);
        break;

    case DryingAction:
        stateStr = AHATOFSTR(HAActionDrying);
        break;

    case IdleAction:
        stateStr = AHATOFSTR(HAActionIdle);
        break;

    case FanAction:
        stateStr = AHATOFSTR(HAActionFan);
        break;   

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAActionTopic),
        stateStr,
        true
    );
}

#endif
