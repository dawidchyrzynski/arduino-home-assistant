#ifndef AHA_HATAGSCANNER_H
#define AHA_HATAGSCANNER_H

#include "BaseDeviceType.h"

class HATagScanner : public BaseDeviceType
{
public:
    /**
     * Initializes tag scanner with the given name.
     *
     * @param name Name of the scanner. Recommendes characters: [a-z0-9\-_]
     */
    HATagScanner(const char* name, HAMqtt& mqtt);
    virtual ~HATagScanner();

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Sends "tag scanned" event to the MQTT (Home Assistant).
     * Based on this event HA may perform user-defined automation.
     *
     * @param tag Value of the scanned tag.
     */
    bool tagScanned(const char* tag);

private:
    void publishConfig();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedTrigger(const char* serializedDevice) const;

    const char* _name;
};

#endif
