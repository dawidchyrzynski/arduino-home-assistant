#include "HATriggers.h"
#ifdef ARDUINOHA_TRIGGERS

#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

HATriggers::HATriggers() :
    BaseDeviceType("device_automation", nullptr),
    _triggers(nullptr),
    _triggersNb(0)
{

}

HATriggers::HATriggers(HAMqtt& mqtt) :
    HATriggers()
{
    (void)mqtt;
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

    HATrigger* triggers = (HATrigger*)realloc(_triggers, sizeof(HATrigger) * (_triggersNb + 1));
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
        componentName(),
        trigger,
        DeviceTypeSerializer::EventTopic
    );

    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    generateTopic(
        topic,
        componentName(),
        trigger,
        DeviceTypeSerializer::EventTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Triggering HATrigger: "));
    Serial.print(type);
    Serial.print(F(" "));
    Serial.print(subtype);
    Serial.println();
#endif

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

        const uint16_t& topicLength = calculateTopicLength(
            componentName(),
            trigger,
            DeviceTypeSerializer::ConfigTopic,
            true,
            true
        );
        const uint16_t& dataLength = calculateSerializedLength(
            trigger,
            serializedDevice
        );
        if (topicLength == 0 || dataLength == 0) {
            continue;
        }

        char topic[topicLength];
        generateTopic(
            topic,
            componentName(),
            trigger,
            DeviceTypeSerializer::ConfigTopic,
            true
        );

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
    const HATrigger *trigger,
    const char* suffix,
    bool includeNullTerminator,
    bool isDiscoveryTopic
) const
{
    uint8_t length = strlen(trigger->type) + strlen(trigger->subtype) + 2; // underscore and slash
    return DeviceTypeSerializer::calculateTopicLength(
        component,
        nullptr,
        suffix,
        includeNullTerminator,
        isDiscoveryTopic
    ) + length;
}

uint16_t HATriggers::generateTopic(
    char* output,
    const char* component,
    const HATrigger *trigger,
    const char* suffix,
    bool isDiscoveryTopic
) const
{
    static const char Underscore[] PROGMEM = {"_"};
    uint8_t length = strlen(trigger->type) + strlen(trigger->subtype) + 2; // underscore + null terminator
    char objectId[length];

    strcpy(objectId, trigger->subtype);
    strcat_P(objectId, Underscore);
    strcat(objectId, trigger->type);

    return DeviceTypeSerializer::generateTopic(
        output,
        component,
        objectId,
        suffix,
        isDiscoveryTopic
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

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);

    // automation type
    {
        // Format: "atype":"trigger"
        size += 17;
    }

    // topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            componentName(),
            trigger,
            DeviceTypeSerializer::EventTopic,
            false
        );
        if (topicSize == 0) {
            return 0;
        }

        // Format: ,"t":"[TOPIC]"
        size += topicSize + 7; // 7 - length of the JSON decorators for this field
    }

    // type
    {
        // Format: ,"type":"[TYPE]"
        size += strlen(trigger->type) + 10; // 10 - length of the JSON decorators for this field
    }

    // subtype
    {
        // Format: ,"stype":"[SUBTYPE]"
        size += strlen(trigger->subtype) + 11; // 11 - length of the JSON decorators for this field;
    }

    return size; // exludes null terminator
}

bool HATriggers::writeSerializedTrigger(
    const HATrigger* trigger,
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // automation type
    {
        static const char Data[] PROGMEM = {"\"atype\":\"trigger\""};
        mqtt()->writePayload_P(Data);
    }

    // topic
    {
        const uint16_t& topicSize = calculateTopicLength(
            componentName(),
            trigger,
            DeviceTypeSerializer::EventTopic
        );
        if (topicSize == 0) {
            return false;
        }

        char topic[topicSize];
        generateTopic(
            topic,
            componentName(),
            trigger,
            DeviceTypeSerializer::EventTopic
        );

        static const char Prefix[] PROGMEM = {",\"t\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(
            Prefix,
            topic
        );
    }

    // type
    {
        static const char Prefix[] PROGMEM = {",\"type\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(Prefix, trigger->type);
    }

    // subtype
    {
        static const char Prefix[] PROGMEM = {",\"stype\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(Prefix, trigger->subtype);
    }

    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
