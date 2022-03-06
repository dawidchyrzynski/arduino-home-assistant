#include <Arduino.h>

#include "ArduinoHADefines.h"
#include "HADevice.h"
#include "HAUtils.h"
#include "HAMqtt.h"
#include "utils/HASerializer.h"

#define HADEVICE_INIT \
    _serializer(new HASerializer(nullptr)), \
    _availabilityTopic(nullptr), \
    _sharedAvailability(false), \
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
    _serializer->set(HADeviceIdentifiersProperty, _uniqueId);
}

HADevice::HADevice(const byte* uniqueId, const uint16_t& length) :
    _uniqueId(HAUtils::byteArrayToStr(uniqueId, length)),
    HADEVICE_INIT
{
    _serializer->set(HADeviceIdentifiersProperty, _uniqueId);
}

HADevice::~HADevice()
{
    if (_serializer) {
        delete _serializer;
        _serializer = nullptr;
    }
}

void HADevice::setManufacturer(const char* manufacturer)
{
    _serializer->set(HADeviceManufacturerProperty, manufacturer);
}

void HADevice::setModel(const char* model)
{
    _serializer->set(HADeviceModelProperty, model);
}

void HADevice::setName(const char* name)
{
    _serializer->set(HANameProperty, name);
}

void HADevice::setSoftwareVersion(const char* softwareVersion)
{
    _serializer->set(HADeviceSoftwareVersionProperty, softwareVersion);
}

void HADevice::setAvailability(bool online)
{
    _available = online;
    publishAvailability();
}

bool HADevice::enableSharedAvailability()
{
    if (_sharedAvailability) {
        return true; // already enabled
    }

    const uint16_t topicLength = HASerializer::calculateDataTopicLength(
        nullptr,
        HAAvailabilityTopic
    );
    if (topicLength == 0) {
        return false;
    }

    _availabilityTopic = (char*)malloc(topicLength);

    if (HASerializer::generateDataTopic(
        _availabilityTopic,
        nullptr,
        HAAvailabilityTopic
    ) > 0) {
        _sharedAvailability = true;
        return true;
    }

    return false;
}

bool HADevice::enableLastWill()
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!mqtt || !_availabilityTopic) {
        return false;
    }

    /* mqtt->setLastWill(
        _availabilityTopic,
        DeviceTypeSerializer::Offline,
        false
    ); */
    return true;
}

bool HADevice::setUniqueId(const byte* uniqueId, const uint16_t& length)
{
    if (!_uniqueId) {
        return false; // unique ID cannot be changed at runtime once it's set
    }

    _uniqueId = HAUtils::byteArrayToStr(uniqueId, length);
    _serializer->set(HADeviceIdentifiersProperty, _uniqueId);
    return true;
}

void HADevice::publishAvailability()
{
    if (!_sharedAvailability) {
        return;
    }

    /* HAMqtt* mqtt = HAMqtt::instance();
    if (mqtt) {
        mqtt->publish(
            _availabilityTopic,
            (
                _available ?
                DeviceTypeSerializer::Online :
                DeviceTypeSerializer::Offline
            ),
            true
        );
    } */
}
