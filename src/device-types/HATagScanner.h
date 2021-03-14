#ifndef AHA_HATAGSCANNER_H
#define AHA_HATAGSCANNER_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_TAG_SCANNER

class HATagScanner : public BaseDeviceType
{
public:
    /**
     * Initializes tag scanner with the given name.
     *
     * @param name Name of the scanner. Recommendes characters: [a-z0-9\-_]
     */
    HATagScanner(const char* name, HAMqtt& mqtt);

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Tag scanner doesn't support availability. Nothing to do here.
     */
    virtual void setAvailability(bool online) override { }

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
    bool writeSerializedData(const char* serializedDevice) const;
};

#endif
#endif
