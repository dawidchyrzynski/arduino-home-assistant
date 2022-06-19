#include "HADeviceTrigger.h"
#ifndef EX_ARDUINOHA_DEVICE_TRIGGER

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HADeviceTrigger::HADeviceTrigger(const char* type, const char* subtype) :
    BaseDeviceType("device_automation", nullptr),
    _type(type),
    _subtype(subtype)
{
    buildUniqueId();
}

bool HADeviceTrigger::trigger()
{
    if (!_type || !_subtype) {
        return false;
    }

    return publishOnDataTopic(HATopic, "");
}

void HADeviceTrigger::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 5); // 5 - max properties nb
    _serializer->set(
        HAAutomationTypeProperty,
        HATrigger,
        HASerializer::ProgmemPropertyValue
    );
    _serializer->set(HATypeProperty, _type);
    _serializer->set(HASubtypeProperty, _subtype);
    _serializer->set(HASerializer::WithDevice);
    _serializer->topic(HATopic);
}

void HADeviceTrigger::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
}

uint16_t HADeviceTrigger::calculateIdSize() const
{
    if (!_type || !_subtype) {
        return 0;
    }

    const uint16_t typeSize = strlen(_type);
    const uint16_t subtypeSize = strlen(_subtype);

    // plus underscore separator and null terminator
    return typeSize + subtypeSize + 2;
}

void HADeviceTrigger::buildUniqueId()
{
    const uint16_t idSize = calculateIdSize();
    if (idSize == 0) {
        return;
    }

    char* id = new char[idSize];
    strcpy(id, _type);
    strcat(id, HASerializerUnderscore);
    strcat(id, _subtype);

    _uniqueId = id;
}

#endif
