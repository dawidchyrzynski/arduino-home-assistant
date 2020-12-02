#include <Arduino.h>

#include "HADevice.h"

HADevice::HADevice(const char* uniqueId) :
    _uniqueId(uniqueId),
    _manufacturer(nullptr),
    _model(nullptr),
    _name(nullptr),
    _softwareVersion(nullptr)
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

uint16_t HADevice::serialize(char* output) const
{
    static const char QuotationSign[] PROGMEM = {"\""};

    {
        static const char DataBefore[] PROGMEM = {"{\"ids\":\""};

        strcpy_P(output, DataBefore);
        strcat(output, _uniqueId);
        strcat_P(output, QuotationSign);
    }

    if (_manufacturer != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"mf\":\""};

        strcat_P(output, DataBefore);
        strcat(output, _manufacturer);
        strcat_P(output, QuotationSign);
    }

    if (_model != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"mdl\":\""};

        strcat_P(output, DataBefore);
        strcat(output, _model);
        strcat_P(output, QuotationSign);
    }

    if (_name != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"name\":\""};

        strcat_P(output, DataBefore);
        strcat(output, _name);
        strcat_P(output, QuotationSign);
    }

    if (_softwareVersion != nullptr) {
        static const char DataBefore[] PROGMEM = {",\"sw\":\""};

        strcat_P(output, DataBefore);
        strcat(output, _softwareVersion);
        strcat_P(output, QuotationSign);
    }

    {
        static const char Data[] PROGMEM = {"}"};
        strcat_P(output, Data);
    }

    return strlen(output) + 1; // size with null terminator
}
