#ifndef HASWITCH_H
#ifndef NO_HA_SWITCH
#define HASWITCH_H

#include "BaseDeviceType.h"

#define HASWITCH_CALLBACK void (*callback)(bool, HASwitch*)

class HASwitch : public BaseDeviceType
{
public:
    HASwitch(
        const char* name,
        bool initialState,
        HAMqtt& mqtt
    );
    virtual ~HASwitch();

    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const char* payload,
        const uint16_t& length
    ) override;

    inline const char* getName() const
        { return _name; }

    void setState(bool state);
    void turnOn();
    void turnOff();
    void onStateChanged(HASWITCH_CALLBACK);

private:
    void triggerCallback(bool state);
    void publishConfig();
    void publishCurrentState();
    void subscribeCommandTopic();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedTrigger(const char* serializedDevice) const;

    const char* _name;
    void (*_stateCallback)(bool, HASwitch*);
    bool _currentState;
};

#endif
#endif
