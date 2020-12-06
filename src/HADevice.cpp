#include <Arduino.h>

#include "HADevice.h"
#include "HAUtils.h"

#define HADEVICE_INIT \
    _manufacturer(nullptr), \
    _model(nullptr), \
    _name(nullptr), \
    _softwareVersion(nullptr)

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
