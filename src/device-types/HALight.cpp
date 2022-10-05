#include "HALight.h"
#ifndef EX_ARDUINOHA_LIGHT

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

const uint8_t HALight::DefaultBrightnessScale = 255;
const uint16_t HALight::DefaultMinMireds = 153;
const uint16_t HALight::DefaultMaxMireds = 500;

HALight::HALight(const char* uniqueId, const uint8_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentLight), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _brightnessScale(DefaultBrightnessScale),
    _currentState(false),
    _currentBrightness(0),
    _minMireds(DefaultMinMireds),
    _maxMireds(DefaultMaxMireds),
    _currentColorTemperature(0),
    _stateCallback(nullptr),
    _brightnessCallback(nullptr),
    _colorTemperatureCallback(nullptr)
{

}

bool HALight::setState(const bool state, const bool force)
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

bool HALight::setBrightness(const uint8_t brightness, const bool force)
{
    if (!force && brightness == _currentBrightness) {
        return true;
    }

    if (publishBrightness(brightness)) {
        _currentBrightness = brightness;
        return true;
    }

    return false;
}

bool HALight::setColorTemperature(const uint16_t temperature, const bool force)
{
    if (!force && temperature == _currentColorTemperature) {
        return true;
    }

    if (publishColorTemperature(temperature)) {
        _currentColorTemperature = temperature;
        return true;
    }

    return false;
}

void HALight::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 16); // 16 - max properties nb
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

    if (_features & BrightnessFeature) {
        _serializer->topic(AHATOFSTR(HABrightnessStateTopic));
        _serializer->topic(AHATOFSTR(HABrightnessCommandTopic));

        if (_brightnessScale != DefaultBrightnessScale) {
            _serializer->set(
                AHATOFSTR(HABrightnessScaleProperty),
                &_brightnessScale,
                HASerializer::NumberP0PropertyType
            );
        }
    }

    if (_features & ColorTemperatureFeature) {
        _serializer->topic(AHATOFSTR(HAColorTemperatureStateTopic));
        _serializer->topic(AHATOFSTR(HAColorTemperatureCommandTopic));

        if (_minMireds != DefaultMinMireds) {
            _serializer->set(
                AHATOFSTR(HAMinMiredsProperty),
                &_minMireds,
                HASerializer::NumberP0PropertyType
            );
        }

        if (_maxMireds != DefaultMaxMireds) {
            _serializer->set(
                AHATOFSTR(HAMaxMiredsProperty),
                &_maxMireds,
                HASerializer::NumberP0PropertyType
            );
        }
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HALight::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishBrightness(_currentBrightness);
        publishColorTemperature(_currentColorTemperature);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (_features & BrightnessFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HABrightnessCommandTopic));
    }

    if (_features & ColorTemperatureFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAColorTemperatureCommandTopic));
    }
}

void HALight::onMqttMessage(
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
        AHATOFSTR(HABrightnessCommandTopic)
    )) {
        handleBrightnessCommand(payload, length);
    }  else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAColorTemperatureCommandTopic)
    )) {
        handleColorTemperatureCommand(payload, length);
    }
}

bool HALight::publishState(const bool state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HALight::publishBrightness(const uint8_t brightness)
{
    if (!uniqueId() || !(_features & BrightnessFeature)) {
        return false;
    }

    char str[3 + 1] = {0}; // uint8_t digits with null terminator
    HAUtils::numberToStr(str, brightness);

    return publishOnDataTopic(AHATOFSTR(HABrightnessStateTopic), str, true);
}

bool HALight::publishColorTemperature(const uint16_t temperature)
{
    if (
        !uniqueId() ||
        !(_features & ColorTemperatureFeature) ||
        temperature < _minMireds ||
        temperature > _maxMireds
    ) {
        return false;
    }

    char str[5 + 1] = {0}; // uint16_t digits with null terminator
    HAUtils::numberToStr(str, temperature);

    return publishOnDataTopic(AHATOFSTR(HAColorTemperatureStateTopic), str, true);
}

void HALight::handleStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_stateCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _stateCallback(state, this);
}

void HALight::handleBrightnessCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_brightnessCallback) {
        return;
    }

    HAUtils::Number number = HAUtils::strToNumber(cmd, length);
    if (
        number != HAUtils::NumberMax &&
        number >= 0 &&
        number <= _brightnessScale
    ) {
        _brightnessCallback(static_cast<uint8_t>(number), this);
    }
}

void HALight::handleColorTemperatureCommand(
    const uint8_t* cmd,
    const uint16_t length
)
{
    if (!_colorTemperatureCallback) {
        return;
    }

    HAUtils::Number number = HAUtils::strToNumber(cmd, length);
    if (
        number != HAUtils::NumberMax &&
        number >= _minMireds &&
        number <= _maxMireds
    ) {
        _colorTemperatureCallback(static_cast<uint16_t>(number), this);
    }
}

#endif
