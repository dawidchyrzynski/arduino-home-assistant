#include "HASwitch.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

const char* HASwitch::CommandTopic = "cmd";
const char* HASwitch::StateTopic = "state";
const char* HASwitch::StateOn = "ON";
const char* HASwitch::StateOff = "OFF";

HASwitch::HASwitch(const char* name, bool initialState, HAMqtt& mqtt) :
    BaseDeviceType(mqtt, "switch", _name),
    _stateCallback(nullptr),
    _currentState(initialState)
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
    publishAvailability();
}

void HASwitch::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
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
        bool onState = (length == strlen(StateOn));
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

    const uint16_t& topicLength = calculateTopicLength(componentName(), name(), ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    generateTopic(topic, componentName(), name(), ConfigTopic);

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedData(serializedDevice);
        mqtt()->endPublish();
    }
}

bool HASwitch::publishState(bool state)
{
    if (strlen(_name) == 0) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        componentName(),
        name(),
        StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        componentName(),
        name(),
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
        componentName(),
        name(),
        CommandTopic
    );
    if (topicSize == 0) {
        return;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        componentName(),
        name(),
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

    uint16_t size = calculateBaseJsonDataSize();
    size += calculateNameFieldSize();
    size += calculateUniqueIdFieldSize();
    size += calculateAvailabilityFieldSize();
    size += calculateDeviceFieldSize(serializedDevice);

    // cmd topic
    {
        const uint16_t& topicLength = calculateTopicLength(
            componentName(),
            name(),
            CommandTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: "cmd_t":"[TOPIC]",
        size += topicLength + 11; // 10 - length of the JSON decorators for this field
    }

    // state topic
    {
        const uint16_t& topicLength = calculateTopicLength(
            componentName(),
            name(),
            StateTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: "stat_t":"[TOPIC]",
        size += topicLength + 12; // 12 - length of the JSON decorators for this field
    }

    return size - 1; // skip extra comma from the last field
}

bool HASwitch::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    mqttWriteBeginningJson();

    // command topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            componentName(),
            name(),
            CommandTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char cmdTopic[topicSize];
        generateTopic(
            cmdTopic,
            componentName(),
            name(),
            CommandTopic
        );

        if (strlen(cmdTopic) == 0) {
            return false;
        }

        static const char Prefix[] PROGMEM = {"\"cmd_t\":\""};
        mqttWriteConstCharField(Prefix, cmdTopic);
    }

    // state topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            componentName(),
            name(),
            StateTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char stateTopic[topicSize];
        generateTopic(
            stateTopic,
            componentName(),
            name(),
            StateTopic
        );

        if (strlen(stateTopic) == 0) {
            return false;
        }

        static const char Prefix[] PROGMEM = {",\"stat_t\":\""};
        mqttWriteConstCharField(Prefix, stateTopic);
    }

    mqttWriteNameField();
    mqttWriteUniqueIdField();
    mqttWriteAvailabilityField();
    mqttWriteDeviceField(serializedDevice);
    mqttWriteEndJson();

    return true;
}
