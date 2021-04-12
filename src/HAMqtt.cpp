#include <PubSubClient.h>

#include "HAMqtt.h"
#include "HADevice.h"
#include "ArduinoHADefines.h"
#include "device-types/BaseDeviceType.h"

static const char* DefaultDiscoveryPrefix = "homeassistant";
HAMqtt* HAMqtt::_instance = nullptr;

void onMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    if (HAMqtt::instance() == nullptr || length > UINT16_MAX) {
        return;
    }

    HAMqtt::instance()->processMessage(topic, payload, static_cast<uint16_t>(length));
}

HAMqtt::HAMqtt(Client& netClient, HADevice& device) :
    _netClient(netClient),
    _device(device),
    _messageCallback(nullptr),
    _connectedCallback(nullptr),
    _connectionFailedCallback(nullptr),
    _initialized(false),
    _discoveryPrefix(DefaultDiscoveryPrefix),
    _dataPrefix(nullptr),
    _mqtt(new PubSubClient(netClient)),
    _username(nullptr),
    _password(nullptr),
    _lastConnectionAttemptAt(0),
    _devicesTypesNb(0),
    _devicesTypes(nullptr),
    _lastWillTopic(nullptr),
    _lastWillMessage(nullptr),
    _lastWillRetain(false)
{
    _instance = this;
}

bool HAMqtt::begin(
    const IPAddress& serverIp,
    const uint16_t& serverPort,
    const char* username,
    const char* password
)
{
#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("Initializing ArduinoHA"));
    Serial.print(F("Server address: "));
    Serial.print(serverIp);
    Serial.print(F(":"));
    Serial.print(serverPort);
    Serial.println();
#endif

    if (_device.getUniqueId() == nullptr) {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Failed to initialize ArduinoHA. Missing device's unique ID."));
#endif

        return false;
    }

    if (_initialized) {
#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("ArduinoHA is already initialized"));
#endif

        return false;
    }

    _username = username;
    _password = password;
    _initialized = true;

    _mqtt->setServer(serverIp, serverPort);
    _mqtt->setCallback(onMessageReceived);

    return true;
}

bool HAMqtt::begin(
    const IPAddress& serverIp,
    const char* username,
    const char* password
)
{
    return begin(serverIp, HAMQTT_DEFAULT_PORT, username, password);
}

bool HAMqtt::begin(
    const char* hostname,
    const uint16_t& serverPort,
    const char* username,
    const char* password
)
{
#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("Initializing ArduinoHA"));
    Serial.print(F("Server address: "));
    Serial.print(hostname);
    Serial.print(F(":"));
    Serial.print(serverPort);
    Serial.println();
#endif

    if (_device.getUniqueId() == nullptr) {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Failed to initialize ArduinoHA. Missing device's unique ID."));
#endif

        return false;
    }

    if (_initialized) {
#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("ArduinoHA is already initialized"));
#endif

        return false;
    }

    _username = username;
    _password = password;
    _initialized = true;

    _mqtt->setServer(hostname, serverPort);
    _mqtt->setCallback(onMessageReceived);

    return true;
}

bool HAMqtt::begin(
    const char* hostname,
    const char* username,
    const char* password
)
{
    return begin(hostname, HAMQTT_DEFAULT_PORT, username, password);
}

bool HAMqtt::disconnect(bool sendLastWill)
{
    if (!_initialized) {
        return false;
    }

#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("Closing connection with MQTT broker"));
#endif

    if (sendLastWill &&
            _lastWillTopic != nullptr &&
            _lastWillMessage != nullptr) {
        publish(_lastWillTopic, _lastWillMessage, _lastWillRetain);
    }

    _initialized = false;
    _lastConnectionAttemptAt = 0;
    _mqtt->disconnect();

    return true;
}

void HAMqtt::loop()
{
    if (_initialized && !_mqtt->loop()) {
        connectToServer();
    }
}

bool HAMqtt::isConnected()
{
    return _mqtt->connected();
}

void HAMqtt::addDeviceType(BaseDeviceType* deviceType)
{
    BaseDeviceType** data = (BaseDeviceType**)realloc(
        _devicesTypes,
        sizeof(BaseDeviceType*) * (_devicesTypesNb + 1)
    );

    if (data != nullptr) {
        _devicesTypes = data;
        _devicesTypes[_devicesTypesNb] = deviceType;
        _devicesTypesNb++;
    }
}

bool HAMqtt::publish(const char* topic, const char* payload, bool retained)
{
    if (!isConnected()) {
        return false;
    }

#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Publishing: "));
    Serial.print(topic);
    Serial.print(F(", len: "));
    Serial.print(strlen(payload));
    Serial.println();
#endif

    _mqtt->beginPublish(topic, strlen(payload), retained);
    _mqtt->write((const uint8_t*)(payload), strlen(payload));
    return _mqtt->endPublish();
}

bool HAMqtt::beginPublish(
    const char* topic,
    uint16_t payloadLength,
    bool retained
)
{
#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Publishing: "));
    Serial.print(topic);
    Serial.print(F(", len: "));
    Serial.print(payloadLength);
    Serial.println();
#endif

    return _mqtt->beginPublish(topic, payloadLength, retained);
}

bool HAMqtt::writePayload(const char* data, uint16_t length)
{
    return (_mqtt->write((const uint8_t*)(data), length) > 0);
}

bool HAMqtt::writePayload_P(const char* src)
{
    char data[strlen_P(src) + 1];
    strcpy_P(data, src);

    return _mqtt->write((const uint8_t*)(data), strlen(data));
}

bool HAMqtt::endPublish()
{
    return _mqtt->endPublish();
}

bool HAMqtt::subscribe(const char* topic)
{
#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Subscribing topic: "));
    Serial.print(topic);
    Serial.println();
#endif

    return _mqtt->subscribe(topic);
}

void HAMqtt::processMessage(char* topic, uint8_t* payload, uint16_t length)
{
#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Received message on topic: "));
    Serial.print(topic);
    Serial.print(F(", payload length: "));
    Serial.print(length);
    Serial.println();
#endif

    if (_messageCallback) {
        _messageCallback(topic, payload, length);
    }

    for (uint8_t i = 0; i < _devicesTypesNb; i++) {
        _devicesTypes[i]->onMqttMessage(topic, payload, length);
    }
}

void HAMqtt::connectToServer()
{
    if (_lastConnectionAttemptAt > 0 &&
            (millis() - _lastConnectionAttemptAt) < ReconnectInterval) {
        return;
    }

    _lastConnectionAttemptAt = millis();

#if defined(ARDUINOHA_DEBUG)
    Serial.print(F("Connecting to the MQTT broker... Client ID: "));
    Serial.print(_device.getUniqueId());
    Serial.println();
#endif

    _mqtt->connect(
        _device.getUniqueId(),
        _username,
        _password,
        _lastWillTopic,
        0,
        _lastWillRetain,
        _lastWillMessage,
        true
    );

    if (isConnected()) {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Connected to the broker"));
#endif

        onConnectedLogic();
    } else {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Failed to connect to the broker"));

        if (_connectionFailedCallback) {
            _connectionFailedCallback();
        }
#endif
    }
}

void HAMqtt::onConnectedLogic()
{
    if (_connectedCallback) {
        _connectedCallback();
    }

    _device.publishAvailability();

    for (uint8_t i = 0; i < _devicesTypesNb; i++) {
        _devicesTypes[i]->onMqttConnected();
    }
}
