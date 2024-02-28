#include "HASensor.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HASensor::HASensor(const char* uniqueId, const uint16_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentSensor), uniqueId),
    _features(features),
    _deviceClass(nullptr),
    _entityCategory(nullptr),
    _stateClass(nullptr),
    _forceUpdate(false),
    _icon(nullptr),
    _unitOfMeasurement(nullptr),
    _expireAfter()
{

}

bool HASensor::setValue(const char* value)
{
    if (!value) {
        return publishOnDataTopic(AHATOFSTR(HAStateTopic), AHATOFSTR(HAStateNone), true);
    }

    return publishOnDataTopic(AHATOFSTR(HAStateTopic), value, true);
}

bool HASensor::setJsonAttributes(const char* json)
{
    return publishOnDataTopic(AHATOFSTR(HAJsonAttributesTopic), json, true);
}

void HASensor::setExpireAfter(uint16_t expireAfter)
{
    if (expireAfter > 0) {
        _expireAfter.setBaseValue(expireAfter);
    } else {
        _expireAfter.reset();
    }
}

void HASensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 14); // 13 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAObjectIdProperty), _objectId);
    _serializer->set(HASerializer::WithUniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _deviceClass);
    _serializer->set(AHATOFSTR(HAStateClassProperty), _stateClass);
    _serializer->set(AHATOFSTR(HAStateEntityCategory), _entityCategory);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAUnitOfMeasurementProperty), _unitOfMeasurement);

    if (_forceUpdate) {
        _serializer->set(
            AHATOFSTR(HAForceUpdateProperty),
            &_forceUpdate,
            HASerializer::BoolPropertyType
        );
    }

    if (_expireAfter.isSet()) {
        _serializer->set(
            AHATOFSTR(HAExpireAfterProperty),
            &_expireAfter,
            HASerializer::NumberPropertyType
        );
    }

    if (_features & JsonAttributesFeature) {
        _serializer->topic(AHATOFSTR(HAJsonAttributesTopic));
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
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
