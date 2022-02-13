#include "HABinarySensor.h"
#ifdef ARDUINOHA_BINARY_SENSOR

#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../utils/HASerializer.h"

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    bool initialState
) :
    BaseDeviceType("binary_sensor", uniqueId),
    _class(nullptr),
    _currentState(initialState)
{

}

void HABinarySensor::buildSerializer()
{
    if (_serializer) {
        return;
    }

    _serializer = new HASerializer(this);
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HADeviceClassProperty, _class);
    _serializer->set(HASerializer::WithDevice);
    //_serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
}

void HABinarySensor::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
        return;
    }

    publishConfig();
    publishState(_currentState);
    publishAvailability();
}

bool HABinarySensor::setState(bool state)
{
    if (state == _currentState) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HABinarySensor::publishState(bool state)
{
    /* if (strlen(uniqueId()) == 0) {
        return false;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        uniqueId(),
        DeviceTypeSerializer::StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        topic,
        componentName(),
        uniqueId(),
        DeviceTypeSerializer::StateTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(
        topic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        ),
        true
    ); */

    return false;
}

#endif
