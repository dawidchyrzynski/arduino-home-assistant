#ifndef AHA_PUBSUBCLIENTMOCK_H
#define AHA_PUBSUBCLIENTMOCK_H

#include "../ArduinoHADefines.h"
#ifdef ARDUINOHA_TEST

#include <Arduino.h>
#include <IPAddress.h>

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

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

    inline const char* getDomain() const
        { return _domain; } 

    inline const IPAddress& getIp() const
        { return _ip; }

    inline const uint16_t& getPort() const
        { return _port; }

    inline const char* getId() const
        { return _id; }

    inline const char* getUser() const
        { return _user; }

    inline const char* getPass() const
        { return _pass; }

    inline const char* willTopic() const
        { return _willTopic; }

    inline const uint8_t& getWillQos() const
        { return _willQos; }

    inline bool isWillRetain() const
        { return _willRetain; }

    inline const char* getWillMessage() const
        { return _willMessage; }

private:
    char _messageBuffer[256];
    const char* _messageTopic;
    bool _messageRetained;
    bool _messageFlushed;
    size_t _messageLength;
    bool _connected;
    const char* _domain;
    IPAddress _ip;
    uint16_t _port;
    const char* _id;
    const char* _user;
    const char* _pass;
    const char* _willTopic;
    uint8_t _willQos;
    bool _willRetain;
    const char* _willMessage;
};

#endif
#endif
