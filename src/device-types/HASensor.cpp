#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include "HASensor.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

template <typename T>
HASensor<T>::HASensor(
    const char* name,
    T initialValue,
    HAMqtt& mqtt
) :
    BaseDeviceType(mqtt, "sensor", name),
    _class(nullptr),
    _units(nullptr),
    _valueType(HAUtils::determineValueType<T>()),
    _currentValue(initialValue)
{

}

template <typename T>
HASensor<T>::HASensor(
    const char* name,
    const char* deviceClass,
    T initialValue,
    HAMqtt& mqtt
) :
    BaseDeviceType(mqtt, "sensor", name),
    _class(deviceClass),
    _units(nullptr),
    _valueType(HAUtils::determineValueType<T>()),
    _currentValue(initialValue)
{

}

template <typename T>
void HASensor<T>::onMqttConnected()
{
    if (strlen(name()) == 0 ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return;
    }

    publishConfig();
    publishValue(_currentValue);
    publishAvailability();
}

template <typename T>
bool HASensor<T>::setValue(T value)
{
    if (strlen(name()) == 0 ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    if (_currentValue == value) {
        return true;
    }

    if (publishValue(value)) {
        _currentValue = value;
        return true;
    }

    return false;
}

template <typename T>
void HASensor<T>::publishConfig()
{
    if (_valueType == HAUtils::ValueTypeUnknown) {
        return;
    }

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
        mqtt(),
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
        mqtt(),
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

template <typename T>
bool HASensor<T>::publishValue(T value)
{
    if (strlen(name()) == 0 ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        mqtt(),
        componentName(),
        name(),
        DeviceTypeSerializer::StateTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        mqtt(),
        topic,
        componentName(),
        name(),
        DeviceTypeSerializer::StateTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    const uint16_t& valueLength = calculateValueLength();
    if (valueLength == 0) {
        return false;
    }

    char valueStr[valueLength];
    if (!valueToStr(valueStr, value)) {
        return false;
    }

    return mqtt()->publish(topic, valueStr, true);
}

template <typename T>
uint16_t HASensor<T>::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return 0;
    }

    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateNameFieldSize(name());
    size += DeviceTypeSerializer::calculateUniqueIdFieldSize(device, name());
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);

    if (isAvailabilityConfigured()) {
        size += DeviceTypeSerializer::calculateAvailabilityFieldSize(
            mqtt(),
            componentName(),
            name()
        );
    }

    {
        const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
            mqtt(),
            componentName(),
            name(),
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

    return size; // exludes null terminator
}

template <typename T>
bool HASensor<T>::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson(mqtt());

    // state topic
    {
        const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
            mqtt(),
            componentName(),
            name(),
            DeviceTypeSerializer::StateTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char topic[topicSize];
        DeviceTypeSerializer::generateTopic(
            mqtt(),
            topic,
            componentName(),
            name(),
            DeviceTypeSerializer::StateTopic
        );

        if (strlen(topic) == 0) {
            return false;
        }

        static const char Prefix[] PROGMEM = {"\"stat_t\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(mqtt(), Prefix, topic);
    }

    // device class
    if (_class != nullptr) {
        static const char Prefix[] PROGMEM = {",\"dev_cla\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(mqtt(), Prefix, _class);
    }

    // units of measurement
    if (_units != nullptr) {
        static const char Prefix[] PROGMEM = {",\"unit_of_meas\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(mqtt(), Prefix, _units);
    }

    DeviceTypeSerializer::mqttWriteNameField(mqtt(), name());
    DeviceTypeSerializer::mqttWriteUniqueIdField(mqtt(), name());

    if (isAvailabilityConfigured()) {
        DeviceTypeSerializer::mqttWriteAvailabilityField(
            mqtt(),
            componentName(),
            name()
        );
    }

    DeviceTypeSerializer::mqttWriteDeviceField(mqtt(), serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson(mqtt());

    return true;
}

template <typename T>
uint16_t HASensor<T>::calculateValueLength() const
{
    if (_valueType == HAUtils::ValueTypeUnknown) {
        return 0;
    }

    uint16_t size = 0;

    switch (_valueType) {
        case HAUtils::ValueTypeUint8:
            size = 3; // from 0 to 255
            break;

        case HAUtils::ValueTypeUint16:
            size = 5; // from 0 to 65535
            break;

        case HAUtils::ValueTypeUint32:
            size = 10; // from 0 to 4294967295
            break;

        case HAUtils::ValueTypeInt8:
            size = 4; // from -128 to 127
            break;

        case HAUtils::ValueTypeInt16:
            size = 6; // from -32768 to 32767
            break;

        case HAUtils::ValueTypeInt32:
            size = 11; // from -2147483648 to 2147483647
            break;

        case HAUtils::ValueTypeDouble:
        case HAUtils::ValueTypeFloat:
            // 3 digits per byte + dot separator + 2 precision digits
            return (HAUtils::getValueTypeLength(_valueType) * 3) + 3;
    }

    return (size > 0 ? (size + 1) : 0);
}

template <typename T>
bool HASensor<T>::valueToStr(char* dst, T value) const
{
    if (_valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    switch (_valueType) {
        case HAUtils::ValueTypeUint8:
        case HAUtils::ValueTypeUint16:
        case HAUtils::ValueTypeUint32:
        case HAUtils::ValueTypeInt8:
        case HAUtils::ValueTypeInt16:
        case HAUtils::ValueTypeInt32:
            itoa(value, dst, 10);
            break;

        case HAUtils::ValueTypeDouble:
        case HAUtils::ValueTypeFloat:
            dtostrf(value, 0, 2, dst);
            break;
    }

    return true;
}
