#ifndef AHA_HAMQTT_H
#define AHA_HAMQTT_H

#include <Client.h>
#include <IPAddress.h>

#define HAMQTT_CALLBACK(name) void (*name)()
#define HAMQTT_MESSAGE_CALLBACK(name) void (*name)(const char* topic, const uint8_t* payload, uint16_t length)
#define HAMQTT_DEFAULT_PORT 1883

class PubSubClient;
class HADevice;
class BaseDeviceType;

class HAMqtt
{
public:
    static const uint16_t ReconnectInterval = 5000; // ms

    inline static HAMqtt* instance()
        { return _instance; }

    HAMqtt(Client& netClient, HADevice& device);

    /**
     * Sets prefix for Home Assistant discovery.
     * It needs to match prefix set in the HA admin panel.
     * The default prefix is "homeassistant".
     *
     * @param prefix
     */
    inline void setDiscoveryPrefix(const char* prefix)
        { _discoveryPrefix = prefix; }

    /**
     * Returns discovery prefix.
     */
    inline const char* getDiscoveryPrefix() const
        { return _discoveryPrefix; }

    /**
     * Sets prefix that will be used for topics different than discovery.
     * It may be useful if you want to pass MQTT trafic through bridge.
     *
     * @param prefix
     */
    inline void setDataPrefix(const char* prefix)
        { _dataPrefix = prefix; }

    /**
     * Returns data prefix.
     */
    inline const char* getDataPrefix() const
        { return _dataPrefix; }

    /**
     * Returns instance of the device assigned to the HAMqtt class.
     */
    inline HADevice const* getDevice() const
        { return &_device; }

    /**
     * Given callback will be called for each received message from the broker.
     *
     * @param callback
     */
    inline void onMessage(HAMQTT_MESSAGE_CALLBACK(callback))
        { _messageCallback = callback; }

    /**
     * Given callback will be called each time the connection with broker is acquired.
     *
     * @param callback
     */
    inline void onConnected(HAMQTT_CALLBACK(callback))
        { _connectedCallback = callback; }

    /**
     * Given callback will be called each time the library fails to connect to the broker.
     *
     * @param callback
     */
    inline void onConnectionFailed(HAMQTT_CALLBACK(callback))
        { _connectionFailedCallback = callback; }

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
        const uint16_t& serverPort = HAMQTT_DEFAULT_PORT,
        const char* username = nullptr,
        const char* password = nullptr
    );
    bool begin(
        const IPAddress& serverIp,
        const char* username,
        const char* password
    );

    /**
     * Connects to the MQTT broker using hostname.
     *
     * @param hostname Hostname of the MQTT broker.
     * @param serverPort Port of the MQTT broker.
     * @param username Username for authentication.
     * @param password Password for authentication.
     */
    bool begin(
        const char* hostname,
        const uint16_t& serverPort = HAMQTT_DEFAULT_PORT,
        const char* username = nullptr,
        const char* password = nullptr
    );
    bool begin(
        const char* hostname,
        const char* username,
        const char* password
    );

    /**
     * Closes connection with the MQTT broker.
     *
     * @param sendLastWill Set to true if you want to publish device unavailability before closing the connection.
     */
    bool disconnect(bool sendLastWill = true);

    /**
     * ArduinoHA's ticker.
     */
    void loop();

    /**
     * Returns true if connection to the MQTT broker is established.
     */
    bool isConnected();

    /**
     * Adds a new device's type to the MQTT.
     * Each time the connection with MQTT broker is acquired, the HAMqtt class
     * calls "onMqttConnected" method in all devices' types instances.
     *
     * @param deviceType Instance of the device's type (eg. HATriggers).
     */
    void addDeviceType(BaseDeviceType* deviceType);

    /**
     * Publishes MQTT message with given topic and payload.
     * Message won't be published if connection with MQTT broker is not established.
     * In this case method returns false.
     *
     * @param topic Topic to publish.
     * @param payload Payload to publish (it may be empty const char).
     * @param retained Determines whether message should be retained.
     */
    bool publish(const char* topic, const char* payload, bool retained = false);

    bool beginPublish(const char* topic, uint16_t payloadLength, bool retained = false);
    bool writePayload(const char* data, uint16_t length);
    bool writePayload_P(const char* src);
    bool endPublish();

    /**
     * Subscribes to the given topic.
     * Whenever a new message is received the onMqttMessage callback in all
     * devices types is called.
     *
     * Please note that you need to subscribe topic each time the connection
     * with the broker is acquired.
     *
     * @param topic Topic to subscribe
     */
    bool subscribe(const char* topic);

    /**
     * Enables last will message that will be produced when device disconnects from the broker.
     * If you want to change availability of the device in Home Assistant panel
     * please use enableLastWill() method in the HADevice object instead.
     *
     * @param lastWillTopic
     * @param lastWillMessage
     * @param lastWillRetain
     */
    inline void setLastWill(
        const char* lastWillTopic,
        const char* lastWillMessage,
        bool lastWillRetain
    ) {
        _lastWillTopic = lastWillTopic;
        _lastWillMessage = lastWillMessage;
        _lastWillRetain = lastWillRetain;
    }

    /**
     * Processes MQTT message received from the broker (subscription).
     *
     * @param topic Topic of the message.
     * @param payload Content of the message.
     * @param length Length of the message.
     */
    void processMessage(char* topic, uint8_t* payload, uint16_t length);

private:
    static HAMqtt* _instance;

    /**
     * Attempts to connect to the MQTT broker.
     * The method uses properties passed to the "begin" method.
     */
    void connectToServer();

    /**
     * This method is called each time the connection with MQTT broker is acquired.
     */
    void onConnectedLogic();

    Client& _netClient;
    HADevice& _device;
    HAMQTT_MESSAGE_CALLBACK(_messageCallback);
    HAMQTT_CALLBACK(_connectedCallback);
    HAMQTT_CALLBACK(_connectionFailedCallback);
    bool _initialized;
    const char* _discoveryPrefix;
    const char* _dataPrefix;
    PubSubClient* _mqtt;
    const char* _username;
    const char* _password;
    uint32_t _lastConnectionAttemptAt;
    uint8_t _devicesTypesNb;
    BaseDeviceType** _devicesTypes;
    const char* _lastWillTopic;
    const char* _lastWillMessage;
    bool _lastWillRetain;
};

#endif
