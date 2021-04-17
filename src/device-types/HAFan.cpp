#include "HAFan.h"
#ifdef ARDUINOHA_FAN

#include "../HAMqtt.h"
#include "../HADevice.h"

const char* HAFan::PercentageCommandTopic = {"sct"};
const char* HAFan::PercentageStateTopic = {"sst"};

HAFan::HAFan(const char* uniqueId, uint8_t features) :
    BaseDeviceType("fan", uniqueId),
    _features(features),
    _currentState(false),
    _stateCallback(nullptr),
    _currentSpeed(0),
    _speedCallback(nullptr),
    _retain(false),
    _speedRangeMin(1),
    _speedRangeMax(100)
{

}

HAFan::HAFan(const char* uniqueId, uint8_t features, HAMqtt& mqtt) :
    HAFan(uniqueId, features)
{
    (void)mqtt;
}

void HAFan::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
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
            PercentageCommandTopic
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

    if (compareTopics(topic, DeviceTypeSerializer::CommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setState(state, true);
    } else if (compareTopics(topic, PercentageCommandTopic)) {
        char speedStr[length + 1];
        memset(speedStr, 0, sizeof(speedStr));
        memcpy(speedStr, payload, length);
        int32_t speed = atoi(speedStr);
        if (speed >= 0) {
            setSpeed(speed);
        }
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

bool HAFan::setSpeed(uint16_t speed)
{
    if (publishSpeed(speed)) {
        _currentSpeed = speed;

        if (_speedCallback) {
            _speedCallback(_currentSpeed);
        }

        return true;
    }

    return false;
}

bool HAFan::publishState(bool state)
{
    if (strlen(uniqueId()) == 0) {
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

bool HAFan::publishSpeed(uint16_t speed)
{
    if (strlen(uniqueId()) == 0) {
        return false;
    }

    uint8_t digitsNb = floor(log10(speed)) + 1;
    char str[digitsNb + 1]; // + null terminator
    memset(str, 0, sizeof(str));
    itoa(speed, str, 10);

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        PercentageStateTopic,
        str
    );
}

uint16_t HAFan::calculateSerializedLength(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateNameFieldSize(getName());
    size += DeviceTypeSerializer::calculateUniqueIdFieldSize(uniqueId());
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);
    size += DeviceTypeSerializer::calculateAvailabilityFieldSize(this);
    size += DeviceTypeSerializer::calculateRetainFieldSize(_retain);

    // command topic
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            uniqueId(),
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
            uniqueId(),
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
        // percentage command topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                PercentageCommandTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"pct_cmd_t":"[TOPIC]"
            size += topicLength + 15; // 15 - length of the JSON decorators for this field
        }

        // percentage state topic
        {
            const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
                componentName(),
                uniqueId(),
                PercentageStateTopic,
                false
            );

            if (topicLength == 0) {
                return 0;
            }

            // Field format: ,"pct_stat_t":"[TOPIC]"
            size += topicLength + 16; // 16 - length of the JSON decorators for this field
        }

        // speed range min
        if (_speedRangeMin != 1) {
            uint8_t digitsNb = floor(log10(_speedRangeMin)) + 1;

            // Field format: ,"spd_rng_min":[VALUE]
            size += digitsNb + 15; // 15 - length of the JSON decorators for this field
        }

        // speed range max
        if (_speedRangeMax != 100) {
            uint8_t digitsNb = floor(log10(_speedRangeMax)) + 1;

            // Field format: ,"spd_rng_max":[VALUE]
            size += digitsNb + 15; // 15 - length of the JSON decorators for this field
        }
    }

    return size; // excludes null terminator
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
        // percentage command topic
        {
            static const char Prefix[] PROGMEM = {",\"pct_cmd_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                PercentageCommandTopic
            );
        }

        // percentage state topic
        {
            static const char Prefix[] PROGMEM = {",\"pct_stat_t\":\""};
            DeviceTypeSerializer::mqttWriteTopicField(
                this,
                Prefix,
                PercentageStateTopic
            );
        }

        // speed range min
        if (_speedRangeMin != 1) {
            uint8_t digitsNb = floor(log10(_speedRangeMin)) + 1;
            char str[digitsNb + 1]; // + null terminator
            memset(str, 0, sizeof(str));
            itoa(_speedRangeMin, str, 10);

            static const char Prefix[] PROGMEM = {",\"spd_rng_min\":"};
            DeviceTypeSerializer::mqttWriteConstCharField(
                Prefix,
                str,
                false
            );
        }

        // speed range max
        if (_speedRangeMax != 100) {
            uint8_t digitsNb = floor(log10(_speedRangeMax)) + 1;
            char str[digitsNb + 1]; // + null terminator
            memset(str, 0, sizeof(str));
            itoa(_speedRangeMax, str, 10);

            static const char Prefix[] PROGMEM = {",\"spd_rng_max\":"};
            DeviceTypeSerializer::mqttWriteConstCharField(
                Prefix,
                str,
                false
            );
        }
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
