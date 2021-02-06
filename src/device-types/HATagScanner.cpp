#include "HATagScanner.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

HATagScanner::HATagScanner(const char* name, HAMqtt& mqtt) :
    BaseDeviceType(mqtt, "tag", name)
{

}

void HATagScanner::onMqttConnected()
{
    if (strlen(_name) == 0) {
        return;
    }

    publishConfig();
}

bool HATagScanner::tagScanned(const char* tag)
{
    if (tag == nullptr || strlen(tag) == 0) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        _componentName,
        _name,
        EventTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        _componentName,
        _name,
        EventTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(topic, tag);
}

void HATagScanner::publishConfig()
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

uint16_t HATagScanner::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    const uint16_t& eventTopicLength = calculateTopicLength(
        _componentName,
        _name,
        EventTopic,
        false
    );

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        eventTopicLength + 6 + // 6 - length of the JSON data for this field
        strlen(serializedDevice) + 7; // 7 - length of the JSON data for this field

    return size;
}

bool HATagScanner::writeSerializedTrigger(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    static const char QuotationSign[] PROGMEM = {"\""};

    // topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            _componentName,
            _name,
            EventTopic
        );
        char topic[topicSize];
        generateTopic(
            topic,
            _componentName,
            _name,
            EventTopic
        );

        static const char DataBefore[] PROGMEM = {"{\"t\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(topic, strlen(topic));
        mqtt()->writePayload_P(QuotationSign);
    }

    // device
    {
        static const char Data[] PROGMEM = {",\"dev\":"};

        mqtt()->writePayload_P(Data);
        mqtt()->writePayload(serializedDevice, strlen(serializedDevice));
    }

    {
        static const char Data[] PROGMEM = {"}"};
        mqtt()->writePayload_P(Data);
    }

    return true;
}
