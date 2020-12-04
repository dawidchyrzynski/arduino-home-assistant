#ifndef NO_HA_TAG_SCANNER

#include "HATagScanner.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

// todo: move all variables to progmem
static const char* HAComponentName = "tag";

HATagScanner::HATagScanner(const char* name, HAMqtt& mqtt) :
    BaseDeviceType(mqtt),
    _name(name)
{

}

HATagScanner::~HATagScanner()
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
    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        _name,
        EventTopic
    );
    char topic[topicSize];
    generateTopic(
        topic,
        HAComponentName,
        _name,
        EventTopic
    );

    return mqtt()->publish(topic, tag);
}

void HATagScanner::publishConfig()
{
    const HADevice* device = mqtt()->getDevice();
    const uint16_t& deviceLength = device->calculateSerializedLength();
    char serializedDevice[deviceLength];
    device->serialize(serializedDevice);

    const uint16_t& topicLength = calculateTopicLength(HAComponentName, _name, ConfigTopic);
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    char topic[topicLength];
    generateTopic(topic, HAComponentName, _name, ConfigTopic);

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedTrigger(serializedDevice);
        mqtt()->endPublish();
    }
}

uint16_t HATagScanner::calculateSerializedLength(
    const char* serializedDevice
) const
{
    const uint16_t& eventTopicLength = calculateTopicLength(
        HAComponentName,
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
    static const char QuotationSign[] PROGMEM = {"\""};

    // command topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            _name,
            EventTopic
        );
        char topic[topicSize];
        generateTopic(
            topic,
            HAComponentName,
            _name,
            EventTopic
        );

        static const char DataBefore[] PROGMEM = {"{\"t\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(topic, strlen(topic));
        mqtt()->writePayload_P(QuotationSign);
    }

    // device
    if (serializedDevice != nullptr) {
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

#endif
