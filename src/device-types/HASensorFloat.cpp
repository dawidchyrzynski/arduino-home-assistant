#include "HASensorFloat.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../HAUtils.h"
#include "../utils/HASerializer.h"

HASensorFloat::HASensorFloat(const char* uniqueId, const Precision precision) :
    HASensor(uniqueId),
    _precision(precision),
    _currentValue(0)
{
    initValueTemplate();
}

bool HASensorFloat::setValue(const float value, const bool force)
{
    if (!force && value == _currentValue) {
        return true;
    }

    if (publishValue(value)) {
        _currentValue = value;
        return true;
    }

    return false;
}

void HASensorFloat::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    HASensor::onMqttConnected();
    publishValue(_currentValue);
}

bool HASensorFloat::publishValue(const float value)
{
    int32_t number = processValue(value);
    uint8_t size = HAUtils::calculateNumberSize(number);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    memset(str, 0, sizeof(str));
    HAUtils::numberToStr(str, number);

    return publishOnDataTopic(
        HAStateTopic,
        str,
        true
    );
}

void HASensorFloat::initValueTemplate()
{
    switch (_precision) {
    case PrecisionP1:
        setValueTemplate(HAValueTemplateFloatP1);
        break;

    case PrecisionP2:
        setValueTemplate(HAValueTemplateFloatP2);
        break;

    case PrecisionP3:
        setValueTemplate(HAValueTemplateFloatP3);
        break;

    case PrecisionP4:
        setValueTemplate(HAValueTemplateFloatP4);
        break;

    default:
        break;
    }
}

int32_t HASensorFloat::processValue(const float value) const
{
    // using pow() increases flash size by ~1k
    switch (_precision) {
    case PrecisionP1:
        return static_cast<int32_t>(value * 10);

    case PrecisionP2:
        return static_cast<int32_t>(value * 100);

    case PrecisionP3:
        return static_cast<int32_t>(value * 1000);

    case PrecisionP4:
        return static_cast<int32_t>(value * 10000);

    default:
        return static_cast<int32_t>(value);
    }
}

#endif