#include "HANumber.h"
#ifndef EX_ARDUINOHA_NUMBER

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

const float HANumber::StateNone = 0;

HANumber::HANumber(const char* uniqueId, const Precision precision) :
    HABaseDeviceType(AHATOFSTR(HAComponentNumber), uniqueId),
    _precision(precision),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _mode(ModeAuto),
    _unitOfMeasurement(nullptr),
    _minValue(1),
    _maxValue(100),
    _step(1),
    _currentState(StateNone),
    _commandCallback(nullptr)
{

}

bool HANumber::setState(const float state, const bool force)
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

void HANumber::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 11); // 11 - max properties nb

    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAUnitOfMeasurementProperty), _unitOfMeasurement);

    if (_minValue != 1) {
        // to do
    }

    if (_maxValue != 100) {
        // to do
    }

    if (_step != 1) {
        // to do
    }

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HANumber::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HANumber::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    (void)length;

    if (_commandCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        handleCommand(reinterpret_cast<const char*>(payload));
    }
}

bool HANumber::publishState(const float state)
{
    /* if (state == StateNone) {
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state == StateLocked ? HAStateLocked : HAStateUnlocked),
        true
    ); */

    return false;
}

void HANumber::handleCommand(const char* cmd)
{
    if (!_commandCallback) {
        return;
    }

    // to do
}

#endif
