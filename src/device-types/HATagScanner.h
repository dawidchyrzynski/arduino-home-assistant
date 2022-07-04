#ifndef AHA_HATAGSCANNER_H
#define AHA_HATAGSCANNER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_TAG_SCANNER

class HATagScanner : public HABaseDeviceType
{
public:
    HATagScanner(const char* uniqueId);

    /**
     * Sends "tag scanned" event to the MQTT (Home Assistant).
     * Based on this event HA may perform user-defined automation.
     *
     * @param tag Value of the scanned tag.
     */
    bool tagScanned(const char* tag);

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
};

#endif
#endif
