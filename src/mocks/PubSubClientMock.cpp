#include "PubSubClientMock.h"
#ifdef ARDUINOHA_TEST

PubSubClientMock::PubSubClientMock() :
    _pendingMessage(nullptr),
    _flushedMessages(nullptr),
    _flushedMessagesNb(0),
    _subscriptions(nullptr),
    _subscriptionsNb(0),
    callback(nullptr)
{

}

PubSubClientMock::~PubSubClientMock()
{
    if (_pendingMessage) {
        delete _pendingMessage;
    }

    if (_subscriptions) {
        delete _subscriptions;
    }

    clearFlushedMessages();
}

bool PubSubClientMock::loop()
{
    return connected();
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
    _connection.id = "dummyId";
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

PubSubClientMock& PubSubClientMock::setServer(
    const char * domain,
    uint16_t port
)
{
    _connection.domain = domain;
    _connection.port = port;

    return *this;
}

PubSubClientMock& PubSubClientMock::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    this->callback = callback;
    return *this;
}

bool PubSubClientMock::beginPublish(
    const char* topic,
    unsigned int plength,
    bool retained
)
{
    if (!connected()) {
        return false;
    }

    if (_pendingMessage) {
        delete _pendingMessage;
    }

    _pendingMessage = new MqttMessage();
    _pendingMessage->retained = retained; 

    {
        size_t size = strlen(topic) + 1;
        _pendingMessage->topic = new char[size];
        _pendingMessage->topicSize = size;

        memset(_pendingMessage->topic, 0, size);
        memcpy(_pendingMessage->topic, topic, size);
    }

    {
        size_t size = plength + 1;
        _pendingMessage->buffer = new char[size];
        _pendingMessage->bufferSize = size;

        memset(_pendingMessage->buffer, 0, size);
    }

    return true;
}

size_t PubSubClientMock::write(const uint8_t *buffer, size_t size)
{
    if (!_pendingMessage || !_pendingMessage->buffer) {
        return 0;
    }

    strncat(_pendingMessage->buffer, (const char*)buffer, size);
    return size;
}

size_t PubSubClientMock::print(const __FlashStringHelper* buffer)
{
    const size_t len = strlen_P(reinterpret_cast<const char*>(buffer));
    char data[len];
    strcpy_P(data, reinterpret_cast<const char*>(buffer));

    return write((const uint8_t*)(data), len);
}

int PubSubClientMock::endPublish()
{
    if (!_pendingMessage) {
        return 0;
    }

    size_t messageSize = _pendingMessage->bufferSize;
    uint8_t index = _flushedMessagesNb;

    _flushedMessagesNb++;
    _flushedMessages = static_cast<MqttMessage*>(
        realloc(_flushedMessages, _flushedMessagesNb * sizeof(MqttMessage))
    );

    _flushedMessages[index] = *_pendingMessage; // handover memory responsibility
    _pendingMessage = nullptr; // do not call destructor

    return messageSize;
}

bool PubSubClientMock::subscribe(const char* topic)
{
    uint8_t index = _subscriptionsNb;

    _subscriptionsNb++;
    _subscriptions = static_cast<MqttSubscription*>(
        realloc(_subscriptions, _subscriptionsNb * sizeof(MqttSubscription))
    );

    size_t topicSize = strlen(topic) + 1;
    MqttSubscription* subscription = &_subscriptions[index];
    subscription->topic = new char[topicSize];

    memcpy(subscription->topic, topic, topicSize);
    return true;
}

void PubSubClientMock::clearFlushedMessages()
{
    if (_flushedMessages) {
        delete _flushedMessages;
    }

    _flushedMessagesNb = 0;
}

void PubSubClientMock::fakeMessage(const char* topic, const char* message)
{
    if (!callback) {
        return;
    }

    callback(
        const_cast<char*>(topic), // hack
        const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(message)), // hack
        strlen(message)
    );
}

#endif
