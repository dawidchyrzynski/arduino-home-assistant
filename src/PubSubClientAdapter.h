#ifndef AHA_PUBSUBCLIENTADAPTER_H
#define AHA_PUBSUBCLIENTADAPTER_H

#ifndef ARDUINOHA_TEST

#include "IMqttClient.h"

class Client;
class PubSubClient;

/**
 * IMqttClient implementation backed by PubSubClient.
 *
 * This is the default transport used by the `HAMqtt(Client&, ...)`
 * constructor and preserves the exact behavior the library has shipped
 * with since 1.x. There is usually no reason to instantiate this class
 * directly — it exists so users who want to layer customization on top
 * of PubSubClient (for example, pre-configuring buffer size or TLS) can
 * construct their own PubSubClient and pass it in.
 *
 * Takes ownership of the wrapped PubSubClient and deletes it in the
 * destructor.
 */
class PubSubClientAdapter : public IMqttClient
{
public:
    /// Constructs a fresh PubSubClient bound to `netClient` and adopts it.
    explicit PubSubClientAdapter(Client& netClient);

    /// Adopts an already-constructed PubSubClient. HAMqtt/this adapter
    /// will delete it on destruction — do not share across adapters.
    explicit PubSubClientAdapter(PubSubClient* pubSub);

    ~PubSubClientAdapter() override;

    bool loop() override;
    void disconnect() override;
    bool connected() override;
    int16_t state() override;

    bool connect(
        const char* id,
        const char* user,
        const char* pass,
        const char* willTopic,
        uint8_t willQos,
        bool willRetain,
        const char* willMessage,
        bool cleanSession
    ) override;

    void setServer(IPAddress ip, uint16_t port) override;
    void setServer(const char* domain, uint16_t port) override;
    void setCallback(MQTT_CALLBACK_SIGNATURE) override;
    void setKeepAlive(uint16_t keepAlive) override;
    bool setBufferSize(uint16_t size) override;

    bool beginPublish(const char* topic, unsigned int plength, bool retained) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    size_t print(const __FlashStringHelper* buffer) override;
    int endPublish() override;

    bool subscribe(const char* topic) override;

private:
    PubSubClient* _pubSub;
};

#endif  // !ARDUINOHA_TEST
#endif
