#include "HAMqtt.h"

#ifndef ARDUINOHA_TEST
#include <PubSubClient.h>
#endif

#include "HADevice.h"
#include "device-types/HABaseDeviceType.h"
#include "mocks/PubSubClientMock.h"

#define HAMQTT_INIT \
    _device(device), \
    _messageCallback(nullptr), \
    _connectedCallback(nullptr), \
    _disconnectedCallback(nullptr), \
    _stateChangedCallback(nullptr), \
    _initialized(false), \
    _discoveryPrefix(DefaultDiscoveryPrefix), \
    _dataPrefix(DefaultDataPrefix), \
    _username(nullptr), \
    _password(nullptr), \
    _lastConnectionAttemptAt(0), \
    _devicesTypesNb(0), \
    _maxDevicesTypesNb(maxDevicesTypesNb), \
    _devicesTypes(new HABaseDeviceType*[maxDevicesTypesNb]), \
    _lastWillTopic(nullptr), \
    _lastWillMessage(nullptr), \
    _lastWillRetain(false), \
    _currentState(StateDisconnected)

static const char* DefaultDiscoveryPrefix = "homeassistant";
static const char* DefaultDataPrefix = "aha";

HAMqtt* HAMqtt::_instance = nullptr;

void onMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    if (HAMqtt::instance() == nullptr || length > UINT16_MAX) {
        return;
    }

    HAMqtt::instance()->processMessage(topic, payload, static_cast<uint16_t>(length));
}

#ifdef ARDUINOHA_TEST
HAMqtt::HAMqtt(
    PubSubClientMock* pubSub,
    HADevice& device,
    uint8_t maxDevicesTypesNb
) :
    _mqtt(pubSub),
    HAMQTT_INIT
{
    _instance = this;
}
#else
HAMqtt::HAMqtt(
    Client& netClient,
    HADevice& device,
    uint8_t maxDevicesTypesNb
) :
    _mqtt(new PubSubClient(netClient)),
    HAMQTT_INIT
{
    _instance = this;
}
#endif

HAMqtt::~HAMqtt()
{
    delete[] _devicesTypes;

    if (_mqtt) {
        delete _mqtt;
    }

    _instance = nullptr;
}

bool HAMqtt::begin(
    const IPAddress serverIp,
    const uint16_t serverPort,
    const char* username,
    const char* password
)
{
    ARDUINOHA_DEBUG_PRINT(F("AHA: init server "))
    ARDUINOHA_DEBUG_PRINT(serverIp)
    ARDUINOHA_DEBUG_PRINT(F(":"))
    ARDUINOHA_DEBUG_PRINTLN(serverPort)

    if (_device.getUniqueId() == nullptr) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: init failed. Missing device unique ID"))
        return false;
    }

    if (_initialized) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: already initialized"))
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
    const IPAddress serverIp,
    const char* username,
    const char* password
)
{
    return begin(serverIp, HAMQTT_DEFAULT_PORT, username, password);
}

bool HAMqtt::begin(
    const char* serverHostname,
    const uint16_t serverPort,
    const char* username,
    const char* password
)
{
    ARDUINOHA_DEBUG_PRINT(F("AHA: init server "))
    ARDUINOHA_DEBUG_PRINT(serverHostname)
    ARDUINOHA_DEBUG_PRINT(F(":"))
    ARDUINOHA_DEBUG_PRINTLN(serverPort)

    if (_device.getUniqueId() == nullptr) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: init failed. Missing device unique ID"))
        return false;
    }

    if (_initialized) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: already initialized"))
        return false;
    }

    _username = username;
    _password = password;
    _initialized = true;

    _mqtt->setServer(serverHostname, serverPort);
    _mqtt->setCallback(onMessageReceived);

    return true;
}

bool HAMqtt::begin(
    const char* serverHostname,
    const char* username,
    const char* password
)
{
    return begin(serverHostname, HAMQTT_DEFAULT_PORT, username, password);
}

bool HAMqtt::disconnect()
{
    if (!_initialized) {
        return false;
    }

    ARDUINOHA_DEBUG_PRINTLN(F("AHA: disconnecting"))

    _initialized = false;
    _lastConnectionAttemptAt = 0;
    _mqtt->disconnect();

    return true;
}

void HAMqtt::loop()
{
    if (!_initialized) {
        return;
    }

    bool result = _mqtt->loop();
    if (_currentState != _mqtt->state()) {
        setState(static_cast<ConnectionState>(_mqtt->state()));
    }

    if (!result) {
        connectToServer();
    }
}

bool HAMqtt::isConnected() const
{
    return _mqtt->connected();
}

void HAMqtt::setKeepAlive(uint16_t keepAlive)
{
    _mqtt->setKeepAlive(keepAlive);
}

bool HAMqtt::setBufferSize(uint16_t size)
{
    return _mqtt->setBufferSize(size);
}

void HAMqtt::addDeviceType(HABaseDeviceType* deviceType)
{
    if (_devicesTypesNb + 1 > _maxDevicesTypesNb) {
        return;
    }

    _devicesTypes[_devicesTypesNb++] = deviceType;
}

bool HAMqtt::publish(const char* topic, const char* payload, bool retained)
{
    if (!isConnected()) {
        return false;
    }

    ARDUINOHA_DEBUG_PRINT(F("AHA: publishing "))
    ARDUINOHA_DEBUG_PRINT(topic)
    ARDUINOHA_DEBUG_PRINT(F(", len: "))
    ARDUINOHA_DEBUG_PRINTLN(strlen(payload))

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
    ARDUINOHA_DEBUG_PRINT(F("AHA: begin publish "))
    ARDUINOHA_DEBUG_PRINT(topic)
    ARDUINOHA_DEBUG_PRINT(F(", len: "))
    ARDUINOHA_DEBUG_PRINTLN(payloadLength)

    return _mqtt->beginPublish(topic, payloadLength, retained);
}

void HAMqtt::writePayload(const char* data, const uint16_t length)
{
    writePayload(reinterpret_cast<const uint8_t*>(data), length);
}

void HAMqtt::writePayload(const uint8_t* data, const uint16_t length)
{
    _mqtt->write(data, length);
}

void HAMqtt::writePayload(const __FlashStringHelper* src)
{
    _mqtt->print(src);
}

bool HAMqtt::endPublish()
{
    return _mqtt->endPublish();
}

bool HAMqtt::subscribe(const char* topic)
{
    ARDUINOHA_DEBUG_PRINT(F("AHA: subscribing "))
    ARDUINOHA_DEBUG_PRINTLN(topic)

    return _mqtt->subscribe(topic);
}

void HAMqtt::processMessage(const char* topic, const uint8_t* payload, uint16_t length)
{
    ARDUINOHA_DEBUG_PRINT(F("AHA: received call "))
    ARDUINOHA_DEBUG_PRINT(topic)
    ARDUINOHA_DEBUG_PRINT(F(", len: "))
    ARDUINOHA_DEBUG_PRINTLN(length)

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
    setState(StateConnecting);

    ARDUINOHA_DEBUG_PRINT(F("AHA: MQTT connecting, client ID: "))
    ARDUINOHA_DEBUG_PRINTLN(_device.getUniqueId())

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
        setState(StateConnected);
    } else {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: failed to connect"))
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

void HAMqtt::setState(ConnectionState state)
{
    ConnectionState previousState = _currentState;
    _currentState = state;

    ARDUINOHA_DEBUG_PRINT(F("AHA: MQTT state changed to "))
    ARDUINOHA_DEBUG_PRINT(_currentState)
    ARDUINOHA_DEBUG_PRINT(F(", previous state: "))
    ARDUINOHA_DEBUG_PRINTLN(previousState)

    if (_currentState == StateConnected) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: MQTT connected"))
        onConnectedLogic();
    } else if (previousState == StateConnected && _currentState != StateConnected) {
        ARDUINOHA_DEBUG_PRINTLN(F("AHA: MQTT disconnected"))

        if (_disconnectedCallback) {
            _disconnectedCallback();
        }
    }

    if (_stateChangedCallback) {
        _stateChangedCallback(_currentState);
    }
}