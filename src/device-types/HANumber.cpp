#include "HANumber.h"
#ifndef EX_ARDUINOHA_NUMBER

#define HANUMBER_PROCESS_FLOAT(value) HAUtils::processFloatValue(value, _precision)

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

const HAUtils::Number HANumber::StateNone = INT32_MAX;

HANumber::HANumber(const char* uniqueId, const NumberPrecision precision) :
    HABaseDeviceType(AHATOFSTR(HAComponentNumber), uniqueId),
    _precision(precision),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _mode(ModeAuto),
    _unitOfMeasurement(nullptr),
    _minValue(HANUMBER_PROCESS_FLOAT(1)),
    _maxValue(HANUMBER_PROCESS_FLOAT(100)),
    _step(HANUMBER_PROCESS_FLOAT(1)),
    _currentState(StateNone),
    _commandCallback(nullptr)
{

}

bool HANumber::setState(const float state, const bool force)
{
    const HAUtils::Number realState = HANUMBER_PROCESS_FLOAT(state);
    if (!force && realState == _currentState) {
        return true;
    }

    if (publishState(realState)) {
        _currentState = realState;
        return true;
    }

    return false;
}

void HANumber::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    const HASerializer::PropertyValueType numberProperty =
        HASerializer::precisionToPropertyType(_precision);

    _serializer = new HASerializer(this, 15); // 15 - max properties nb

    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAUnitOfMeasurementProperty), _unitOfMeasurement);
    _serializer->set(
        AHATOFSTR(HAModeProperty),
        getModeProperty(),
        HASerializer::ProgmemPropertyValue
    );

    if (_minValue != HANUMBER_PROCESS_FLOAT(1)) {
        _serializer->set(AHATOFSTR(HAMinProperty), &_minValue, numberProperty);
    }

    if (_maxValue != HANUMBER_PROCESS_FLOAT(100)) {
        _serializer->set(AHATOFSTR(HAMaxProperty), &_maxValue, numberProperty);
    }

    if (_step != HANUMBER_PROCESS_FLOAT(1)) {
        _serializer->set(AHATOFSTR(HAStepProperty), &_step, numberProperty);
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

bool HANumber::publishState(const HAUtils::Number state)
{
    if (state == StateNone) {
        return publishOnDataTopic(
            AHATOFSTR(HAStateTopic),
            AHATOFSTR(HAStateNode),
            true
        );
    }

    uint8_t size = HAUtils::calculateNumberSize(state, _precision);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    memset(str, 0, sizeof(str));
    HAUtils::numberToStr(str, state, _precision);

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        str,
        true
    );
}

void HANumber::handleCommand(const char* cmd)
{
    if (!_commandCallback) {
        return;
    }

    // to do
}

const __FlashStringHelper* HANumber::getModeProperty() const
{
    switch (_mode) {
    case ModeBox:
        return AHATOFSTR(HAModeBox);

    case ModeSlider:
        return AHATOFSTR(HAModeSlider);

    default:
        return nullptr;
    }
}

#endif
