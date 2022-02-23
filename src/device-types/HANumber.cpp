#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include "HANumber.h"
#ifdef ARDUINOHA_NUMBER

#include "../HAMqtt.h"
#include "../HADevice.h"

HANumber::HANumber(const char* uniqueId) :
    BaseDeviceType("number", uniqueId),
    _units(nullptr),
    _icon(nullptr),
    _valueCallback(nullptr),
    _retain(false),
    _value(0),
    _precision(0),
    _step(1)
{

}

HANumber::HANumber(const char* uniqueId, HAMqtt& mqtt) :
    HANumber(uniqueId)
{
    (void)mqtt;
}

void HANumber::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
        return;
    }

    publishConfig();
    publishAvailability();

     if (!_retain) {
        publishValue(_value);
    }


    DeviceTypeSerializer::mqttSubscribeTopic(
        this,
        DeviceTypeSerializer::CommandTopic
    );

}

void HANumber::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t& length
)
{
    if (compareTopics(topic, DeviceTypeSerializer::CommandTopic)) {
        char valueStr[length + 1];
        memcpy(valueStr, payload, length);
        valueStr[length] = 0;
        float floatVal = atof(valueStr);  
        setValue(floatVal, true);
    }
}


bool HANumber::setValue(float value, bool force)
{
    if (!force && _value == value) {
        return true;
    }

    if (publishValue(value)) {
        _value = value;

        if (_valueCallback) {
            _valueCallback(value, this);
        }

        return true;
    }

    return false;

}

bool HANumber::publishValue(float fValue)
{
    uint8_t digitsNb = floor(log10(floor(fValue))) + 1;
    char value[digitsNb + 3 + _precision]; // null terminator, dot, minus sign
    dtostrf(fValue, 0, _precision, value);

    if (strlen(uniqueId()) == 0 ) {
        return false;
    }


    if (!mqtt()->isConnected()) {
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

    return mqtt()->publish(topic, value, true);
}

uint16_t HANumber::calculateSerializedLength(
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

      // cmd topic
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

    // Field format: ,"step":"[STEP]"   
    size +=  (_step > 1 ? floor(log10(_step)) : 1) + _precision + (_precision > 0 ? 1 : 0)  + 10;

    // units of measurement
    if (_units != nullptr) {
        // Format: ,"unit_of_meas":"[UNITS]"
        size += strlen(_units) + 18; // 18 - length of the JSON decorators for this field
    }

    // icon
    if (_icon != nullptr) {
        // Field format: ,"ic":"[ICON]"
        size += strlen(_icon) + 8; // 8 - length of the JSON decorators for this field
    }

    return size; // exludes null terminator
}

bool HANumber::writeSerializedData(const char* serializedDevice) const
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

    {

        uint8_t digits = (_step > 1 ? floor(log10(_step)) : 1) + _precision + (_precision > 0 ? 1 : 0) ;
        char str[digits + 1]; // null terminator, dot, minus sign
        dtostrf(_step, 0, _precision, str);

        static const char Prefix[] PROGMEM = {",\"step\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(Prefix, str);
    }

    // units of measurement
    if (_units != nullptr) {
        static const char Prefix[] PROGMEM = {",\"unit_of_meas\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(Prefix, _units);
    }

    // icon
    if (_icon != nullptr) {
        static const char Prefix[] PROGMEM = {",\"ic\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            _icon
        );
    }

    DeviceTypeSerializer::mqttWriteNameField(getName());
    DeviceTypeSerializer::mqttWriteUniqueIdField(uniqueId());
    DeviceTypeSerializer::mqttWriteAvailabilityField(this);
    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
