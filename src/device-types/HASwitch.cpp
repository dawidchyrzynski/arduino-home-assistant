#ifndef NO_HA_SWITCH

#include "HASwitch.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

// todo: move all variables to progmem
static const char* HAComponentName = "switch";
static const char* CommandTopic = "cmd";
static const char* StateTopic = "state";
static const char* StateOn = "ON";
static const char* StateOff = "OFF";

HASwitch::HASwitch(const char* name, bool initialState, HAMqtt& mqtt) :
    BaseDeviceType(mqtt),
    _name(name),
    _stateCallback(nullptr),
    _currentState(initialState)
{

}

HASwitch::~HASwitch()
{

}

void HASwitch::onMqttConnected()
{
    if (strlen(_name) == 0) {
        return;
    }

    publishConfig();
    publishState(_currentState);
    subscribeCommandTopic();
}

void HASwitch::onMqttMessage(
    const char* topic,
    const char* payload,
    const uint16_t& length
)
{
    if (strlen(_name) == 0) {
        return;
    }

    static const char Slash[] PROGMEM = {"/"};
    uint8_t suffixLength = strlen(_name) + strlen(CommandTopic) + 3; // two slashes + null terminator
    char suffix[suffixLength];

    strcpy_P(suffix, Slash);
    strcat(suffix, _name);
    strcat_P(suffix, Slash);
    strcat(suffix, CommandTopic);

    if (HAUtils::endsWith(topic, suffix)) {
        bool onState = (strncmp(payload, StateOn, length) == 0);
        setState(onState);
    }
}

bool HASwitch::setState(bool state)
{
    if (_currentState == state) {
        return true;
    }

    if (strlen(_name) == 0) {
        return false;
    }

    if (publishState(state)) {
        _currentState = state;
        triggerCallback(_currentState);
        return true;
    }

    return false;
}

void HASwitch::onStateChanged(HASWITCH_CALLBACK)
{
    _stateCallback = callback;
}

void HASwitch::triggerCallback(bool state)
{
    if (_stateCallback == nullptr) {
        return;
    }

    _stateCallback(state, this);
}

void HASwitch::publishConfig()
{
    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return;
    }

    const uint16_t& deviceLength = device->calculateSerializedLength();
    if (deviceLength == 0) {
        return;
    }

    char serializedDevice[deviceLength];
    if (device->serialize(serializedDevice) == 0) {
        return;
    }

    const uint16_t& topicLength = calculateTopicLength(HAComponentName, _name, ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    generateTopic(topic, HAComponentName, _name, ConfigTopic);

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedTrigger(serializedDevice);
        mqtt()->endPublish();
    }
}

bool HASwitch::publishState(bool state)
{
    if (strlen(_name) == 0) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        _name,
        StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        HAComponentName,
        _name,
        StateTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(topic, (state ? StateOn : StateOff), true);
}

void HASwitch::subscribeCommandTopic()
{
    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        _name,
        CommandTopic
    );
    if (topicSize == 0) {
        return;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        HAComponentName,
        _name,
        CommandTopic
    );

    if (strlen(topic) == 0) {
        return;
    }

    mqtt()->subscribe(topic);
}

uint16_t HASwitch::calculateSerializedLength(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    const uint16_t& cmdTopicLength = calculateTopicLength(
        HAComponentName,
        _name,
        CommandTopic,
        false
    );
    const uint16_t& stateTopicLength = calculateTopicLength(
        HAComponentName,
        _name,
        StateTopic,
        false
    );

    if (cmdTopicLength == 0 || stateTopicLength == 0) {
        return 0;
    }

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        cmdTopicLength + 10 + // 10 - length of the JSON data for this field
        stateTopicLength + 12 + // 12 - length of the JSON data for this field
        strlen(_name) + 10; // 10 - length of the JSON data for this field

    // unique ID
    size += strlen(mqtt()->getDevice()->getUniqueId());
    size += strlen(_name) + 14; // 14 - length of the JSON data for this field

    // device
    size += strlen(serializedDevice) + 7; // 7 - length of the JSON data for this field

    return size;
}

bool HASwitch::writeSerializedTrigger(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    static const char QuotationSign[] PROGMEM = {"\""};

    // command topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            _name,
            CommandTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char cmdTopic[topicSize];
        generateTopic(
            cmdTopic,
            HAComponentName,
            _name,
            CommandTopic
        );

        if (strlen(cmdTopic) == 0) {
            return false;
        }

        static const char DataBefore[] PROGMEM = {"{\"cmd_t\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(cmdTopic, strlen(cmdTopic));
        mqtt()->writePayload_P(QuotationSign);
    }

    // state topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            _name,
            StateTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char stateTopic[topicSize];
        generateTopic(
            stateTopic,
            HAComponentName,
            _name,
            StateTopic
        );

        if (strlen(stateTopic) == 0) {
            return false;
        }

        static const char DataBefore[] PROGMEM = {",\"stat_t\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(stateTopic, strlen(stateTopic));
        mqtt()->writePayload_P(QuotationSign);
    }

    // name
    {
        static const char DataBefore[] PROGMEM = {",\"name\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(_name, strlen(_name));
        mqtt()->writePayload_P(QuotationSign);
    }

    // unique ID
    {
        static const char Underscore[] PROGMEM = {"_"};
        static const char DataBefore[] PROGMEM = {",\"uniq_id\":\""};

        const HADevice* device = mqtt()->getDevice();
        uint8_t uniqueIdLength = strlen(_name) + strlen(device->getUniqueId()) + 2; // underscore and null temrinator
        char uniqueId[uniqueIdLength];
        strcpy(uniqueId, _name);
        strcat_P(uniqueId, Underscore);
        strcat(uniqueId, device->getUniqueId());

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(uniqueId, strlen(uniqueId));
        mqtt()->writePayload_P(QuotationSign);
    }

    // device
    {
        static const char Data[] PROGMEM = {",\"dev\":"};

        mqtt()->writePayload_P(Data);
        mqtt()->writePayload(serializedDevice, strlen(serializedDevice));
    }

    {
        static const char Data[] PROGMEM = {"}"};
        mqtt()->writePayload_P(Data);
    }

    return true;
}

#endif
