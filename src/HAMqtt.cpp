#include <IPAddress.h>
#include <PubSubClient.h>

#include "HAMqtt.h"
#include "HADevice.h"
#include "ArduinoHADefines.h"
#include "device-types/BaseDeviceType.h"

#define HAMQTT_INIT \
    _initialized(false), \
    _discoveryPrefix(DefaultDiscoveryPrefix), \
    _mqtt(new PubSubClient(netClient)), \
    _serverIp(new IPAddress()), \
    _serverPort(0), \
    _username(nullptr), \
    _password(nullptr), \
    _lastConnectionAttemptAt(0), \
    _devicesTypesNb(0), \
    _devicesTypes(nullptr)

const char* HAMqtt::DefaultDiscoveryPrefix = "homeassistant";

HAMqtt::HAMqtt(const char* clientId, Client& netClient) :
    _clientId(clientId),
    _netClient(netClient),
    _hasDevice(false),
    HAMQTT_INIT
{

}

HAMqtt::HAMqtt(const char* clientId, Client& netClient, HADevice& device) :
    _clientId(clientId),
    _netClient(netClient),
    _device(device),
    _hasDevice(true),
    HAMQTT_INIT
{

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

    if (_initialized) {
#if defined(ARDUINOHA_DEBUG)
    Serial.println(F("ArduinoHA is already initialized"));
#endif

        return false;
    }

    *_serverIp = serverIp;
    _serverPort = serverPort;
    _username = username;
    _password = password;
    _initialized = true;

    _mqtt->setServer(*_serverIp, _serverPort);
    // _mqtt->setCallback(...);

    return true;
}

void HAMqtt::loop()
{
    if (!_initialized) {
        return;
    }

    if (!_mqtt->loop()) {
        connectToServer();
    }
}

bool HAMqtt::isConnected()
{
    return _mqtt->connected();
}

void HAMqtt::addDeviceType(BaseDeviceType* deviceType)
{
    BaseDeviceType** data = realloc(_devicesTypes, sizeof(BaseDeviceType*) * (_devicesTypesNb + 1));
    if (data != nullptr) {
        _devicesTypes = data;
        _devicesTypes[_devicesTypesNb] = deviceType;
        _devicesTypesNb++;
    }
}

void HAMqtt::removeDeviceType(BaseDeviceType* deviceType)
{
    // to do
}

bool HAMqtt::publish(const char* topic, const char* payload, bool retained)
{
    if (!isConnected()) {
        return false;
    }

    #if defined(ARDUINOHA_DEBUG)
        Serial.print(F("Publishing message with topic: "));
        Serial.print(topic);
        Serial.print(F(", payload length: "));
        Serial.print(strlen(payload));
        Serial.println();
    #endif

    _mqtt->beginPublish(topic, strlen(payload), retained);
    _mqtt->write(payload, strlen(payload));
    return _mqtt->endPublish();
}

bool HAMqtt::beginPublish(
    const char* topic,
    uint16_t payloadLength,
    bool retained
)
{
    #if defined(ARDUINOHA_DEBUG)
        Serial.print(F("Publishing message with topic: "));
        Serial.print(topic);
        Serial.print(F(", payload length: "));
        Serial.print(payloadLength);
        Serial.println();
    #endif

    return _mqtt->beginPublish(topic, payloadLength, retained);
}

bool HAMqtt::writePayload(const char* data, uint16_t length)
{
    return (_mqtt->write(data, length) > 0);
}

bool HAMqtt::endPublish()
{
    return _mqtt->endPublish();
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
        Serial.print(_clientId);
        Serial.println();
    #endif

    if (_username == nullptr || _password == nullptr) {
        _mqtt->connect(_clientId);
    } else {
        _mqtt->connect(_clientId, _username, _password);
    }

    if (isConnected()) {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Connected to the broker"));
#endif

        onConnected();
    } else {
#if defined(ARDUINOHA_DEBUG)
        Serial.println(F("Failed to connect to the broker"));
#endif
    }
}

void HAMqtt::onConnected()
{
    for (uint8_t i = 0; i < _devicesTypesNb; i++) {
        _devicesTypes[i]->onMqttConnected();
    }
}
