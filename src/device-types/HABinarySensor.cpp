#include "HABinarySensor.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

// todo: move all variables to progmem
static const char* StateTopic = "state";
static const char* StateOn = "ON";
static const char* StateOff = "OFF";

HABinarySensor::HABinarySensor(
    const char* name,
    bool initialState,
    HAMqtt& mqtt
) :
    BaseDeviceType(mqtt, "binary_sensor", name),
    _class(nullptr),
    _currentState(initialState)
{

}

HABinarySensor::HABinarySensor(
    const char* name,
    const char* deviceClass,
    bool initialState,
    HAMqtt& mqtt
) :
    BaseDeviceType(mqtt, "binary_sensor", name),
    _class(deviceClass),
    _currentState(initialState)
{

}

HABinarySensor::~HABinarySensor()
{

}

void HABinarySensor::onMqttConnected()
{
    if (strlen(_name) == 0) {
        return;
    }

    publishConfig();
    publishState(_currentState);
}

bool HABinarySensor::setState(bool state)
{
    if (state == _currentState) {
        return true;
    }

    if (strlen(_name) == 0) {
        return false;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HABinarySensor::publishConfig()
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

    const uint16_t& topicLength = calculateTopicLength(_componentName, _name, ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    generateTopic(topic, _componentName, _name, ConfigTopic);

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedTrigger(serializedDevice);
        mqtt()->endPublish();
    }
}

bool HABinarySensor::publishState(bool state)
{
    if (strlen(_name) == 0) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        _componentName,
        _name,
        StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        _componentName,
        _name,
        StateTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(topic, (state ? StateOn : StateOff), true);
}

uint16_t HABinarySensor::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    const uint16_t& stateTopicLength = calculateTopicLength(
        _componentName,
        _name,
        StateTopic,
        false
    );

    if (stateTopicLength == 0) {
        return 0;
    }

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        stateTopicLength + 11 + // 11 - length of the JSON data for this field
        strlen(_name) + 10; // 10 - length of the JSON data for this field

    // unique ID
    size += strlen(mqtt()->getDevice()->getUniqueId());
    size += strlen(_name) + 14; // 14 - length of the JSON data for this field

    // device class
    if (_class != nullptr) {
        size += strlen(_class) + 13; // 13 - length of the JSON data for this field
    }

    // device
    size += strlen(serializedDevice) + 7; // 7 - length of the JSON data for this field

    return size;
}

bool HABinarySensor::writeSerializedTrigger(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    static const char QuotationSign[] PROGMEM = {"\""};

    // state topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            _componentName,
            _name,
            StateTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char stateTopic[topicSize];
        generateTopic(
            stateTopic,
            _componentName,
            _name,
            StateTopic
        );

        if (strlen(stateTopic) == 0) {
            return false;
        }

        static const char DataBefore[] PROGMEM = {"{\"stat_t\":\""};

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

    // device class
    if (_class != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"dev_cla\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(_class, strlen(_class));
        mqtt()->writePayload_P(QuotationSign);
    }

    {
        static const char Data[] PROGMEM = {"}"};
        mqtt()->writePayload_P(Data);
    }

    return true;
}
