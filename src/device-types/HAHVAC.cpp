#include "HAHVAC.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

const char* HAHVAC::ActionTopic = "at";
const char* HAHVAC::AuxCommandTopic = "act";
const char* HAHVAC::AuxStateTopic = "ast";
const char* HAHVAC::AwayCommandTopic = "amct";
const char* HAHVAC::AwayStateTopic = "amst";

HAHVAC::HAHVAC(
    const char* uniqueId,
    HAMqtt& mqtt
) :
    BaseDeviceType("climate", uniqueId),
    _uniqueId(uniqueId),
    _features(0),
    _temperatureUnit(SystemUnit),
    _action(OffAction),
    _auxHeatingCallback(nullptr),
    _auxHeatingState(false),
    _awayCallback(nullptr),
    _awayState(false)
{

}

void HAHVAC::onMqttConnected()
{
    if (strlen(name()) == 0) {
        return;
    }

    publishConfig();
    publishAction(_action);
    publishAuxHeatingState(_auxHeatingState);
    publishAwayState(_awayState);
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    // parse topics:
    // aux_command_topic
    // away_mode_command_topic
    // fan_mode_command_topic
    // mode_command_topic
    // temperature_command_topic
}

bool HAHVAC::setAction(Action action)
{
    if (_action == action) {
        return true;
    }

    if (strlen(name()) == 0) {
        return false;
    }

    if (publishAction(action)) {
        _action = action;
        return true;
    }

    return false;
}

bool HAHVAC::setAuxHeatingState(bool state)
{
    if (!(_features & AuxHeatingFeature) ||
            strlen(name()) == 0) {
        return false;
    }

    if (_auxHeatingState == state) {
        return true;
    }

    if (publishAuxHeatingState(state)) {
        _auxHeatingState = state;

        if (_auxHeatingCallback) {
            _auxHeatingCallback(state);
        }

        return true;
    }

    return false;
}

bool HAHVAC::setAwayState(bool state)
{
    if (!(_features & AwayModeFeature) ||
            strlen(name()) == 0) {
        return;
    }

    if (_awayState == state) {
        return true;
    }

    if (publishAwayState(state)) {
        _awayState = state;

        if (_awayCallback) {
            _awayCallback(state);
        }

        return true;
    }

    return false;
}

void HAHVAC::publishConfig()
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

    const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic
    );
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    DeviceTypeSerializer::generateTopic(
        topic,
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic
    );

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedData(serializedDevice);
        mqtt()->endPublish();
    }
}

bool HAHVAC::publishAction(Action action)
{
    if (strlen(name()) == 0) {
        return false;
    }

    char actionStr[8];
    switch (action) {
        case OffAction:
            strcpy(actionStr, "off");
            break;

        case HeatingAction:
            strcpy(actionStr, "heating");
            break;

        case CoolingAction:
            strcpy(actionStr, "cooling");
            break;

        case DryingAction:
            strcpy(actionStr, "drying");
            break;

        case IdleAction:
            strcpy(actionStr, "idle");
            break;

        case FanAction:
            strcpy(actionStr, "fan");
            break;

        default:
            return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        ActionTopic,
        actionStr
    );
}

bool HAHVAC::publishAuxHeatingState(bool state)
{
    if (!(_features & AuxHeatingFeature) ||
            strlen(name()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        AuxStateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

bool HAHVAC::publishAwayState(bool state)
{
    if (!(_features & AwayModeFeature) ||
            strlen(name()) == 0) {
        return false;
    }

    return DeviceTypeSerializer::mqttPublishMessage(
        this,
        AwayStateTopic,
        (
            state ?
            DeviceTypeSerializer::StateOn :
            DeviceTypeSerializer::StateOff
        )
    );
}

uint16_t HAHVAC::calculateSerializedLength(const char* serializedDevice) const
{
    if (serializedDevice == nullptr || _uniqueId == nullptr) {
        return 0;
    }

    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateUniqueIdFieldSize(device, _uniqueId);
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);
    size += DeviceTypeSerializer::calculateAvailabilityFieldSize(this);

    // action topic
    {
        const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            name(),
            ActionTopic,
            false
        );

        if (topicLength == 0) {
            return 0;
        }

        // Field format: "act_t":"[TOPIC]"
        size += topicLength + 10; // 10 - length of the JSON decorators for this field
    }

    // aux heating
    if (_features & AuxHeatingFeature) {

    }

    // away mode
    if (_features & AwayModeFeature) {

    }

    return size; // exludes null terminator
}

bool HAHVAC::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr || _uniqueId == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // action topic
    {
        static const char Prefix[] PROGMEM = {"\"act_t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            ActionTopic
        );
    }

    DeviceTypeSerializer::mqttWriteUniqueIdField(_uniqueId);
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();
}
