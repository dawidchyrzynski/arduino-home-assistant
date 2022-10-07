#include "HALight.h"
#ifndef EX_ARDUINOHA_LIGHT

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

const uint8_t HALight::DefaultBrightnessScale = 255;
const uint16_t HALight::DefaultMinMireds = 153;
const uint16_t HALight::DefaultMaxMireds = 500;
const uint8_t HALight::RGBStringMaxLength = 3*4; // 4 characters per color

void HALight::RGBColor::fromBuffer(const uint8_t* data, const uint16_t length)
{
    if (length > RGBStringMaxLength) {
        return;
    }

    uint8_t firstCommaPos = 0;
    uint8_t secondCommaPos = 0;

    for (uint8_t i = 0; i < length; i++) {
        if (data[i] == ',') {
            if (firstCommaPos == 0) {
                firstCommaPos = i;
            } else if (secondCommaPos == 0) {
                secondCommaPos = i;
            }
        }
    }

    if (firstCommaPos == 0 || secondCommaPos == 0) {
        return;
    }

    const uint8_t redLen = firstCommaPos;
    const uint8_t greenLen = secondCommaPos - firstCommaPos - 1; // minus comma
    const uint8_t blueLen = length - redLen - greenLen - 2; // minus two commas

    HAUtils::Number r = HAUtils::strToNumber(data, redLen);
    HAUtils::Number g = HAUtils::strToNumber(
        &data[redLen + 1],
        greenLen
    );
    HAUtils::Number b = HAUtils::strToNumber(
        &data[redLen + greenLen + 2],
        blueLen
    );

    if (
        r >= 0 && r <= 255 &&
        g >= 0 && g <= 255 &&
        b >= 0 && b <= 255
    ) {
        red = static_cast<uint8_t>(r);
        green = static_cast<uint8_t>(g);
        blue = static_cast<uint8_t>(b);
        isSet = true;
    }
}

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
    _currentRGBColor(),
    _stateCallback(nullptr),
    _brightnessCallback(nullptr),
    _colorTemperatureCallback(nullptr),
    _rgbColorCallback(nullptr)
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

bool HALight::setRGBColor(const RGBColor& color, const bool force)
{
    if (!force && color == _currentRGBColor) {
        return true;
    }

    if (publishRGBColor(color)) {
        _currentRGBColor = color;
        return true;
    }

    return false;
}

void HALight::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 18); // 18 - max properties nb
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

    if (_features & RGBFeature) {
        _serializer->topic(AHATOFSTR(HARGBCommandTopic));
        _serializer->topic(AHATOFSTR(HARGBStateTopic));
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
        publishRGBColor(_currentRGBColor);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (_features & BrightnessFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HABrightnessCommandTopic));
    }

    if (_features & ColorTemperatureFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAColorTemperatureCommandTopic));
    }

    if (_features & RGBFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HARGBCommandTopic));
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
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAColorTemperatureCommandTopic)
    )) {
        handleColorTemperatureCommand(payload, length);
    } else if (
        HASerializer::compareDataTopics(
            topic,
            uniqueId(),
            AHATOFSTR(HARGBCommandTopic)
        )
    ) {
        handleRGBCommand(payload, length);
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
    if (!(_features & BrightnessFeature)) {
        return false;
    }

    char str[3 + 1] = {0}; // uint8_t digits with null terminator
    HAUtils::numberToStr(str, brightness);

    return publishOnDataTopic(AHATOFSTR(HABrightnessStateTopic), str, true);
}

bool HALight::publishColorTemperature(const uint16_t temperature)
{
    if (
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

bool HALight::publishRGBColor(const RGBColor& color)
{
    if (!(_features & RGBFeature) || !color.isSet) {
        return false;
    }

    char str[RGBStringMaxLength] = {0};
    uint16_t len = 0;

    // append red color with comma
    len += HAUtils::numberToStr(&str[0], color.red);
    str[len++] = ',';

    // append green color with comma
    len += HAUtils::numberToStr(&str[len], color.green);
    str[len++] = ',';

    // append blue color
    HAUtils::numberToStr(&str[len], color.blue);

    return publishOnDataTopic(AHATOFSTR(HARGBStateTopic), str, true);
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

void HALight::handleRGBCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_rgbColorCallback) {
        return;
    }

    RGBColor color;
    color.fromBuffer(cmd, length);

    if (color.isSet) {
        _rgbColorCallback(color, this);
    }
}

#endif
