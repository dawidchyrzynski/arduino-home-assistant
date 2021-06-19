#include "HALight.h"
#ifdef ARDUINOHA_LIGHT

#include "../HAMqtt.h"
#include "../HADevice.h"

HALight::HALight(const char* uniqueId, uint8_t features) :
    BaseDeviceType("light", uniqueId),
    _features(features),
    _currentState(false),
    _stateCallback(nullptr),
    _retain(false)
{

}

void HALight::onMqttConnected()
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
    }
}

void HALight::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    (void)payload;

    if (compareTopics(topic, DeviceTypeSerializer::CommandTopic)) {
        bool state = (length == strlen(DeviceTypeSerializer::StateOn));
        setState(state, true);
    }
}

bool HALight::setState(bool state, bool force)
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


bool HALight::publishState(bool state)
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


uint16_t HALight::calculateSerializedLength(const char* serializedDevice) const
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

    return size; // excludes null terminator
}

bool HALight::writeSerializedData(const char* serializedDevice) const
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


    DeviceTypeSerializer::mqttWriteNameField(getName());
    DeviceTypeSerializer::mqttWriteUniqueIdField(uniqueId());
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteRetainField(_retain);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
