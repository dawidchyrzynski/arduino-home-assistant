#include "HACover.h"
#ifdef ARDUINOHA_COVER

#include "../HAMqtt.h"
#include "../HADevice.h"

static const char ClosedStr[] PROGMEM = {"closed"};
static const char ClosingStr[] PROGMEM = {"closing"};
static const char OpenStr[] PROGMEM = {"open"};
static const char OpeningStr[] PROGMEM = {"opening"};
static const char StoppedStr[] PROGMEM = {"stopped"};

HACover::HACover(const char* uniqueId) :
    BaseDeviceType("cover", uniqueId),
    _currentState(StateUnknown),
    _label(nullptr),
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
    if (strlen(name()) == 0) {
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

void HACover::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    (void)payload;

    if (isMyTopic(topic, DeviceTypeSerializer::CommandTopic)) {
        Serial.println((const char*)payload);
        // to do
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

bool HACover::publishState(CoverState state)
{
    if (strlen(name()) == 0) {
        return false;
    }

    return false;

    /* return DeviceTypeSerializer::mqttPublishMessage(
        this,
        DeviceTypeSerializer::StateTopic,
        // to do
    ); */
}

uint16_t HACover::calculateSerializedLength(const char* serializedDevice) const
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

    DeviceTypeSerializer::mqttWriteRetainField(_retain);
    DeviceTypeSerializer::mqttWriteNameField(_label);
    DeviceTypeSerializer::mqttWriteUniqueIdField(name());
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
