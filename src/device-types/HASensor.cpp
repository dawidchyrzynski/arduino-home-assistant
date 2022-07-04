#include "HASensor.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HASensor::HASensor(const char* uniqueId) :
    HABaseDeviceType("sensor", uniqueId),
    _deviceClass(nullptr),
    _forceUpdate(false),
    _icon(nullptr),
    _unitOfMeasurement(nullptr),
    _valueTemplate(nullptr)
{

}

bool HASensor::setValue(const char* value)
{
    return publishOnDataTopic(HAStateTopic, value, true);
}

void HASensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 10); // 10 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HADeviceClassProperty, _deviceClass);
    _serializer->set(HAIconProperty, _icon);
    _serializer->set(HAUnitOfMeasurementProperty, _unitOfMeasurement);
    _serializer->set(HAValueTemplateProperty, _valueTemplate);

    // optional property
    if (_forceUpdate) {
        _serializer->set(
            HAForceUpdateProperty,
            &_forceUpdate,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
}

void HASensor::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
}

#endif
