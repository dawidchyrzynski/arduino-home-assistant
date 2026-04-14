#ifndef AHA_IMQTTCLIENT_H
#define AHA_IMQTTCLIENT_H

#include <Arduino.h>
#include <IPAddress.h>

// MQTT_CALLBACK_SIGNATURE mirrors the definition in PubSubClient.h.
// Duplicated here (with a guard) so this header can be included without
// forcing a PubSubClient dependency on translation units that only need
// the abstract interface.
#ifndef MQTT_CALLBACK_SIGNATURE
#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif
#endif

#if defined(ARDUINO_API_VERSION)
using namespace arduino;
#endif

/**
 * Abstract transport interface used by HAMqtt.
 *
 * By default HAMqtt is constructed with a `Client&` (EthernetClient,
 * WiFiClient, etc.) and internally wraps it in PubSubClient via
 * PubSubClientAdapter. To use a different MQTT stack — AsyncMqttClient,
 * ESP-IDF native MQTT, a mock, etc. — implement this interface and pass
 * an instance to the `HAMqtt(IMqttClient*, HADevice&, uint8_t)` overload.
 *
 * Ownership: HAMqtt takes ownership of the pointer it receives and
 * deletes it in its destructor. Heap-allocate your adapter with `new`.
 *
 * State contract: the integer returned from `state()` is surfaced
 * directly via `HAMqtt::ConnectionState`. Values MUST follow the
 * PubSubClient convention so existing user code that switches on the
 * enum keeps working:
 *
 *   -5 Connecting     0 Connected     3 Unavailable
 *   -4 Timeout        1 BadProtocol   4 BadCredentials
 *   -3 Lost           2 BadClientId   5 Unauthorized
 *   -2 Failed
 *   -1 Disconnected
 *
 * Adapters for non-PubSub backends are responsible for mapping native
 * error codes onto these values.
 *
 * Minimal adapter skeleton:
 * @code
 * class MyBackendAdapter : public IMqttClient {
 *     MyBackend _backend;
 * public:
 *     bool loop() override { _backend.poll(); return _backend.isConnected(); }
 *     void disconnect() override { _backend.close(); }
 *     bool connected() override { return _backend.isConnected(); }
 *     int16_t state() override { return _backend.isConnected() ? 0 : -1; }
 *     // ...the rest...
 * };
 *
 * auto* adapter = new MyBackendAdapter();
 * HAMqtt mqtt(adapter, device);
 * @endcode
 */
class IMqttClient
{
public:
    virtual ~IMqttClient() = default;

    /// Pumps the client. Called by HAMqtt::loop(). Returns false when disconnected.
    virtual bool loop() = 0;

    /// Closes the MQTT session.
    virtual void disconnect() = 0;

    /// True while the MQTT session is established.
    virtual bool connected() = 0;

    /// Current state, encoded per the PubSubClient convention. See class docs.
    virtual int16_t state() = 0;

    /**
     * Attempts to connect to the broker. Argument semantics match
     * PubSubClient::connect(). Last-will parameters may be nullptr.
     */
    virtual bool connect(
        const char* id,
        const char* user,
        const char* pass,
        const char* willTopic,
        uint8_t willQos,
        bool willRetain,
        const char* willMessage,
        bool cleanSession
    ) = 0;

    virtual void setServer(IPAddress ip, uint16_t port) = 0;
    virtual void setServer(const char* domain, uint16_t port) = 0;

    /// Registers the incoming-message callback. Signature is platform-dependent
    /// (std::function on ESP8266/ESP32, function pointer on AVR).
    virtual void setCallback(MQTT_CALLBACK_SIGNATURE) = 0;

    virtual void setKeepAlive(uint16_t keepAlive) = 0;

    /// Returns false if the backend refused the requested buffer size.
    virtual bool setBufferSize(uint16_t size) = 0;

    virtual bool beginPublish(const char* topic, unsigned int plength, bool retained) = 0;
    virtual size_t write(const uint8_t* buffer, size_t size) = 0;
    virtual size_t print(const __FlashStringHelper* buffer) = 0;
    virtual int endPublish() = 0;

    virtual bool subscribe(const char* topic) = 0;
};

#endif
