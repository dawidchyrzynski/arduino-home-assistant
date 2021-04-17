#include "HAHVAC.h"
#ifdef ARDUINOHA_HVAC

#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

const char* HAHVAC::ActionTopic = "at";
const char* HAHVAC::AuxCommandTopic = "act";
const char* HAHVAC::AuxStateTopic = "ast";
const char* HAHVAC::AwayCommandTopic = "amct";
const char* HAHVAC::AwayStateTopic = "amst";
const char* HAHVAC::HoldCommandTopic = "hct";
const char* HAHVAC::HoldStateTopic = "hst";
const char* HAHVAC::TargetTemperatureCommandTopic = "ttct";
const char* HAHVAC::TargetTemperatureStateTopic = "ttst";
const char* HAHVAC::CurrentTemperatureTopic = "ctt";
const char* HAHVAC::ModeCommandTopic = "mct";
const char* HAHVAC::ModeStateTopic = "mst";

static const char OffStr[] PROGMEM = {"off"};
static const char AutoStr[] PROGMEM = {"auto"};
static const char CoolStr[] PROGMEM = {"cool"};
static const char HeatStr[] PROGMEM = {"heat"};
static const char DryStr[] PROGMEM = {"dry"};
static const char FanOnlyStr[] PROGMEM = {"fan_only"};

HAHVAC::HAHVAC(const char* uniqueId, uint8_t features) :
    BaseDeviceType("climate", uniqueId),
    _uniqueId(uniqueId),
    _features(features),
    _temperatureUnit(DefaultUnit),
    _action(OffAction),
    _auxHeatingCallback(nullptr),
    _auxHeatingState(false),
    _awayCallback(nullptr),
    _awayState(false),
    _holdCallback(nullptr),
    _holdState(false),
    _currentTemperature(__DBL_MAX__),
    _minTemp(__DBL_MAX__),
    _maxTemp(__DBL_MAX__),
    _tempStep(1),
    _targetTempCallback(nullptr),
    _targetTemperature(__DBL_MAX__),
    _modes(
        OffMode |
        AutoMode |
        CoolMode |
        HeatMode |
        DryMode |
        FanOnlyMode
    ),
    _modeChangedCallback(nullptr),
    _currentMode(Mode::UnknownMode),
    _retain(false)
{

}

HAHVAC::HAHVAC(
    const char* uniqueId,
    uint8_t features,
    HAMqtt& mqtt
) :
    HAHVAC(uniqueId, features)
{
    (void)mqtt;
}

void HAHVAC::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
        return;
    }

    publishConfig();
    publishCurrentTemperature(_currentTemperature);
    publishTargetTemperature(_targetTemperature);

    if (!_retain) {
        publishAction(_action);
        publishAuxHeatingState(_auxHeatingState);
        publishAwayState(_awayState);
        publishHoldState(_holdState);
        publishMode(_currentMode);
    }

    subscribeTopics();
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    if ((_features & AuxHeatingFeature) &&
            compareTopics(topic, AuxCommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setAuxHeatingState(state);
    } else if ((_features & AwayModeFeature) &&
            compareTopics(topic, AwayCommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setAwayState(state);
    } else if ((_features & HoldFeature) &&
            compareTopics(topic, HoldCommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setHoldState(state);
    } else if (compareTopics(topic, TargetTemperatureCommandTopic)) {
        char src[length + 1];
        memset(src, 0, sizeof(src));
        memcpy(src, payload, length);

        setTargetTemperature(HAUtils::strToTemp(src));
    } else if (compareTopics(topic, ModeCommandTopic)) {
        char mode[length + 1];
        memset(mode, 0, sizeof(mode));
        memcpy(mode, payload, length);

        setModeFromStr(mode);
    }
}

bool HAHVAC::setAction(Action action)
{
    if (publishAction(action)) {
        _action = action;
        return true;
    }

    return false;
}

bool HAHVAC::setAuxHeatingState(bool state)
{
    if (publishAuxHeatingState(state)) {
        _auxHeatingState = state;

        if (_auxHeatingCallback) {
            _auxHeatingCallback(_auxHeatingState);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setAwayState(bool state)
{
    if (publishAwayState(state)) {
        _awayState = state;

        if (_awayCallback) {
            _awayCallback(_awayState);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setHoldState(bool state)
{
    if (publishHoldState(state)) {
        _holdState = state;

        if (_holdCallback) {
            _holdCallback(_holdState);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setTargetTemperature(double temperature)
{
    if (publishTargetTemperature(temperature)) {
        _targetTemperature = temperature;

        if (_targetTempCallback) {
            _targetTempCallback(_targetTemperature);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setCurrentTemperature(double temperature)
{
    if (_currentTemperature == temperature) {
        return true;
    }

    if (publishCurrentTemperature(temperature)) {
        _currentTemperature = temperature;
        return true;
    }

    return false;
}

bool HAHVAC::setMode(Mode mode)
{
    if (mode == UnknownMode) {
        return false;
    }

    if (publishMode(mode)) {
        _currentMode = mode;

        if (_modeChangedCallback) {
            _modeChangedCallback(_currentMode);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setModeFromStr(const char* mode)
{
    if (strcmp_P(mode, OffStr) == 0) {
        return setMode(OffMode);
    } else if (strcmp_P(mode, AutoStr) == 0) {
        return setMode(AutoMode);
    } else if (strcmp_P(mode, CoolStr) == 0) {
        return setMode(CoolMode);
    } else if (strcmp_P(mode, HeatStr) == 0) {
        return setMode(HeatMode);
    } else if (strcmp_P(mode, DryStr) == 0) {
        return setMode(DryMode);
    } else if (strcmp_P(mode, FanOnlyStr) == 0) {
        return setMode(FanOnlyMode);
    }

    return false;
}

bool HAHVAC::setMinTemp(double minTemp)
{
    if (minTemp == __DBL_MAX__) {
        return false;
    }

    _minTemp = minTemp;
    return true;
}

bool HAHVAC::setMaxTemp(double maxTemp)
{
    if (maxTemp == __DBL_MAX__) {
        return false;
    }

    _maxTemp = maxTemp;
    return true;
}

bool HAHVAC::setTempStep(double tempStep)
{
    if (tempStep <= 0 || tempStep >= 255) {
        return false;
    }

    _tempStep = tempStep;
    return true;
}

bool HAHVAC::publishAction(Action action)
{
    if (!(_features & ActionFeature) ||
            strlen(uniqueId()) == 0) {
        return false;
    }

    char actionStr[8];
    switch (action) {
        case OffAction:
            strcpy_P(actionStr, OffStr);
            break;

        case HeatingAction:
            strcpy(actionStr, "heating");
            break;

        case CoolingAction:
            strcpy(actionStr, "cooling");
            break;

        case DryingAction:
            strcpy(actionStr, "drying");
            break;

        case IdleAction:
            strcpy(actionStr, "idle");
            break;

        case FanAction:
            strcpy(actionStr, "fan");
            break;

        default:
            return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        ActionTopic,
        actionStr
    );
}

bool HAHVAC::publishAuxHeatingState(bool state)
{
    if (!(_features & AuxHeatingFeature) ||
            strlen(uniqueId()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        AuxStateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

bool HAHVAC::publishAwayState(bool state)
{
    if (!(_features & AwayModeFeature) ||
            strlen(uniqueId()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        AwayStateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

bool HAHVAC::publishHoldState(bool state)
{
    if (!(_features & HoldFeature) ||
            strlen(uniqueId()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        HoldStateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

bool HAHVAC::publishCurrentTemperature(double temperature)
{
    if (strlen(uniqueId()) == 0) {
        return false;
    }

    if (temperature == __DBL_MAX__) {
        return false;
    }

    char str[AHA_SERIALIZED_TEMP_SIZE];
    HAUtils::tempToStr(str, temperature);

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        CurrentTemperatureTopic,
        str
    );
}

bool HAHVAC::publishTargetTemperature(double temperature)
{
    if (strlen(uniqueId()) == 0) {
        return false;
    }

    if (temperature == __DBL_MAX__) {
        return false;
    }

    char str[AHA_SERIALIZED_TEMP_SIZE];
    HAUtils::tempToStr(str, temperature);

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        TargetTemperatureStateTopic,
        str
    );
}

bool HAHVAC::publishMode(Mode mode)
{
    if (strlen(uniqueId()) == 0 || mode == UnknownMode) {
        return false;
    }

    char modeStr[9];
    switch (mode) {
        case OffMode:
            strcpy_P(modeStr, OffStr);
            break;

        case AutoMode:
            strcpy_P(modeStr, AutoStr);
            break;

        case CoolMode:
            strcpy_P(modeStr, CoolStr);
            break;

        case HeatMode:
            strcpy_P(modeStr, HeatStr);
            break;

        case DryMode:
            strcpy_P(modeStr, DryStr);
            break;

        case FanOnlyMode:
            strcpy_P(modeStr, FanOnlyStr);
            break;

        default:
            return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        ModeStateTopic,
        modeStr
    );
}

void HAHVAC::subscribeTopics()
{
    // aux heating
    if (_features & AuxHeatingFeature) {
        DeviceTypeSerializer::mqttSubscribeTopic(
            this,
            AuxCommandTopic
        );
    }

    // away mode
    if (_features & AwayModeFeature) {
        DeviceTypeSerializer::mqttSubscribeTopic(
            this,
            AwayCommandTopic
        );
    }

    // hold
    if (_features & HoldFeature) {
        DeviceTypeSerializer::mqttSubscribeTopic(
            this,
            HoldCommandTopic
        );
    }

    // target temperature
    DeviceTypeSerializer::mqttSubscribeTopic(
        this,
        TargetTemperatureCommandTopic
    );

    // mode
    DeviceTypeSerializer::mqttSubscribeTopic(
        this,
        ModeCommandTopic
    );
}

uint16_t HAHVAC::calculateSerializedLength(const char* serializedDevice) const
{
    if (serializedDevice == nullptr || _uniqueId == nullptr) {
        return 0;
    }

    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateNameFieldSize(getName());
    size += DeviceTypeSerializer::calculateUniqueIdFieldSize(uniqueId());
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);
    size += DeviceTypeSerializer::calculateAvailabilityFieldSize(this);
    size += DeviceTypeSerializer::calculateRetainFieldSize(_retain);

    // current temperature
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            uniqueId(),
            CurrentTemperatureTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: "curr_temp_t":"[TOPIC]"
        size += topicLength + 16; // 16 - length of the JSON decorators for this field
    }

    // action topic
    if (_features & ActionFeature) {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            uniqueId(),
            ActionTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: ,"act_t":"[TOPIC]"
        size += topicLength + 11; // 11 - length of the JSON decorators for this field
    }

    // aux heating
    if (_features & AuxHeatingFeature) {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                AuxCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"aux_cmd_t":"[TOPIC]"
            size += topicLength + 15; // 15 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                AuxStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"aux_stat_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }
    }

    // away mode
    if (_features & AwayModeFeature) {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                AwayCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"away_mode_cmd_t":"[TOPIC]"
            size += topicLength + 21; // 21 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                AwayStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"away_mode_stat_t":"[TOPIC]"
            size += topicLength + 22; // 22 - length of the JSON decorators for this field
        }
    }

    // hold
    if (_features & HoldFeature) {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                HoldCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"hold_cmd_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                HoldStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"hold_stat_t":"[TOPIC]"
            size += topicLength + 17; // 17 - length of the JSON decorators for this field
        }
    }

    // min temp
    if (_minTemp != __DBL_MAX__) {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _minTemp);

        // Field format: ,"min_temp":[TEMP]
        size += strlen(str) + 12; // 12 - length of the JSON decorators for this field
    }

    // max temp
    if (_maxTemp != __DBL_MAX__) {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _maxTemp);

        // Field format: ,"max_temp":[TEMP]
        size += strlen(str) + 12; // 12 - length of the JSON decorators for this field
    }

    // temp step
    {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _tempStep);

        // Field format: ,"temp_step":[TEMP]
        size += strlen(str) + 13; // 13 - length of the JSON decorators for this field
    }

    // target temperature
    {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                TargetTemperatureCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"temp_cmd_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                TargetTemperatureStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"temp_stat_t":"[TOPIC]"
            size += topicLength + 17; // 17 - length of the JSON decorators for this field
        }
    }

    // temperature unit
    if (_temperatureUnit != DefaultUnit) {
        // Field format: ,"temp_unit":"[UNIT]"
        // UNIT may C or F
        size += 15 + 1; // 15 - length of the JSON decorators for this field
    }

    // modes
    if (_modes != 0) {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                ModeCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"mode_cmd_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                ModeStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"mode_stat_t":"[TOPIC]"
            size += topicLength + 17; // 17 - length of the JSON decorators for this field
        }
    }

    // Supported modes
    // Field format: ,"modes":MODES
    size += 9 + calculateModesLength(); // 9 - length of the JSON decorators for this field

    return size; // excludes null terminator
}

uint16_t HAHVAC::calculateModesLength() const
{
    uint16_t length = 2; // opening and closing bracket

    if (_modes & OffMode) {
        // escape + comma
        length += 3 + strlen_P(OffStr);
    }

    if (_modes & AutoMode) {
        // escape + comma
        length += 3 + strlen_P(AutoStr);
    }

    if (_modes & CoolMode) {
        // escape + comma
        length += 3 + strlen_P(CoolStr);
    }

    if (_modes & HeatMode) {
        // escape + comma
        length += 3 + strlen_P(HeatStr);
    }

    if (_modes & DryMode) {
        // escape + comma
        length += 3 + strlen_P(DryStr);
    }

    if (_modes & FanOnlyMode) {
        // escape + comma
        length += 3 + strlen_P(FanOnlyStr);
    }

    if (length > 2) {
        length--; // remove trailing comma
    }

    return length; // excludes null terminator
}

bool HAHVAC::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr || _uniqueId == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // current temperature topic
    {
        static const char Prefix[] PROGMEM = {"\"curr_temp_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            CurrentTemperatureTopic
        );
    }

    // action topic
    if (_features & ActionFeature) {
        static const char Prefix[] PROGMEM = {",\"act_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            ActionTopic
        );
    }

    // aux heating
    if (_features & AuxHeatingFeature) {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"aux_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                AuxCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"aux_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                AuxStateTopic
            );
        }
    }

    // away mode
    if (_features & AwayModeFeature) {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"away_mode_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                AwayCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"away_mode_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                AwayStateTopic
            );
        }
    }

    // hold
    if (_features & HoldFeature) {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"hold_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                HoldCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"hold_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                HoldStateTopic
            );
        }
    }

    // min temp
    if (_minTemp != __DBL_MAX__) {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _minTemp);

        static const char Prefix[] PROGMEM = {",\"min_temp\":"};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            str,
            false
        );
    }

    // max temp
    if (_maxTemp != __DBL_MAX__) {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _maxTemp);

        static const char Prefix[] PROGMEM = {",\"max_temp\":"};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            str,
            false
        );
    }

    // temp step
    {
        char str[AHA_SERIALIZED_TEMP_SIZE];
        HAUtils::tempToStr(str, _tempStep);

        static const char Prefix[] PROGMEM = {",\"temp_step\":"};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            str,
            false
        );
    }

    // target temperature
    {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"temp_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                TargetTemperatureCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"temp_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                TargetTemperatureStateTopic
            );
        }
    }

    // temperature unit
    if (_temperatureUnit != DefaultUnit) {
        static const char Prefix[] PROGMEM = {",\"temp_unit\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            (_temperatureUnit == CelsiusUnit ? "C" : "F")
        );
    }

    // modes
    if (_modes != 0) {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"mode_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                ModeCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"mode_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                ModeStateTopic
            );
        }
    }

    // supported modes
    {
        static const char CharQuotation[] PROGMEM = {"\""};
        static const char CharQuotationComma[] PROGMEM = {"\","};
        static const char Prefix[] PROGMEM = {",\"modes\":"};

        char modesStr[calculateModesLength() + 1]; // plus null terminator
        memset(modesStr, 0, sizeof(modesStr));
        strcpy(modesStr, "[");

        if (_modes != 0) {
            if (_modes & OffMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, OffStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            if (_modes & AutoMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, AutoStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            if (_modes & CoolMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, CoolStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            if (_modes & HeatMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, HeatStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            if (_modes & DryMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, DryStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            if (_modes & FanOnlyMode) {
                strcat_P(modesStr, CharQuotation);
                strcat_P(modesStr, FanOnlyStr);
                strcat_P(modesStr, CharQuotationComma);
            }

            modesStr[strlen(modesStr) - 1] = ']';
        } else {
            strcat(modesStr, "]");
        }

        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            modesStr,
            false
        );
    }

    DeviceTypeSerializer::mqttWriteNameField(getName());
    DeviceTypeSerializer::mqttWriteUniqueIdField(uniqueId());
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteRetainField(_retain);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
