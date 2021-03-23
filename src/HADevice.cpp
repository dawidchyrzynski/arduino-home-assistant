#include <Arduino.h>

#include "ArduinoHADefines.h"
#include "HADevice.h"
#include "HAUtils.h"
#include "HAMqtt.h"
#include "device-types/DeviceTypeSerializer.h"

#define HADEVICE_INIT \
    _manufacturer(nullptr), \
    _model(nullptr), \
    _name(nullptr), \
    _softwareVersion(nullptr), \
    _sharedAvailability(false), \
    _availabilityTopic(nullptr), \
    _available(true) // device will be available by default

HADevice::HADevice() :
    _uniqueId(nullptr),
    HADEVICE_INIT
{

}

HADevice::HADevice(const char* uniqueId) :
    _uniqueId(uniqueId),
    HADEVICE_INIT
{

}

HADevice::HADevice(const byte* uniqueId, const uint16_t& length) :
    _uniqueId(HAUtils::byteArrayToStr(uniqueId, length)),
    HADEVICE_INIT
{

}

void HADevice::setAvailability(bool online)
{
    _available = online;
    publishAvailability();
}

bool HADevice::enableSharedAvailability()
{
    if (_sharedAvailability) {
        return false;
    }

#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("Enabling shared availability in the device"));
#endif

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        nullptr,
        nullptr,
        DeviceTypeSerializer::AvailabilityTopic
    );
    if (topicSize == 0) {
        return false;
    }

    _availabilityTopic = (char*)malloc(topicSize);
    _sharedAvailability = true;

    DeviceTypeSerializer::generateTopic(
        _availabilityTopic,
        nullptr,
        nullptr,
        DeviceTypeSerializer::AvailabilityTopic
    );
    return true;
}

bool HADevice::enableLastWill()
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (mqtt == nullptr || _availabilityTopic == nullptr) {
        return false;
    }

    mqtt->setLastWill(
        _availabilityTopic,
        DeviceTypeSerializer::Offline,
        false
    );
    return true;
}

bool HADevice::setUniqueId(const byte* uniqueId, const uint16_t& length)
{
    if (_uniqueId != nullptr) {
        return false;
    }

    _uniqueId = HAUtils::byteArrayToStr(uniqueId, length);
    return true;
}

void HADevice::publishAvailability()
{
    if (!_sharedAvailability) {
        return;
    }

    HAMqtt* mqtt = HAMqtt::instance();
    if (mqtt == nullptr) {
        return;
    }

    mqtt->publish(
        _availabilityTopic,
        (
            _available ?
            DeviceTypeSerializer::Online :
            DeviceTypeSerializer::Offline
        ),
        true
    );
}

uint16_t HADevice::calculateSerializedLength() const
{
    uint16_t size =
        3 + // opening and closing bracket + null terminator
        strlen(_uniqueId) + 8; // 8 - length of the JSON data for this field

    if (_manufacturer != nullptr) {
        size += strlen(_manufacturer) + 8; // 8 - length of the JSON data for this field
    }

    if (_model != nullptr) {
        size += strlen(_model) + 9; // 9 - length of the JSON data for this field
    }

    if (_name != nullptr) {
        size += strlen(_name) + 10; // 10 - length of the JSON data for this field
    }

    if (_softwareVersion != nullptr) {
        size += strlen(_softwareVersion) + 8; // 8 - length of the JSON data for this field
    }

    return size;
}

uint16_t HADevice::serialize(char* dst) const
{
    static const char QuotationSign[] PROGMEM = {"\""};

    {
        static const char DataBefore[] PROGMEM = {"{\"ids\":\""};

        strcpy_P(dst, DataBefore);
        strcat(dst, _uniqueId);
        strcat_P(dst, QuotationSign);
    }

    if (_manufacturer != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"mf\":\""};

        strcat_P(dst, DataBefore);
        strcat(dst, _manufacturer);
        strcat_P(dst, QuotationSign);
    }

    if (_model != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"mdl\":\""};

        strcat_P(dst, DataBefore);
        strcat(dst, _model);
        strcat_P(dst, QuotationSign);
    }

    if (_name != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"name\":\""};

        strcat_P(dst, DataBefore);
        strcat(dst, _name);
        strcat_P(dst, QuotationSign);
    }

    if (_softwareVersion != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"sw\":\""};

        strcat_P(dst, DataBefore);
        strcat(dst, _softwareVersion);
        strcat_P(dst, QuotationSign);
    }

    {
        static const char Data[] PROGMEM = {"}"};
        strcat_P(dst, Data);
    }

    return strlen(dst) + 1; // size with null terminator
}
