#include "HABinarySensor.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

const char* HABinarySensor::StateTopic = "state";
const char* HABinarySensor::StateOn = "ON";
const char* HABinarySensor::StateOff = "OFF";

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

void HABinarySensor::onMqttConnected()
{
    if (strlen(_name) == 0) {
        return;
    }

    publishConfig();
    publishState(_currentState);
    publishAvailability();
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

bool HABinarySensor::publishState(bool state)
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

uint16_t HABinarySensor::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    uint16_t size = calculateBaseJsonDataSize();
    size += calculateNameFieldSize();
    size += calculateUniqueIdFieldSize();
    size += calculateAvailabilityFieldSize();
    size += calculateDeviceFieldSize(serializedDevice);

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

    // device class
    if (_class != nullptr) {
        // Field format: "dev_cla":"[CLASS]",
        size += strlen(_class) + 13; // 13 - length of the JSON decorators for this field
    }

    return size - 1; // skip extra comma from the last field
}

bool HABinarySensor::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    mqttWriteBeginningJson();

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

        static const char Prefix[] PROGMEM = {"\"stat_t\":\""};
        mqttWriteConstCharField(Prefix, stateTopic);
    }

    // device class
    if (_class != nullptr) {
        static const char Prefix[] PROGMEM = {",\"dev_cla\":\""};
        mqttWriteConstCharField(Prefix, _class);
    }

    mqttWriteNameField();
    mqttWriteUniqueIdField();
    mqttWriteAvailabilityField();
    mqttWriteDeviceField(serializedDevice);
    mqttWriteEndJson();

    return true;
}
