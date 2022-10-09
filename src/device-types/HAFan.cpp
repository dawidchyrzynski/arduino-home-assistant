#include "HAFan.h"
#ifndef EX_ARDUINOHA_FAN

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

const uint8_t HAFan::DefaultSpeedRangeMin = 1;
const uint8_t HAFan::DefaultSpeedRangeMax = 100;

HAFan::HAFan(const char* uniqueId, const uint8_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentFan), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _speedRangeMax(DefaultSpeedRangeMax),
    _speedRangeMin(DefaultSpeedRangeMin),
    _currentState(false),
    _currentSpeed(0),
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

bool HAFan::setSpeed(const uint16_t speed, const bool force)
{
    if (!force && speed == _currentSpeed) {
        return true;
    }

    if (publishSpeed(speed)) {
        _currentSpeed = speed;
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

        if (_speedRangeMax != DefaultSpeedRangeMax) {
            _serializer->set(
                AHATOFSTR(HASpeedRangeMaxProperty),
                &_speedRangeMax,
                HASerializer::NumberP0PropertyType
            );
        }

        if (_speedRangeMin != DefaultSpeedRangeMin) {
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
        publishSpeed(_currentSpeed);
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
        handleStateCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAPercentageCommandTopic)
    )) {
        handleSpeedCommand(payload, length);
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

bool HAFan::publishSpeed(const uint16_t speed)
{
    if (!(_features & SpeedsFeature)) {
        return false;
    }

    char str[5 + 1] = {0}; // uint16_t digits with null terminator
    HAUtils::numberToStr(str, speed);

    return publishOnDataTopic(AHATOFSTR(HAPercentageStateTopic), str, true);
}

void HAFan::handleStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_stateCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _stateCallback(state, this);
}

void HAFan::handleSpeedCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_speedCallback) {
        return;
    }

    HAUtils::Number number = HAUtils::strToNumber(cmd, length);
    if (
        number != HAUtils::NumberMax &&
        number >= _speedRangeMin &&
        number <= _speedRangeMax
    ) {
        _speedCallback(static_cast<uint16_t>(number), this);
    }
}

#endif
