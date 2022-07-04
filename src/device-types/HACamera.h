#ifndef AHA_HACAMERA_H
#define AHA_HACAMERA_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_CAMERA

class HACamera : public HABaseDeviceType
{
public:
    enum ImageEncoding {
        EncodingBinary = 1,
        EncodingBase64
    };

    HACamera(const char* uniqueId);

    /**
     * Sets encoding of the image content.
     * 
     * @param encoding
     */
    inline void setEncoding(const ImageEncoding encoding)
        { _encoding = encoding; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Publishes MQTT message with the given data as a message content.
     *
     * @param data Image data (raw binary data or base64)
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool publishImage(const char* data);

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    const char* getEncodingProperty() const;

    ImageEncoding _encoding;
    const char* _icon;
    
};

#endif
#endif
