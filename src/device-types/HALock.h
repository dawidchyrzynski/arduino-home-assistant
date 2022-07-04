#ifndef AHA_HALOCK_H
#define AHA_HALOCK_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_LOCK

#define HALOCK_CALLBACK(name) void (*name)(LockCommand command, HALock* sender)

class HALock : public HABaseDeviceType
{
public:
    enum LockState {
        StateUnknown = 0,
        StateLocked,
        StateUnlocked
    };

    enum LockCommand {
        CommandLock = 1,
        CommandUnlock,
        CommandOpen
    };

    HALock(const char* uniqueId);

    /**
     * Sets icon of the lock.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the lock's command.
     * If set to true the command produced by Home Assistant will be retained.
     * 
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Changes state of the lock and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the lock.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const LockState state, const bool force = false);

    /**
     * Sets current state of the lock without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the lock.
     */
    inline void setCurrentState(const LockState state)
        { _currentState = state; }

    /**
     * Returns last known state of the lock.
     * If setState method wasn't called the initial value will be returned.
     */
    inline LockState getCurrentState() const
        { return _currentState; }

    /**
     * Registers callback that will be called each time the lock/unlock/open command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same lock.
     *
     * @param callback
     */
    inline void onCommand(HALOCK_CALLBACK(callback))
        { _commandCallback = callback; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    bool publishState(const LockState state);
    void handleCommand(const char* cmd);

    const char* _icon;
    bool _retain;
    LockState _currentState;
    HALOCK_CALLBACK(_commandCallback);
};

#endif
#endif
