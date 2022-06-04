#include "PubSubClientMock.h"
#ifdef ARDUINOHA_TEST

PubSubClientMock::PubSubClientMock() :
    _messageRetained(false),
    _messageFlushed(false),
    _messageLength(0)
{
    memset(_messageBuffer, 0, sizeof(_messageBuffer));
    memset(_messageTopic, 0, sizeof(_messageTopic));
}

bool PubSubClientMock::loop()
{
    return true; // nothing to do
}

void PubSubClientMock::disconnect()
{
    _connection.connected = false;
}

bool PubSubClientMock::connected()
{
    return _connection.connected;
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
    (void)willQos;
    (void)cleanSession;

    _connection.connected = true;
    _connection.id = id;
    _connection.user = user;
    _connection.pass = pass;

    _lastWill.topic = willTopic;
    _lastWill.message = willMessage;
    _lastWill.retain = willRetain;

    return true;
}

bool PubSubClientMock::connectDummy()
{
    _connection.connected = true;
    _connection.id = nullptr;
    _connection.user = nullptr;
    _connection.pass = nullptr;

    _lastWill.topic = nullptr;
    _lastWill.message = nullptr;
    _lastWill.retain = false;

    return true;
}

PubSubClientMock& PubSubClientMock::setServer(IPAddress ip, uint16_t port)
{
    _connection.ip = ip;
    _connection.port = port;

    return *this;
}

PubSubClientMock& PubSubClientMock::setServer(const char * domain, uint16_t port)
{
    _connection.domain = domain;
    _connection.port = port;

    return *this;
}

PubSubClientMock& PubSubClientMock::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    (void)callback;

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
