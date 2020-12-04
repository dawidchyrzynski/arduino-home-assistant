#ifndef HATAGSCANNER_H
#ifndef NO_HA_TAG_SCANNER
#define HATAGSCANNER_H

#include "BaseDeviceType.h"

class HATagScanner : public BaseDeviceType
{
public:
    HATagScanner(const char* name, HAMqtt& mqtt);
    virtual ~HATagScanner();

    virtual void onMqttConnected() override;
    bool tagScanned(const char* tag);

private:
    void publishConfig();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedTrigger(const char* serializedDevice) const;

    const char* _name;
};

#endif
#endif
