#include "HAFan.h"
#ifdef ARDUINOHA_FAN

#include "../HAMqtt.h"
#include "../HADevice.h"

const char* HAFan::SpeedCommandTopic = {"sct"};
const char* HAFan::SpeedStateTopic = {"sst"};

static const char OffStr[] PROGMEM = {"off"};
static const char LowStr[] PROGMEM = {"low"};
static const char MediumStr[] PROGMEM = {"medium"};
static const char HighStr[] PROGMEM = {"high"};

HAFan::HAFan(const char* uniqueId, uint8_t features) :
    BaseDeviceType("fan", uniqueId),
    _features(features),
    _speeds(OffSpeed | LowSpeed | MediumSpeed | HighSpeed),
    _currentState(false),
    _stateCallback(nullptr),
    _currentSpeed(OffSpeed),
    _speedCallback(nullptr),
    _label(nullptr),
    _retain(false)
{

}

HAFan::HAFan(const char* uniqueId, uint8_t features, HAMqtt& mqtt) :
    HAFan(uniqueId, features)
{
    (void)mqtt;
}

void HAFan::onMqttConnected()
{
    if (strlen(name()) == 0) {
        return;
    }

    publishConfig();
    publishAvailability();

    DeviceTypeSerializer::mqttSubscribeTopic(
        this,
        DeviceTypeSerializer::CommandTopic
    );

    if (_features & SpeedsFeature) {
        DeviceTypeSerializer::mqttSubscribeTopic(
            this,
            SpeedCommandTopic
        );
    }

    if (!_retain) {
        publishState(_currentState);
        publishSpeed(_currentSpeed);
    }
}

void HAFan::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    (void)payload;

    if (isMyTopic(topic, DeviceTypeSerializer::CommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setState(state, true);
    } else if (isMyTopic(topic, SpeedCommandTopic)) {
        char speed[length + 1];
        memset(speed, 0, sizeof(speed));
        memcpy(speed, payload, length);

        setSpeedFromStr(speed);
    }
}

bool HAFan::setState(bool state, bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;

        if (_stateCallback) {
            _stateCallback(state);
        }

        return true;
    }

    return false;
}

bool HAFan::setSpeed(Speed speed)
{
    if (speed == UnknownSpeed) {
        return false;
    }

    if (publishSpeed(speed)) {
        _currentSpeed = speed;

        if (_speedCallback) {
            _speedCallback(_currentSpeed);
        }

        return true;
    }

    return false;
}

bool HAFan::setSpeedFromStr(const char* speed)
{
    if (strcmp_P(speed, OffStr) == 0) {
        return setSpeed(OffSpeed);
    } else if (strcmp_P(speed, LowStr) == 0) {
        return setSpeed(LowSpeed);
    } else if (strcmp_P(speed, MediumStr) == 0) {
        return setSpeed(MediumSpeed);
    } else if (strcmp_P(speed, HighStr) == 0) {
        return setSpeed(HighSpeed);
    }

    return false;
}

bool HAFan::publishState(bool state)
{
    if (strlen(name()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        DeviceTypeSerializer::StateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

bool HAFan::publishSpeed(Speed speed)
{
    if (strlen(name()) == 0 || speed == UnknownSpeed) {
        return false;
    }

    char speedStr[7];
    switch (speed) {
        case OffSpeed:
            strcpy_P(speedStr, OffStr);
            break;

        case LowSpeed:
            strcpy_P(speedStr, LowStr);
            break;

        case MediumSpeed:
            strcpy_P(speedStr, MediumStr);
            break;

        case HighSpeed:
            strcpy_P(speedStr, HighStr);
            break;

        default:
            return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        SpeedStateTopic,
        speedStr
    );
}

uint16_t HAFan::calculateSerializedLength(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateUniqueIdFieldSize(name());
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);
    size += DeviceTypeSerializer::calculateAvailabilityFieldSize(this);
    size += DeviceTypeSerializer::calculateNameFieldSize(_label);
    size += DeviceTypeSerializer::calculateRetainFieldSize(_retain);

    // command topic
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            name(),
            DeviceTypeSerializer::CommandTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: "cmd_t":"[TOPIC]"
        size += topicLength + 10; // 10 - length of the JSON decorators for this field
    }

    // state topic
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            name(),
            DeviceTypeSerializer::StateTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: ,"stat_t":"[TOPIC]"
        size += topicLength + 12; // 12 - length of the JSON decorators for this field
    }

    // speeds
    if (_features & SpeedsFeature) {
        // command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                name(),
                SpeedCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"spd_cmd_t":"[TOPIC]"
            size += topicLength + 15; // 15 - length of the JSON decorators for this field
        }

        // state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                name(),
                SpeedStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"spd_stat_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }

        // speeds list
        // Format: ,"spds":[SPEEDS]
        size += calculateSpeedsLength() + 8; // 8 - length of the JSON decorators for this field
    }

    return size; // exludes null terminator
}

uint16_t HAFan::calculateSpeedsLength() const
{
    uint16_t length = 2; // opening and closing bracket

    if (_speeds & OffSpeed) {
        // escape + comma
        length += 3 + strlen_P(OffStr);
    }

    if (_speeds & LowSpeed) {
        // escape + comma
        length += 3 + strlen_P(LowStr);
    }

    if (_speeds & MediumSpeed) {
        // escape + comma
        length += 3 + strlen_P(MediumStr);
    }

    if (_speeds & HighSpeed) {
        // escape + comma
        length += 3 + strlen_P(HighStr);
    }

    if (length > 2) {
        length--; // remove trailing comma
    }

    return length; // excludes null terminator
}

bool HAFan::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // command topic
    {
        static const char Prefix[] PROGMEM = {"\"cmd_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            DeviceTypeSerializer::CommandTopic
        );
    }

    // state topic
    {
        static const char Prefix[] PROGMEM = {",\"stat_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            DeviceTypeSerializer::StateTopic
        );
    }

    // speeds
    if (_features & SpeedsFeature) {
        // command topic
        {
            static const char Prefix[] PROGMEM = {",\"spd_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                SpeedCommandTopic
            );
        }

        // state topic
        {
            static const char Prefix[] PROGMEM = {",\"spd_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                SpeedStateTopic
            );
        }

        // supported speeds
        {
            static const char CharQuotation[] PROGMEM = {"\""};
            static const char CharQuotationComma[] PROGMEM = {"\","};
            static const char Prefix[] PROGMEM = {",\"spds\":"};

            char speedStr[calculateSpeedsLength() + 1]; // plus null terminator
            memset(speedStr, 0, sizeof(speedStr));
            strcpy(speedStr, "[");

            if (_speeds != 0) {
                if (_speeds & OffSpeed) {
                    strcat_P(speedStr, CharQuotation);
                    strcat_P(speedStr, OffStr);
                    strcat_P(speedStr, CharQuotationComma);
                }

                if (_speeds & LowSpeed) {
                    strcat_P(speedStr, CharQuotation);
                    strcat_P(speedStr, LowStr);
                    strcat_P(speedStr, CharQuotationComma);
                }

                if (_speeds & MediumSpeed) {
                    strcat_P(speedStr, CharQuotation);
                    strcat_P(speedStr, MediumStr);
                    strcat_P(speedStr, CharQuotationComma);
                }

                if (_speeds & HighSpeed) {
                    strcat_P(speedStr, CharQuotation);
                    strcat_P(speedStr, HighStr);
                    strcat_P(speedStr, CharQuotationComma);
                }

                speedStr[strlen(speedStr) - 1] = ']';
            } else {
                strcat(speedStr, "]");
            }

            DeviceTypeSerializer::mqttWriteConstCharField(
                Prefix,
                speedStr,
                false
            );
        }
    }

    DeviceTypeSerializer::mqttWriteRetainField(_retain);
    DeviceTypeSerializer::mqttWriteNameField(_label);
    DeviceTypeSerializer::mqttWriteUniqueIdField(name());
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
