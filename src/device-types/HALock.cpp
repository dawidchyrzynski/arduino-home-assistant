#include "HALock.h"
#ifndef EX_ARDUINOHA_LOCK

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HALock::HALock(const char* uniqueId) :
    HABaseDeviceType("lock", uniqueId),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
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

    _serializer = new HASerializer(this, 9); // 9 - max properties nb
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

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HALock::onMqttMessage(
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

bool HALock::publishState(const LockState state)
{
    if (state == StateUnknown) {
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
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
