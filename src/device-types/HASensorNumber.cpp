#include "HASensorNumber.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HASensorNumber::HASensorNumber(
    const char* uniqueId,
    const NumberPrecision precision
) :
    HASensor(uniqueId),
    _precision(precision),
    _currentValue(0)
{

}

bool HASensorNumber::setValue(const HAUtils::Number value, const bool force)
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

void HASensorNumber::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    HASensor::onMqttConnected();
    publishValue(_currentValue);
}

bool HASensorNumber::publishValue(const HAUtils::Number value)
{
    uint8_t size = HAUtils::calculateNumberSize(value, _precision);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    HAUtils::numberToStr(str, value, _precision);

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        str,
        true
    );
}

#endif