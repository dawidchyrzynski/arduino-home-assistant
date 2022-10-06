#include "HAHVAC.h"
#ifndef EX_ARDUINOHA_HVAC

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

const uint8_t HAHVAC::DefaultFanModes = AutoFanMode | LowFanMode | MediumFanMode | HighFanMode;

HAHVAC::HAHVAC(
    const char* uniqueId,
    const uint16_t features,
    const NumberPrecision precision
) :
    HABaseDeviceType(AHATOFSTR(HAComponentClimate), uniqueId),
    _features(features),
    _precision(precision),
    _icon(nullptr),
    _retain(false),
    _currentTemperature(HAUtils::NumberMax),
    _action(UnknownAction),
    _temperatureUnit(DefaultUnit),
    _minTemp(HAUtils::NumberMax),
    _maxTemp(HAUtils::NumberMax),
    _tempStep(HAUtils::NumberMax),
    _auxCallback(nullptr),
    _auxState(false),
    _powerCallback(nullptr),
    _fanMode(UnknownFanMode),
    _fanModes(DefaultFanModes),
    _fanModesSerializer(nullptr),
    _fanModeCallback(nullptr)
{
    if (_features & FanFeature) {
        _fanModesSerializer = new HASerializerArray(4);
    }
}

bool HAHVAC::setCurrentTemperature(const float temperature, const bool force)
{
    const HAUtils::Number realTemperature = HAUtils::processFloatValue(
        temperature,
        _precision
    );

    if (!force && realTemperature == _currentTemperature) {
        return true;
    }

    if (publishCurrentTemperature(realTemperature)) {
        _currentTemperature = realTemperature;
        return true;
    }

    return false;
}

bool HAHVAC::setAction(const Action action, const bool force)
{
    if (!force && action == _action) {
        return true;
    }

    if (publishAction(action)) {
        _action = action;
        return true;
    }

    return false;
}

bool HAHVAC::setAuxState(const bool state, const bool force)
{
    if (!force && state == _auxState) {
        return true;
    }

    if (publishAuxState(state)) {
        _auxState = state;
        return true;
    }

    return false;
}

bool HAHVAC::setFanMode(const FanMode mode, const bool force)
{
    if (!force && mode == _fanMode) {
        return true;
    }

    if (publishFanMode(mode)) {
        _fanMode = mode;
        return true;
    }

    return false;
}

void HAHVAC::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    const HASerializer::PropertyValueType numberProperty =
        HASerializer::precisionToPropertyType(_precision);

    _serializer = new HASerializer(this, 17); // 17 - max properties nb
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

    if (_features & ActionFeature) {
        _serializer->topic(AHATOFSTR(HAActionTopic));
    }

    if (_features & AuxHeatingFeature) {
        _serializer->topic(AHATOFSTR(HAAuxCommandTopic));
        _serializer->topic(AHATOFSTR(HAAuxStateTopic));
    }

    if (_features & PowerFeature) {
        _serializer->topic(AHATOFSTR(HAPowerCommandTopic));
    }

    if (_features & FanFeature) {
        _serializer->topic(AHATOFSTR(HAFanModeCommandTopic));
        _serializer->topic(AHATOFSTR(HAFanModeStateTopic));

        if (_fanModes != DefaultFanModes) {
            _fanModesSerializer->clear();

            if (_fanModes & AutoFanMode) {
                _fanModesSerializer->add(HAFanModeAuto);
            }

            if (_fanModes & LowFanMode) {
                _fanModesSerializer->add(HAFanModeLow);
            }

            if (_fanModes & MediumFanMode) {
                _fanModesSerializer->add(HAFanModeMedium);
            }

            if (_fanModes & HighFanMode) {
                _fanModesSerializer->add(HAFanModeHigh);
            }

            _serializer->set(
                AHATOFSTR(HAFanModesProperty),
                _fanModesSerializer,
                HASerializer::ArrayPropertyType
            );
        }
    }

    if (_temperatureUnit != DefaultUnit) {
        const __FlashStringHelper *unitStr = _temperatureUnit == CelsiusUnit
            ? AHATOFSTR(HATemperatureUnitC)
            : AHATOFSTR(HATemperatureUnitF);

        _serializer->set(
            AHATOFSTR(HATemperatureUnitProperty),
            unitStr,
            HASerializer::ProgmemPropertyValue
        );
    }

    if (_minTemp != HAUtils::NumberMax) {
        _serializer->set(
            AHATOFSTR(HAMinTempProperty),
            &_minTemp,
            numberProperty
        );
    }

    if (_maxTemp != HAUtils::NumberMax) {
        _serializer->set(
            AHATOFSTR(HAMaxTempProperty),
            &_maxTemp,
            numberProperty
        );
    }

    if (_tempStep != HAUtils::NumberMax) {
        _serializer->set(
            AHATOFSTR(HATempStepProperty),
            &_tempStep,
            numberProperty
        );
    }

    _serializer->topic(AHATOFSTR(HACurrentTemperatureTopic));
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
}

void HAHVAC::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishCurrentTemperature(_currentTemperature);
        publishAction(_action);
        publishAuxState(_auxState);
        publishFanMode(_fanMode);
    }

    if (_features & AuxHeatingFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAAuxCommandTopic));
    }

    if (_features & PowerFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAPowerCommandTopic));
    }

    if (_features & FanFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAFanModeCommandTopic));
    }
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAAuxCommandTopic)
    )) {
        handleAuxStateCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAPowerCommandTopic)
    )) {
        handlePowerCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAFanModeCommandTopic)
    )) {
        handleFanModeCommand(payload, length);
    }
}

bool HAHVAC::publishCurrentTemperature(const HAUtils::Number temperature)
{
    if (temperature == HAUtils::NumberMax) {
        return false;
    }

    uint8_t size = HAUtils::calculateNumberSize(temperature, _precision);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    HAUtils::numberToStr(str, temperature, _precision);

    return publishOnDataTopic(
        AHATOFSTR(HACurrentTemperatureTopic),
        str,
        true
    );
}

bool HAHVAC::publishAction(const Action action)
{
    if (action == UnknownAction || !(_features & ActionFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (action) {
    case OffAction:
        stateStr = AHATOFSTR(HAActionOff);
        break;

    case HeatingAction:
        stateStr = AHATOFSTR(HAActionHeating);
        break;

    case CoolingAction:
        stateStr = AHATOFSTR(HAActionCooling);
        break;

    case DryingAction:
        stateStr = AHATOFSTR(HAActionDrying);
        break;

    case IdleAction:
        stateStr = AHATOFSTR(HAActionIdle);
        break;

    case FanAction:
        stateStr = AHATOFSTR(HAActionFan);
        break;   

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAActionTopic),
        stateStr,
        true
    );
}

bool HAHVAC::publishAuxState(const bool state)
{
    if (!(_features & AuxHeatingFeature)) {
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAAuxStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HAHVAC::publishFanMode(const FanMode mode)
{
    if (mode == UnknownFanMode || !(_features & FanFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (mode) {
    case AutoFanMode:
        stateStr = AHATOFSTR(HAFanModeAuto);
        break;

    case LowFanMode:
        stateStr = AHATOFSTR(HAFanModeLow);
        break;

    case MediumFanMode:
        stateStr = AHATOFSTR(HAFanModeMedium);
        break;

    case HighFanMode:
        stateStr = AHATOFSTR(HAFanModeHigh);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAFanModeStateTopic),
        stateStr,
        true
    );
}

void HAHVAC::handleAuxStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_auxCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _auxCallback(state, this);
}

void HAHVAC::handlePowerCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_powerCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _powerCallback(state, this);
}

void HAHVAC::handleFanModeCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_fanModeCallback) {
        return;
    }

    if (memcmp_P(cmd, HAFanModeAuto, length) == 0) {
        _fanModeCallback(AutoFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeLow, length) == 0) {
        _fanModeCallback(LowFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeMedium, length) == 0) {
        _fanModeCallback(MediumFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeHigh, length) == 0) {
        _fanModeCallback(HighFanMode, this);
    }
}

#endif
