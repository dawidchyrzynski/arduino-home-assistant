#include "HALock.h"
#ifndef EX_ARDUINOHA_LOCK

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HALock::HALock(const char* uniqueId) :
    HABaseDeviceType("lock", uniqueId),
    _icon(nullptr),
    _retain(false),
    _currentState(StateUnknown),
    _commandCallback(nullptr)
{

}

bool HALock::setState(const LockState state, const bool force)
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

void HALock::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 8); // 8 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HAIconProperty, _icon);

    // optional property
    if (_retain) {
        _serializer->set(
            HARetainProperty,
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
    _serializer->topic(HACommandTopic);
}

void HALock::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
    }

    subscribeTopic(uniqueId(), HACommandTopic);
}

void HALock::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    (void)payload;
    (void)length;

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

bool HALock::publishState(const LockState state)
{
    if (state == StateUnknown) {
        return false;
    }

    return publishOnDataTopic(
        HAStateTopic,
        state == StateLocked ? HAStateLocked : HAStateUnlocked,
        true,
        true
    );
}

void HALock::handleCommand(const char* cmd)
{
    if (!_commandCallback) {
        return;
    }

    if (strcmp_P(cmd, HALockCommand) == 0) {
        _commandCallback(CommandLock, this);
    } else if (strcmp_P(cmd, HAUnlockCommand) == 0) {
        _commandCallback(CommandUnlock, this);
    } else if (strcmp_P(cmd, HAOpenCommand) == 0) {
        _commandCallback(CommandOpen, this);
    }
}

#endif
