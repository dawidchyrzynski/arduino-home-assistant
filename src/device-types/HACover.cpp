#include "HACover.h"
#ifndef EX_ARDUINOHA_COVER

#include "../HAMqtt.h"
#include "../HAUtils.h"
#include "../utils/HASerializer.h"

HACover::HACover(const char* uniqueId) :
    HABaseDeviceType("cover", uniqueId),
    _commandCallback(nullptr),
    _currentState(StateUnknown),
    _currentPosition(DefaultPosition),
    _class(nullptr),
    _icon(nullptr),
    _retain(false)
{

}

bool HACover::setState(const CoverState state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HACover::setPosition(const int16_t position, const bool force)
{
    if (!force && _currentPosition == position) {
        return true;
    }

    if (publishPosition(position)) {
        _currentPosition = position;
        return true;
    }

    return false;
}


void HACover::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 10); // 10 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HADeviceClassProperty, _class);
    _serializer->set(HAIconProperty, _icon);

    // optional property
    if (_retain) {
        _serializer->set(HARetainProperty, &_retain, HASerializer::BoolPropertyType);
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
    _serializer->topic(HACommandTopic);
    _serializer->topic(HAPositionTopic);
}

void HACover::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishPosition(_currentPosition);
    }

    subscribeTopic(uniqueId(), HACommandTopic);
}

void HACover::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (_commandCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        HACommandTopic
    )) {
        char cmd[length + 1];
        memset(cmd, 0, sizeof(cmd));
        memcpy(cmd, payload, length);
        handleCommand(cmd);
    }
}

bool HACover::publishState(CoverState state)
{
    if (!uniqueId() || state == StateUnknown) {
        return false;
    }

    const char *stateP = nullptr;
    switch (state) {
    case StateClosed:
        stateP = HAClosedState;
        break;

    case StateClosing:
        stateP = HAClosingState;
        break;

    case StateOpen:
        stateP = HAOpenState;
        break;

    case StateOpening:
        stateP = HAOpeningState;
        break;

    case StateStopped:
        stateP = HAStoppedState;
        break;

    default:
        return false;
    }

    return publishOnDataTopic(HAStateTopic, stateP, true, true);
}

bool HACover::publishPosition(int16_t position)
{
    if (!uniqueId() || position == DefaultPosition) {
        return false;
    }

    uint8_t size = HAUtils::calculateNumberSize(position);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    memset(str, 0, sizeof(str));
    HAUtils::numberToStr(str, position);

    return publishOnDataTopic(HAPositionTopic, str, true);
}

void HACover::handleCommand(const char* cmd)
{
    if (!_commandCallback) {
        return;
    }

    if (strcmp_P(cmd, HACloseCommand) == 0) {
        _commandCallback(CommandClose, this);
    } else if (strcmp_P(cmd, HAOpenCommand) == 0) {
        _commandCallback(CommandOpen, this);
    } else if (strcmp_P(cmd, HAStopCommand) == 0) {
        _commandCallback(CommandStop, this);
    }
}

#endif
