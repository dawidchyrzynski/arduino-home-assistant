#include "HAFan.h"
#ifndef EX_ARDUINOHA_FAN

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HAFan::HAFan(const char* uniqueId, const Features features) :
    HABaseDeviceType(AHATOFSTR(HAComponentFan), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _speedRangeMax(100),
    _speedRangeMin(1),
    _currentState(false),
    _currentSpeedPercentage(0),
    _stateCallback(nullptr),
    _speedCallback(nullptr)
{

}

bool HAFan::setState(const bool state, const bool force)
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

bool HAFan::setSpeed(const uint8_t speedPercentage, const bool force)
{
    if (!force && speedPercentage == _currentSpeedPercentage) {
        return true;
    }

    if (publishSpeed(speedPercentage)) {
        _currentSpeedPercentage = speedPercentage;
        return true;
    }

    return false;
}

void HAFan::buildSerializer()
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

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    if (_features & SpeedsFeature) {
        _serializer->topic(AHATOFSTR(HAPercentageStateTopic));
        _serializer->topic(AHATOFSTR(HAPercentageCommandTopic));

        if (_speedRangeMax != 100) {
            _serializer->set(
                AHATOFSTR(HASpeedRangeMaxProperty),
                &_speedRangeMax,
                HASerializer::NumberP0PropertyType
            );
        }

        if (_speedRangeMin != 1) {
            _serializer->set(
                AHATOFSTR(HASpeedRangeMinProperty),
                &_speedRangeMin,
                HASerializer::NumberP0PropertyType
            );
        }
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAFan::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishSpeed(_currentSpeedPercentage);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (_features & SpeedsFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAPercentageCommandTopic));
    }
}

void HAFan::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        handleStateCommand(reinterpret_cast<const char*>(payload), length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAPercentageCommandTopic)
    )) {
        handleSpeedCommand(reinterpret_cast<const char*>(payload), length);
    }
}

bool HAFan::publishState(const bool state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HAFan::publishSpeed(const uint8_t speedPercentage)
{
    if (!uniqueId() || !(_features & SpeedsFeature)) {
        return false;
    }

    uint8_t size = HAUtils::calculateNumberSize(speedPercentage);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    HAUtils::numberToStr(str, speedPercentage);

    return publishOnDataTopic(AHATOFSTR(HAPercentageStateTopic), str, true);
}

void HAFan::handleStateCommand(const char* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_stateCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _stateCallback(state, this);
}

void HAFan::handleSpeedCommand(const char* cmd, const uint16_t length)
{
    if (!_speedCallback) {
        return;
    }

    HAUtils::Number number = HAUtils::strToNumber(cmd);
    if (number != HAUtils::NumberMax && number >= 0 && number <= 255) {
        _speedCallback(static_cast<uint8_t>(number), this);
    }
}

#endif
