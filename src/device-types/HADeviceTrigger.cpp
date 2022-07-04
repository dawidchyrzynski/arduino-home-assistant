#include "HADeviceTrigger.h"
#ifndef EX_ARDUINOHA_DEVICE_TRIGGER

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HADeviceTrigger::HADeviceTrigger(const char* type, const char* subtype) :
    HABaseDeviceType("device_automation", nullptr),
    _type(type),
    _subtype(subtype),
    _isProgmemType(false),
    _isProgmemSubtype(false)
{
    buildUniqueId();
}

HADeviceTrigger::HADeviceTrigger(TriggerType type, const char* subtype) :
    HABaseDeviceType("device_automation", nullptr),
    _type(determineProgmemType(type)),
    _subtype(subtype),
    _isProgmemType(true),
    _isProgmemSubtype(false)
{
    buildUniqueId();
}

HADeviceTrigger::HADeviceTrigger(const char* type, TriggerSubtype subtype) :
    HABaseDeviceType("device_automation", nullptr),
    _type(type),
    _subtype(determineProgmemSubtype(subtype)),
    _isProgmemType(false),
    _isProgmemSubtype(true)
{
    buildUniqueId();
}

HADeviceTrigger::HADeviceTrigger(TriggerType type, TriggerSubtype subtype) :
    HABaseDeviceType("device_automation", nullptr),
    _type(determineProgmemType(type)),
    _subtype(determineProgmemSubtype(subtype)),
    _isProgmemType(true),
    _isProgmemSubtype(true)
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

    const uint16_t typeSize = _isProgmemType ? strlen_P(_type) : strlen(_type);
    const uint16_t subtypeSize = _isProgmemSubtype
        ? strlen_P(_subtype)
        : strlen(_subtype);

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

    if (_isProgmemType) {
        strcpy_P(id, _type);
    } else {
        strcpy(id, _type);
    }

    strcat(id, HASerializerUnderscore);

    if (_isProgmemSubtype) {
        strcat_P(id, _subtype);
    } else {
        strcat(id, _subtype);
    }

    _uniqueId = id;
}

const char* HADeviceTrigger::determineProgmemType(TriggerType type) const
{
    switch (type) {
    case ButtonShortPressType:
        return HAButtonShortPressType;

    case ButtonShortReleaseType:
        return HAButtonShortReleaseType;

    case ButtonLongPressType:
        return HAButtonLongPressType;

    case ButtonLongReleaseType:
        return HAButtonLongReleaseType;

    case ButtonDoublePressType:
        return HAButtonDoublePressType;

    case ButtonTriplePressType:
        return HAButtonTriplePressType;

    case ButtonQuadruplePressType:
        return HAButtonQuadruplePressType;

    case ButtonQuintuplePressType:
        return HAButtonQuintuplePressType;

    default:
        return nullptr;
    }
}

const char* HADeviceTrigger::determineProgmemSubtype(
    TriggerSubtype subtype
) const
{
    switch (subtype) {
    case TurnOnSubtype:
        return HATurnOnSubtype;

    case TurnOffSubtype:
        return HATurnOffSubtype;

    case Button1Subtype:
        return HAButton1Subtype;

    case Button2Subtype:
        return HAButton2Subtype;

    case Button3Subtype:
        return HAButton3Subtype;

    case Button4Subtype:
        return HAButton4Subtype;

    case Button5Subtype:
        return HAButton5Subtype;

    case Button6Subtype:
        return HAButton6Subtype;

    default:
        return nullptr;
    }
}

#endif
