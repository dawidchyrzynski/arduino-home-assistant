#include "HABinarySensor.h"
#ifndef EX_ARDUINOHA_BINARY_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HABinarySensor::HABinarySensor(const char* uniqueId) :
    HABaseDeviceType("binary_sensor", uniqueId),
    _class(nullptr),
    _icon(nullptr),
    _currentState(false)
{

}

bool HABinarySensor::setState(const bool state, const bool force)
{
    if (!force && state == _currentState) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HABinarySensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 7); // 7 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HADeviceClassProperty, _class);
    _serializer->set(HAIconProperty, _icon);
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
}

void HABinarySensor::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
    publishState(_currentState);
}

bool HABinarySensor::publishState(const bool state)
{
    return publishOnDataTopic(
        HAStateTopic,
        state ? HAStateOn : HAStateOff,
        true,
        true
    );
}

#endif
