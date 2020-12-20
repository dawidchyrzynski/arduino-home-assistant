#ifndef AHA_HAMQTT_H
#define AHA_HAMQTT_H

#include <Arduino.h>

class IPAddress;
class Client;
class PubSubClient;
class HADevice;
class BaseDeviceType;

class HAMqtt
{
public:
    static const uint16_t ReconnectInterval = 5000; // ms

    HAMqtt(Client& netClient, HADevice& device);
    HAMqtt(const char* clientId, Client& netClient, HADevice& device);

    /**
     * Sets prefix for Home Assistant discovery.
     * It needs to match prefix set in the HA admin panel.
     * The default prefix is "homeassistant".
     */
    inline void setDiscoveryPrefix(const char* prefix)
        { _discoveryPrefix = prefix; }

    /**
     * Returns discovery prefix.
     */
    inline const char* getDiscoveryPrefix() const
        { return _discoveryPrefix; }

    /**
     * Returns instance of the device assigned to the HAMqtt class.
     */
    inline HADevice const* getDevice() const
        { return (_hasDevice ? &_device : nullptr); }

    /**
     * Sets parameters of the connection to the MQTT broker.
     * The library will try to connect to the broker in first loop cycle.
     * Please note that the library automatically reconnects to the broker if connection is lost.
     *
     * @param serverIp IP address of the MQTT broker.
     * @param serverPort Port of the MQTT broker.
     * @param username Username for authentication.
     * @param password Password for authentication.
     */
    bool begin(
        const IPAddress& serverIp,
        const uint16_t& serverPort = 1883,
        const char* username = nullptr,
        const char* password = nullptr
    );

    /**
     * ArduinoHA's ticker.
     */
    void loop();

    /**
     * Returns true if connection to the MQTT broker is established.
     */
    bool isConnected();

    /**
     * Adds a new device's type to the MQTT.
     * Each time the connection with MQTT broker is acquired, the HAMqtt class
     * calls "onMqttConnected" method in all devices' types instances.
     *
     * @param deviceType Instance of the device's type (eg. HATriggers).
     */
    void addDeviceType(BaseDeviceType* deviceType);

    /**
     * Publishes MQTT message with given topic and payload.
     * Message won't be published if connection with MQTT broker is not established.
     * In this case method returns false.
     *
     * @param topic Topic to publish.
     * @param payload Payload to publish (it may be empty const char).
     * @param retained Determines whether message should be retained.
     */
    bool publish(const char* topic, const char* payload, bool retained = false);

    bool beginPublish(const char* topic, uint16_t payloadLength, bool retained = false);
    bool writePayload(const char* data, uint16_t length);
    bool writePayload_P(const char* src);
    bool endPublish();
    bool subscribe(const char* topic);
    void processMessage(char* topic, uint8_t* payload, uint16_t length);

private:
    /**
     * Attempts to connect to the MQTT broker.
     * The method uses properties passed to the "begin" method.
     */
    void connectToServer();

    /**
     * This method is called each time the connection with MQTT broker is acquired.
     */
    void onConnected();

    const char* _clientId;
    Client& _netClient;
    HADevice& _device;
    bool _hasDevice;
    bool _initialized;
    const char* _discoveryPrefix;
    PubSubClient* _mqtt;
    IPAddress* _serverIp;
    uint16_t _serverPort;
    const char* _username;
    const char* _password;
    uint32_t _lastConnectionAttemptAt;
    uint8_t _devicesTypesNb;
    BaseDeviceType** _devicesTypes;
};

#endif
