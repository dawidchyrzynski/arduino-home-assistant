#ifndef AHA_HASENSORFLOAT_H
#define AHA_HASENSORFLOAT_H

#include "HASensor.h"

#ifndef EX_ARDUINOHA_SENSOR

class HASensorFloat : public HASensor
{
public:
    enum Precision {
        PrecisionP0,
        PrecisionP1,
        PrecisionP2,
        PrecisionP3,
        PrecisionP4
    };

    HASensorFloat(
        const char* uniqueId,
        const Precision precision = PrecisionP2
    );
    bool setValue(const float value, const bool force = false);

    inline void setCurrentValue(const float value)
        { _currentValue = value; }

    inline float getCurrentValue() const
        { return _currentValue; }

private:
    bool publishValue(float value);
    void initValueTemplate();
    uint8_t getPrecision(const Precision precision) const;

    Precision _precision;
    float _currentValue;
};

#endif
#endif