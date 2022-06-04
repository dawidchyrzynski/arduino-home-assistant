#ifndef AHA_PUBSUBCLIENTMOCK_H
#define AHA_PUBSUBCLIENTMOCK_H

#ifdef ARDUINOHA_TEST

#include <Arduino.h>
#include <IPAddress.h>

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

struct MqttMessage
{
    const char* topic;
    size_t topicSize;
    const char* buffer;
    size_t bufferSize;
    bool retained;

    MqttMessage() :
        topic(nullptr),
        topicSize(0),
        buffer(nullptr),
        bufferSize(0),
        retained(false)
    {

    }

    ~MqttMessage()
    {
        delete topic;
        delete buffer;
    }
};

struct MqttConnection
{
    bool connected;
    const char* domain;
    IPAddress ip;
    uint16_t port;
    const char* id;
    const char* user;
    const char* pass;

    MqttConnection() :
        connected(false),
        domain(nullptr),
        port(0),
        id(nullptr),
        user(nullptr),
        pass(nullptr)
    {

    }
};

struct MqttWill
{
    const char* topic;
    const char* message;
    bool retain;

    MqttWill() :
        topic(nullptr),
        message(nullptr),
        retain(false)
    {

    }
};

class PubSubClientMock
{
public:
    PubSubClientMock();

    bool loop();
    void disconnect();
    bool connected();
    bool connect(
        const char *id,
        const char *user,
        const char *pass,
        const char* willTopic,
        uint8_t willQos,
        bool willRetain,
        const char* willMessage,
        bool cleanSession
    );
    bool connectDummy();
    PubSubClientMock& setServer(IPAddress ip, uint16_t port);
    PubSubClientMock& setServer(const char* domain, uint16_t port);
    PubSubClientMock& setCallback(MQTT_CALLBACK_SIGNATURE);
    bool beginPublish(const char* topic, unsigned int plength, bool retained);
    size_t write(const uint8_t *buffer, size_t size);
    int endPublish();
    bool subscribe(const char* topic);

    inline const char* getMessageBuffer() const
        { return _messageBuffer; }

    inline const char* getMessageTopic() const
        { return _messageTopic; }

    inline bool isMessageRetained() const
        { return _messageRetained; }

    inline const size_t& getMessageLength() const
        { return _messageLength; }

    inline bool isMessageFlushed() const
        { return _messageFlushed; }

    inline const MqttConnection& getConnection() const
        { return _connection; }

    inline const MqttWill& getLastWill() const
        { return _lastWill; }

private:
    char _messageBuffer[256];
    char _messageTopic[64];
    bool _messageRetained;
    bool _messageFlushed;
    size_t _messageLength;
    
    MqttConnection _connection;
    MqttWill _lastWill;
};

#endif
#endif
