#ifndef HAMQTT_H
#define HAMQTT_H

class IPAddress;
class Client;
class MQTTClient;
class HADevice;

class HAMqtt
{
public:
    static const char* DefaultDiscoveryPrefix;
    static const uint16_t ReconnectInterval = 3000; // ms

    HAMqtt(Client& netClient);
    HAMqtt(Client& netClient, HADevice& device);

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
        const uint16_t& serverPort,
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
     * Sets prefix for Home Assistant discovery.
     * It needs to match prefix set in the HA admin panel.
     * The default prefix is "homeassistant".
     */
    inline void setDiscoveryPrefix(const char* prefix)
        { _discoveryPrefix = prefix; }

private:
    void connectToServer();
    String generateClientId() const;

    Client& _netClient;
    HADevice& _device;
    bool _hasDevice;
    bool _initialized;
    const char* _discoveryPrefix;
    MQTTClient* _mqtt;
    IPAddress* _serverIp;
    uint16_t _serverPort;
    const char* _username;
    const char* _password;
    bool _connecting;
    uint32_t _randomNumber;
    uint32_t _lastConnectionAttemptAt;
};

#endif
