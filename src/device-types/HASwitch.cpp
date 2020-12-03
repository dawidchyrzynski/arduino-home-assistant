#ifndef NO_HA_SWITCH

#include "HASwitch.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

// todo: move all variables to progmem
static const char* HAComponentName = "switch";
static const char* CommandTopic = "cmd";
static const char* StateTopic = "state";
static const char* StateOn = "ON";
static const char* StateOff = "OFF";

HASwitch::HASwitch(const char* name, HAMqtt& mqtt) :
    BaseDeviceType(mqtt),
    _name(name),
    _stateCallback(nullptr),
    _currentState(false)
{

}

HASwitch::~HASwitch()
{

}

void HASwitch::onMqttConnected()
{
    publishConfig();
}

void HASwitch::setState(bool state)
{
    if (state) {
        turnOn();
    } else {
        turnOff();
    }
}

void HASwitch::turnOn()
{
    _currentState = true;
    triggerCallback(_currentState);
    publishCurrentState();
}

void HASwitch::turnOff()
{
    _currentState = false;
    triggerCallback(_currentState);
    publishCurrentState();
}

void HASwitch::onStateChanged(HASWITCH_CALLBACK)
{
    if (_stateCallback != nullptr) {
        return; // only one callback may be assigned to the switch
    }

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
    if (strlen(_name) == 0) {
        return;
    }

    const HADevice* device = mqtt()->getDevice();
    uint16_t deviceLength = (device == nullptr ? 0 : device->calculateSerializedLength());
    char serializedDevice[deviceLength];

    if (device != nullptr) {
        device->serialize(serializedDevice);
    }

    const uint16_t& topicLength = calculateTopicLength(HAComponentName, _name, ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    char topic[topicLength];
    generateTopic(topic, HAComponentName, _name, ConfigTopic);

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedTrigger(serializedDevice);
        mqtt()->endPublish();
    }
}

void HASwitch::publishCurrentState()
{
    if (strlen(_name) == 0) {
        return;
    }

    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        _name,
        StateTopic
    );
    char topic[topicSize];
    generateTopic(
        topic,
        HAComponentName,
        _name,
        StateTopic
    );

    mqtt()->publish(topic, (_currentState ? StateOn : StateOff));
}

uint16_t HASwitch::calculateSerializedLength(const char* serializedDevice) const
{
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

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        cmdTopicLength + 10 +
        stateTopicLength + 12 +
        strlen(_name) + 10;

    // unique ID
    if (serializedDevice == nullptr) {
        size += strlen(_name) + 13;
    } else {
        size += strlen(mqtt()->getDevice()->getUniqueId()) + strlen(_name) + 14;

        // device
        size += strlen(serializedDevice) + 7;
    }

    return size;
}

bool HASwitch::writeSerializedTrigger(const char* serializedDevice) const
{
    static const char QuotationSign[] PROGMEM = {"\""};

    // command topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            _name,
            CommandTopic
        );
        char cmdTopic[topicSize];
        generateTopic(
            cmdTopic,
            HAComponentName,
            _name,
            CommandTopic
        );

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
        char stateTopic[topicSize];
        generateTopic(
            stateTopic,
            HAComponentName,
            _name,
            StateTopic
        );

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
        static const char DataBefore[] PROGMEM = {",\"uniq_id\":\""};
        const HADevice* device = mqtt()->getDevice();

        mqtt()->writePayload_P(DataBefore);

        if (device == nullptr) {
            mqtt()->writePayload(_name, strlen(_name));
        } else {
            static const char Underscore[] PROGMEM = {"_"};

            uint8_t uniqueIdLength = strlen(_name) + strlen(device->getUniqueId()) + 2; // underscore and null temrinator
            char uniqueId[uniqueIdLength];
            strcpy(uniqueId, _name);
            strcat_P(uniqueId, Underscore);
            strcat(uniqueId, device->getUniqueId());

            mqtt()->writePayload(uniqueId, strlen(uniqueId));
        }

        mqtt()->writePayload_P(QuotationSign);
    }

    // device
    if (serializedDevice != nullptr) {
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
