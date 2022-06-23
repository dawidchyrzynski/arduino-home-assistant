#ifndef AHA_HASENSORINTEGER_H
#define AHA_HASENSORINTEGER_H

#include "HASensor.h"

#ifndef EX_ARDUINOHA_SENSOR

class HASensorInteger : public HASensor
{
public:
    HASensorInteger(const char* uniqueId);
    bool setValue(const int32_t value, const bool force = false);

    inline void setCurrentValue(const int32_t value)
        { _currentValue = value; }

    inline int32_t getCurrentValue() const
        { return _currentValue; }

protected:
    virtual void onMqttConnected() override;

private:
    bool publishValue(const int32_t value);

    int32_t _currentValue;
};

#endif
#endif