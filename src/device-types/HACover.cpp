#include "HACover.h"
#ifdef ARDUINOHA_COVER

#include "../HAMqtt.h"
#include "../HADevice.h"

static const char ClosedStateStr[] PROGMEM = {"closed"};
static const char ClosingStateStr[] PROGMEM = {"closing"};
static const char OpenStateStr[] PROGMEM = {"open"};
static const char OpeningStateStr[] PROGMEM = {"opening"};
static const char StoppedStateStr[] PROGMEM = {"stopped"};
static const char OpenCommandStr[] PROGMEM = {"OPEN"};
static const char CloseCommandStr[] PROGMEM = {"CLOSE"};
static const char StopCommandStr[] PROGMEM = {"STOP"};

const char* HACover::PositionTopic = "ps";

HACover::HACover(const char* uniqueId) :
    BaseDeviceType("cover", uniqueId),
    _commandCallback(nullptr),
    _currentState(StateUnknown),
    _currentPosition(0),
    _retain(false)
{

}

HACover::HACover(const char* uniqueId, HAMqtt& mqtt) :
    HACover(uniqueId)
{
    (void)mqtt;
}

void HACover::onMqttConnected()
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

    if (!_retain) {
        publishState(_currentState);
        publishPosition(_currentPosition);
    }
}

void HACover::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    (void)payload;

    if (compareTopics(topic, DeviceTypeSerializer::CommandTopic)) {
        char cmd[length + 1];
        memset(cmd, 0, sizeof(cmd));
        memcpy(cmd, payload, length);
        handleCommand(cmd);
    }
}

bool HACover::setState(CoverState state, bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HACover::setPosition(int16_t position)
{
    if (_currentPosition == position) {
        return true;
    }

    if (publishPosition(position)) {
        _currentPosition = position;
        return true;
    }

    return false;
}

bool HACover::publishState(CoverState state)
{
    if (strlen(uniqueId()) == 0 || state == StateUnknown) {
        return false;
    }

    char stateStr[8];
    switch (state) {
        case StateClosed:
            strcpy_P(stateStr, ClosedStateStr);
            break;

        case StateClosing:
            strcpy_P(stateStr, ClosingStateStr);
            break;

        case StateOpen:
            strcpy_P(stateStr, OpenStateStr);
            break;

        case StateOpening:
            strcpy_P(stateStr, OpeningStateStr);
            break;

        case StateStopped:
            strcpy_P(stateStr, StoppedStateStr);
            break;

        default:
            return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        DeviceTypeSerializer::StateTopic,
        stateStr
    );
}

bool HACover::publishPosition(int16_t position)
{
    if (strlen(uniqueId()) == 0) {
        return false;
    }

    uint8_t digitsNb = floor(log10(position)) + 1;
    char str[digitsNb + 2]; // + null terminator + negative sign
    memset(str, 0, sizeof(str));
    itoa(position, str, 10);

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        PositionTopic,
        str
    );
}

uint16_t HACover::calculateSerializedLength(const char* serializedDevice) const
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

    // position topic
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            uniqueId(),
            PositionTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: ,"pos_t":"[TOPIC]"
        size += topicLength + 11; // 11 - length of the JSON decorators for this field
    }

    return size; // exludes null terminator
}

bool HACover::writeSerializedData(const char* serializedDevice) const
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

    // position topic
    {
        static const char Prefix[] PROGMEM = {",\"pos_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            PositionTopic
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

void HACover::handleCommand(const char* cmd)
{
    if (!_commandCallback) {
        return;
    }

    if (strcmp_P(cmd, CloseCommandStr) == 0) {
        _commandCallback(CommandClose);
    } else if (strcmp_P(cmd, OpenCommandStr) == 0) {
        _commandCallback(CommandOpen);
    } else if (strcmp_P(cmd, StopCommandStr) == 0) {
        _commandCallback(CommandStop);
    }
}

#endif
