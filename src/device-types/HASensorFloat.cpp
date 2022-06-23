#include "HASensorFloat.h"
#ifndef EX_ARDUINOHA_SENSOR

#include <math.h>

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

bool HASensorFloat::publishValue(float value)
{
    value *= pow(10, getPrecision(_precision));

    int32_t number = static_cast<int32_t>(value);
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

uint8_t HASensorFloat::getPrecision(const Precision precision) const
{
    switch (precision) {
    case PrecisionP1:
        return 1;

    case PrecisionP2:
        return 2;

    case PrecisionP3:
        return 3;

    case PrecisionP4:
        return 4;

    default:
        return 0;
    }
}

#endif