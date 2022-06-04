#include "HABinarySensor.h"
#ifndef EX_ARDUINOHA_BINARY_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    bool initialState
) :
    BaseDeviceType("binary_sensor", uniqueId),
    _class(nullptr),
    _icon(nullptr),
    _currentState(initialState)
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

bool HABinarySensor::publishState(const bool state)
{
    return publishOnDataTopic(
        HAStateTopic,
        state ? HAStateOn : HAStateOff,
        true,
        true
    );
}

void HABinarySensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this);
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

#endif
