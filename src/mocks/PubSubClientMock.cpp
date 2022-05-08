#include "PubSubClientMock.h"
#ifdef ARDUINOHA_TEST

PubSubClientMock::PubSubClientMock() :
    _messageRetained(false),
    _messageFlushed(false),
    _messageLength(0),
    _connected(false),
    _port(0),
    _id(nullptr),
    _user(nullptr),
    _pass(nullptr),
    _willTopic(nullptr),
    _willQos(0),
    _willRetain(false),
    _willMessage(nullptr)
{
    memset(_messageBuffer, 0, sizeof(_messageBuffer));
    memset(_messageTopic, 0, sizeof(_messageTopic));
}

bool PubSubClientMock::loop()
{
    // nothing to do
}

void PubSubClientMock::disconnect()
{
    _connected = false;
}

bool PubSubClientMock::connected()
{
    return _connected;
}

bool PubSubClientMock::connect(
    const char *id,
    const char *user,
    const char *pass,
    const char* willTopic,
    uint8_t willQos,
    bool willRetain,
    const char* willMessage,
    bool cleanSession
)
{
    _connected = true;
    _id = id;
    _user = user;
    _pass = pass;
    _willTopic = willTopic;
    _willQos = willQos;
    _willRetain = willRetain;
    _willMessage = willMessage;

    return true;
}

bool PubSubClientMock::connectDummy()
{
    _connected = true;
    _id = nullptr;
    _user = nullptr;
    _pass = nullptr;
    _willTopic = nullptr;
    _willQos = 0;
    _willRetain = false;
    _willMessage = nullptr;

    return true;
}

PubSubClientMock& PubSubClientMock::setServer(IPAddress ip, uint16_t port)
{
    _ip = ip;
    _port = port;

    return *this;
}

PubSubClientMock& PubSubClientMock::setServer(const char * domain, uint16_t port)
{
    _domain = domain;
    _port = port;

    return *this;
}

PubSubClientMock& PubSubClientMock::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    return *this;
}

bool PubSubClientMock::beginPublish(const char* topic, unsigned int plength, bool retained)
{
    _messageRetained = retained;
    _messageFlushed = false;
    _messageLength = plength;

    if (strlen(topic) >= sizeof(_messageTopic)) {
        Serial.println("WARNING: Message topic overflow (PubSubClientMock)");
    }

    strcpy(_messageTopic, topic);
    memset(_messageBuffer, 0, sizeof(_messageBuffer));
    return true;
}

size_t PubSubClientMock::write(const uint8_t *buffer, size_t size)
{
    strncat(_messageBuffer, (const char*)buffer, size);
    return size;
}

int PubSubClientMock::endPublish()
{
    _messageFlushed = true;
    return strlen(_messageBuffer);
}

bool PubSubClientMock::subscribe(const char* topic)
{
    return false; // to do
}

#endif
