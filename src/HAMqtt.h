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

    bool begin(
        const IPAddress& serverIp,
        const uint16_t& serverPort,
        const char* username = nullptr,
        const char* password = nullptr
    );
    void loop();

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
