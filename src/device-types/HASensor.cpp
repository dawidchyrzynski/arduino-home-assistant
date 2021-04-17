#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include "HASensor.h"
#ifdef ARDUINOHA_SENSOR

#include "../HAMqtt.h"
#include "../HADevice.h"

HASensor::HASensor(const char* uniqueId) :
    BaseDeviceType("sensor", uniqueId),
    _class(nullptr),
    _units(nullptr),
    _icon(nullptr)
{

}

HASensor::HASensor(const char* uniqueId, HAMqtt& mqtt) :
    HASensor(uniqueId)
{
    (void)mqtt;
}

void HASensor::onMqttConnected()
{
    if (strlen(uniqueId()) == 0) {
        return;
    }

    publishConfig();
    publishAvailability();
}

bool HASensor::setValue(const char* value)
{
    return publishValue(value);
}

bool HASensor::setValue(uint32_t value)
{
    uint8_t digitsNb = floor(log10(value)) + 1;
    char str[digitsNb + 1]; // + null terminator
    memset(str, 0, sizeof(str));
    itoa(value, str, 10);

    return publishValue(str);
}

bool HASensor::setValue(int32_t value)
{
    uint8_t digitsNb = floor(log10(value)) + 1;
    char str[digitsNb + 2]; // + null terminator and minus sign
    memset(str, 0, sizeof(str));
    itoa(value, str, 10);

    return publishValue(str);
}

bool HASensor::setValue(double value, uint8_t precision)
{
    uint8_t digitsNb = floor(log10(floor(value))) + 1;
    char str[digitsNb + 3 + precision]; // null terminator, dot, minus sign
    dtostrf(value, 0, precision, str);

    return publishValue(str);
}

bool HASensor::setValue(float value, uint8_t precision)
{
    uint8_t digitsNb = floor(log10(floor(value))) + 1;
    char str[digitsNb + 3 + precision]; // null terminator, dot, minus sign
    dtostrf(value, 0, precision, str);

    return publishValue(str);
}

bool HASensor::publishValue(const char* value)
{
    if (strlen(uniqueId()) == 0 || value == nullptr) {
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

uint16_t HASensor::calculateSerializedLength(
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

    {
        const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
            componentName(),
            uniqueId(),
            DeviceTypeSerializer::StateTopic,
            false
        );

        if (topicSize == 0) {
            return 0;
        }

        // Format: "stat_t":"[TOPIC]"
        size += topicSize + 11; // 11 - length of the JSON decorators for this field
    }

    // device class
    if (_class != nullptr) {
        // Field format: ,"dev_cla":"[CLASS]"
        size += strlen(_class) + 13; // 13 - length of the JSON decorators for this field
    }

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

bool HASensor::writeSerializedData(const char* serializedDevice) const
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
