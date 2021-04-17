#include "HABinarySensor.h"
#ifdef ARDUINOHA_BINARY_SENSOR

#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    bool initialState
) :
    BaseDeviceType("binary_sensor", uniqueId),
    _class(nullptr),
    _currentState(initialState)
{

}

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    bool initialState,
    HAMqtt& mqtt
) :
    HABinarySensor(uniqueId, initialState)
{
    (void)mqtt;
}

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    const char* deviceClass,
    bool initialState
) :
    BaseDeviceType("binary_sensor", uniqueId),
    _class(deviceClass),
    _currentState(initialState)
{

}

HABinarySensor::HABinarySensor(
    const char* uniqueId,
    const char* deviceClass,
    bool initialState,
    HAMqtt& mqtt
) :
    HABinarySensor(uniqueId, deviceClass, initialState)
{
    (void)mqtt;
}

void HABinarySensor::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
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

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HABinarySensor::publishState(bool state)
{
    if (strlen(uniqueId()) == 0) {
        return false;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        uniqueId(),
        DeviceTypeSerializer::StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        topic,
        componentName(),
        uniqueId(),
        DeviceTypeSerializer::StateTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(
        topic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        ),
        true
    );
}

uint16_t HABinarySensor::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
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

        // Field format: "stat_t":"[TOPIC]"
        size += topicLength + 11; // 11 - length of the JSON decorators for this field
    }

    // device class
    if (_class != nullptr) {
        // Field format: ,"dev_cla":"[CLASS]"
        size += strlen(_class) + 13; // 13 - length of the JSON decorators for this field
    }

    return size; // exludes null terminator
}

bool HABinarySensor::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // state topic
    {
        static const char Prefix[] PROGMEM = {"\"stat_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            DeviceTypeSerializer::StateTopic
        );
    }

    // device class
    if (_class != nullptr) {
        static const char Prefix[] PROGMEM = {",\"dev_cla\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(Prefix, _class);
    }

    DeviceTypeSerializer::mqttWriteNameField(getName());
    DeviceTypeSerializer::mqttWriteUniqueIdField(uniqueId());
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
