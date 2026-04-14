#include "PubSubClientAdapter.h"

#ifndef ARDUINOHA_TEST

#include <PubSubClient.h>

PubSubClientAdapter::PubSubClientAdapter(Client& netClient) :
    _pubSub(new PubSubClient(netClient))
{
}

PubSubClientAdapter::PubSubClientAdapter(PubSubClient* pubSub) :
    _pubSub(pubSub)
{
}

PubSubClientAdapter::~PubSubClientAdapter()
{
    if (_pubSub) {
        delete _pubSub;
    }
}

bool PubSubClientAdapter::loop()
{
    return _pubSub->loop();
}

void PubSubClientAdapter::disconnect()
{
    _pubSub->disconnect();
}

bool PubSubClientAdapter::connected()
{
    return _pubSub->connected();
}

int16_t PubSubClientAdapter::state()
{
    return _pubSub->state();
}

bool PubSubClientAdapter::connect(
    const char* id,
    const char* user,
    const char* pass,
    const char* willTopic,
    uint8_t willQos,
    bool willRetain,
    const char* willMessage,
    bool cleanSession
)
{
    return _pubSub->connect(
        id, user, pass,
        willTopic, willQos, willRetain, willMessage,
        cleanSession
    );
}

void PubSubClientAdapter::setServer(IPAddress ip, uint16_t port)
{
    _pubSub->setServer(ip, port);
}

void PubSubClientAdapter::setServer(const char* domain, uint16_t port)
{
    _pubSub->setServer(domain, port);
}

void PubSubClientAdapter::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    _pubSub->setCallback(callback);
}

void PubSubClientAdapter::setKeepAlive(uint16_t keepAlive)
{
    _pubSub->setKeepAlive(keepAlive);
}

bool PubSubClientAdapter::setBufferSize(uint16_t size)
{
    return _pubSub->setBufferSize(size);
}

bool PubSubClientAdapter::beginPublish(const char* topic, unsigned int plength, bool retained)
{
    return _pubSub->beginPublish(topic, plength, retained);
}

size_t PubSubClientAdapter::write(const uint8_t* buffer, size_t size)
{
    return _pubSub->write(buffer, size);
}

size_t PubSubClientAdapter::print(const __FlashStringHelper* buffer)
{
    return _pubSub->print(buffer);
}

int PubSubClientAdapter::endPublish()
{
    return _pubSub->endPublish();
}

bool PubSubClientAdapter::subscribe(const char* topic)
{
    return _pubSub->subscribe(topic);
}

#endif  // !ARDUINOHA_TEST
