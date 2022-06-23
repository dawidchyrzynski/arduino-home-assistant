#include "HASensorInteger.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../HAUtils.h"
#include "../utils/HASerializer.h"

HASensorInteger::HASensorInteger(const char* uniqueId) :
    HASensor(uniqueId),
    _currentValue(0)
{

}

bool HASensorInteger::setValue(const int32_t value, const bool force)
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

void HASensorInteger::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    HASensor::onMqttConnected();
    publishValue(_currentValue);
}

bool HASensorInteger::publishValue(const int32_t value)
{
    uint8_t size = HAUtils::calculateNumberSize(value);
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    memset(str, 0, sizeof(str));
    HAUtils::numberToStr(str, value);

    return publishOnDataTopic(
        HAStateTopic,
        str,
        true
    );
}

#endif