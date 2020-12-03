#include <Arduino.h>
#include "HATriggers.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

const char* HATriggers::MqttNamespace = "device_automation";

HATriggers::HATriggers(HAMqtt& mqtt) :
    BaseDeviceType(nullptr, mqtt),
    _triggers(nullptr),
    _triggersNb(0)
{
#if defined(ARDUINOHA_DEBUG)
    if (mqtt.getDevice() == nullptr) {
        Serial.println(F("HATriggers requires HADevice to be defined."));
    }
#endif
}

HATriggers::~HATriggers()
{
    free(_triggers);
}

void HATriggers::onMqttConnected()
{
    const HADevice* device = mqtt()->getDevice();
    uint16_t deviceLength = (device == nullptr ? 0 : device->calculateSerializedLength());
    char serializedDevice[deviceLength];

    if (device != nullptr) {
        device->serialize(serializedDevice);
    }

    for (uint8_t i = 0; i < _triggersNb; i++) {
        const HATrigger* trigger = &_triggers[i];
        const uint16_t& topicLength = calculateTopicLength(MqttNamespace, trigger->type, trigger->subtype, ConfigTopic);
        const uint16_t& dataLength = calculateSerializedLength(trigger, serializedDevice);

        char topic[topicLength];
        char data[dataLength];
        generateTopic(topic, MqttNamespace, trigger->type, trigger->subtype, ConfigTopic);

        if (mqtt()->beginPublish(topic, dataLength, true)) {
            writeSerializedTrigger(trigger, serializedDevice);
            mqtt()->endPublish();
        }
    }
}

bool HATriggers::add(const char* type, const char* subtype)
{
    if (mqtt()->getDevice() == nullptr) {
        return false; // device is required for triggers
    }

    HATrigger* triggers = realloc(_triggers, sizeof(HATrigger) * (_triggersNb + 1));
    if (triggers == nullptr) {
        return false;
    }

#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Adding HATrigger: "));
    Serial.print(type);
    Serial.print(F(" "));
    Serial.print(subtype);
    Serial.println();
#endif

    _triggers = triggers;
    _triggers[_triggersNb].type = type;
    _triggers[_triggersNb].subtype = subtype;

    _triggersNb++;
    return true;
}

bool HATriggers::trigger(const char* type, const char* subtype)
{
    bool found = false;
    for (uint8_t i = 0; i < _triggersNb; i++) {
        const HATrigger* trigger = &_triggers[i];
        if (strcmp(trigger->type, type) == 0 &&
            strcmp(trigger->subtype, subtype) == 0) {
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    const uint16_t& size = calculateTopicLength(MqttNamespace, type, subtype, EventTopic);
    char topic[size];

    generateTopic(topic, MqttNamespace, type, subtype, EventTopic);
    mqtt()->publish(topic, "");
}

uint16_t HATriggers::calculateSerializedLength(
    const HATrigger* trigger,
    const char* serializedDevice
) const
{
    const uint16_t& topicLength = calculateTopicLength(
        MqttNamespace,
        trigger->type,
        trigger->subtype,
        EventTopic,
        false
    );
    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        17 + // automation type
        topicLength + 7 +
        strlen(trigger->type) + 10 +
        strlen(trigger->subtype) + 11;

    if (serializedDevice != nullptr) {
        size += strlen(serializedDevice) + 7;
    }

    return size;
}

bool HATriggers::writeSerializedTrigger(
    const HATrigger* trigger,
    const char* serializedDevice
) const
{
    static const char QuotationSign[] PROGMEM = {"\""};

    // automation type
    {
        static const char Data[] PROGMEM = {"{\"atype\":\"trigger\""};
        mqtt()->writePayload_P(Data);
    }

    // topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            MqttNamespace,
            trigger->type,
            trigger->subtype,
            EventTopic
        );
        char eventTopic[topicSize];
        generateTopic(eventTopic,
            MqttNamespace,
            trigger->type,
            trigger->subtype,
            EventTopic
        );

        static const char DataBefore[] PROGMEM = {",\"t\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(eventTopic, strlen(eventTopic));
        mqtt()->writePayload_P(QuotationSign);
    }

    // type
    {
        static const char DataBefore[] PROGMEM = {",\"type\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(trigger->type, strlen(trigger->type));
        mqtt()->writePayload_P(QuotationSign);
    }

    // subtype
    {
        static const char DataBefore[] PROGMEM = {",\"stype\":\""};

        mqtt()->writePayload_P(DataBefore);
        mqtt()->writePayload(trigger->subtype, strlen(trigger->subtype));
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
