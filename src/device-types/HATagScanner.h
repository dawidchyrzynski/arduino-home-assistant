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
     * @param uniqueId Unique ID of the scanner. Recommendes characters: [a-z0-9\-_]
     */
    HATagScanner(const char* uniqueId);
    HATagScanner(const char* uniqueId, HAMqtt& mqtt); // legacy constructor

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Tag scanner doesn't support availability. Nothing to do here.
     */
    virtual void setAvailability(bool online) override { (void)online; }

    /**
     * Sends "tag scanned" event to the MQTT (Home Assistant).
     * Based on this event HA may perform user-defined automation.
     *
     * @param tag Value of the scanned tag.
     */
    bool tagScanned(const char* tag);

private:
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;
};

#endif
#endif
