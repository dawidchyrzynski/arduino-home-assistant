#include "HATriggers.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

static const char* HAComponentName = "device_automation"; // todo: move to progmem

HATriggers::HATriggers(HAMqtt& mqtt) :
    BaseDeviceType(mqtt),
    _triggers(nullptr),
    _triggersNb(0)
{

}

HATriggers::~HATriggers()
{
    if (_triggers != nullptr) {
        free(_triggers);
    }
}

void HATriggers::onMqttConnected()
{
    publishConfig();
}

bool HATriggers::add(const char* type, const char* subtype)
{
    if (mqtt()->getDevice() == nullptr) {
        return false;
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
    HATrigger* trigger = nullptr;
    for (uint8_t i = 0; i < _triggersNb; i++) {
        if (strcmp(_triggers[i].type, type) == 0 &&
            strcmp(_triggers[i].subtype, subtype) == 0) {
            trigger = &_triggers[i];
            break;
        }
    }

    if (trigger == nullptr) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        HAComponentName,
        trigger,
        EventTopic
    );

    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(topic, HAComponentName, trigger, EventTopic);

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(topic, "");
}

void HATriggers::publishConfig()
{
    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return; // device is required for triggers
    }

    const uint16_t& deviceLength = device->calculateSerializedLength();
    if (deviceLength == 0) {
        return;
    }

    char serializedDevice[deviceLength];
    if (device->serialize(serializedDevice) == 0) {
        return;
    }

    for (uint8_t i = 0; i < _triggersNb; i++) {
        const HATrigger* trigger = &_triggers[i];
        if (trigger == nullptr) {
            continue;
        }

        const uint16_t& topicLength = calculateTopicLength(HAComponentName, trigger, ConfigTopic);
        const uint16_t& dataLength = calculateSerializedLength(trigger, serializedDevice);

        if (topicLength == 0 || dataLength == 0) {
            continue;
        }

        char topic[topicLength];
        generateTopic(topic, HAComponentName, trigger, ConfigTopic);

        if (strlen(topic) == 0) {
            continue;
        }

        if (mqtt()->beginPublish(topic, dataLength, true)) {
            writeSerializedTrigger(trigger, serializedDevice);
            mqtt()->endPublish();
        }
    }
}

uint16_t HATriggers::calculateTopicLength(
    const char* component,
    HATrigger *trigger,
    const char* suffix,
    bool includeNullTerminator
) const
{
    uint8_t length = strlen(trigger->type) + strlen(trigger->subtype) + 1; // + underscore
    return BaseDeviceType::calculateTopicLength(
        component,
        nullptr,
        suffix,
        includeNullTerminator
    ) + length;
}

uint16_t HATriggers::generateTopic(
    char* output,
    const char* component,
    HATrigger *trigger,
    const char* suffix
) const
{
    static const char Underscore[] PROGMEM = {"_"};
    uint8_t length = strlen(trigger->type) + strlen(trigger->subtype) + 2; // slash + null terminator
    char objectId[length];

    strcpy(objectId, trigger->subtype);
    strcat_P(objectId, Underscore);
    strcat(objectId, trigger->type);

    return BaseDeviceType::generateTopic(
        output,
        component,
        objectId,
        suffix
    );
}

uint16_t HATriggers::calculateSerializedLength(
    const HATrigger* trigger,
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    const uint16_t& topicLength = calculateTopicLength(
        HAComponentName,
        trigger,
        EventTopic,
        false
    );
    if (topicLength == 0) {
        return 0;
    }

    uint16_t size =
        2 + // opening and closing bracket (without null terminator)
        17 + // automation type
        topicLength + 7 + // 7 - length of the JSON data for this field
        strlen(trigger->type) + 10 + // 10 - length of the JSON data for this field
        strlen(trigger->subtype) + 11; // 11 - length of the JSON data for this field

    if (serializedDevice != nullptr) {
        size += strlen(serializedDevice) + 7; // 7 - length of the JSON data for this field
    }

    return size;
}

bool HATriggers::writeSerializedTrigger(
    const HATrigger* trigger,
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    static const char QuotationSign[] PROGMEM = {"\""};

    // automation type
    {
        static const char Data[] PROGMEM = {"{\"atype\":\"trigger\""};
        mqtt()->writePayload_P(Data);
    }

    // topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            HAComponentName,
            trigger,
            EventTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char eventTopic[topicSize];
        generateTopic(
            eventTopic,
            HAComponentName,
            trigger,
            EventTopic
        );

        if (strlen(eventTopic) == 0) {
            return false;
        }

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
