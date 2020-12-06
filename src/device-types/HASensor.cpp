#ifndef NO_HA_SENSOR

#include "HASensor.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

// todo: move all variables to progmem
static const char* HAComponentName = "sensor";
static const char* ValueTopic = "value";

template <typename T>
HASensor<T>::HASensor(
    const char* name,
    T initialValue,
    HAMqtt& mqtt
) :
    BaseDeviceType(mqtt),
    _name(name),
    _class(nullptr),
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
    BaseDeviceType(mqtt),
    _name(name),
    _class(deviceClass),
    _valueType(HAUtils::determineValueType<T>()),
    _currentValue(initialValue)
{

}

template <typename T>
HASensor<T>::~HASensor()
{

}

template <typename T>
void HASensor<T>::onMqttConnected()
{
    if (strlen(_name) == 0 || _valueType == HAUtils::ValueTypeUnknown) {
        return;
    }

    publishConfig();
    publishValue(_currentValue);
}

template <typename T>
bool HASensor<T>::setValue(T value)
{
    if (strlen(_name) == 0 ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
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

    const uint16_t& topicLength = calculateTopicLength(HAComponentName, _name, ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    generateTopic(topic, HAComponentName, _name, ConfigTopic);

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedTrigger(serializedDevice);
        mqtt()->endPublish();
    }
}

template <typename T>
bool HASensor<T>::publishValue(T value)
{
    if (strlen(_name) == 0 ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        _name,
        ValueTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        HAComponentName,
        _name,
        ValueTopic
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

    const uint16_t& valueTopicLength = calculateTopicLength(
        HAComponentName,
        _name,
        ValueTopic,
        false
    );

    if (valueTopicLength == 0) {
        return 0;
    }

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        valueTopicLength + 11 + // 11 - length of the JSON data for this field
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

template <typename T>
bool HASensor<T>::writeSerializedTrigger(const char* serializedDevice) const
{
    if (serializedDevice == nullptr ||
            _valueType == HAUtils::ValueTypeUnknown) {
        return false;
    }

    static const char QuotationSign[] PROGMEM = {"\""};

    // state topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            _name,
            ValueTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char stateTopic[topicSize];
        generateTopic(
            stateTopic,
            HAComponentName,
            _name,
            ValueTopic
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

template <typename T>
uint16_t HASensor<T>::calculateValueLength() const
{
    if (_valueType == HAUtils::ValueTypeUnknown) {
        return 0;
    }

    switch (_valueType) {
        case HAUtils::ValueTypeUint8:
        case HAUtils::ValueTypeUint16:
        case HAUtils::ValueTypeUint32:
        case HAUtils::ValueTypeInt8:
        case HAUtils::ValueTypeInt16:
        case HAUtils::ValueTypeInt32:
            return (HAUtils::getValueTypeLength(_valueType) * 3) + 1; // 3 digits per byte + null terminator

        case HAUtils::ValueTypeDouble:
        case HAUtils::ValueTypeFloat:
            return (HAUtils::getValueTypeLength(_valueType) * 3) + 4; // 3 digits per byte + dot separator + 2 precision digits + null terminator
    }

    return 0;
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
            sprintf(dst, "%d", value);
            break;

        case HAUtils::ValueTypeDouble:
            dtostrf(value, 0, 2, dst);
            break;
    }

    return true;
}

#endif
