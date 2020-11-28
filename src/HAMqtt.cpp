#include <IPAddress.h>
#include <MQTT.h>

#include "HAMqtt.h"
#include "HADevice.h"
#include "ArduinoHADefines.h"

#define HAMQTT_INIT \
    _initialized(false), \
    _discoveryPrefix(DefaultDiscoveryPrefix), \
    _mqtt(new MQTTClient()), \
    _serverIp(new IPAddress()), \
    _serverPort(0), \
    _username(nullptr), \
    _password(nullptr), \
    _connecting(false), \
    _randomNumber(0), \
    _lastConnectionAttemptAt(0)

// Author: https://rheingoldheavy.com/better-arduino-random-values/
uint32_t generateRandomSeed()
{
    uint8_t  seedBitValue  = 0;
    uint8_t  seedByteValue = 0;
    uint32_t seedWordValue = 0;

    for (uint8_t wordShift = 0; wordShift < 4; wordShift++)
    {
        for (uint8_t byteShift = 0; byteShift < 8; byteShift++)
        {
            for (uint8_t bitSum = 0; bitSum <= 8; bitSum++)
            {
                seedBitValue = seedBitValue + (analogRead(A0) & 0x01);
            }

            delay(1);
            seedByteValue = seedByteValue | ((seedBitValue & 0x01) << byteShift);
            seedBitValue = 0;
        }

        seedWordValue = seedWordValue | (uint32_t)seedByteValue << (8 * wordShift);
        seedByteValue = 0;
    }

    return (seedWordValue);
}

const char* HAMqtt::DefaultDiscoveryPrefix = "homeassistant";

HAMqtt::HAMqtt(Client& netClient) :
    _netClient(netClient),
    _hasDevice(false),
    HAMQTT_INIT
{

}

HAMqtt::HAMqtt(Client& netClient, HADevice& device) :
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
#ifdef ARDUINOHA_DEBUG
    Serial.println(F("Initializing ArduinoHA"));
    Serial.print(F("Server address: "));
    Serial.print(serverIp);
    Serial.print(F(":"));
    Serial.print(serverPort);
    Serial.println();
#endif

    if (_initialized) {
#ifdef ARDUINOHA_DEBUG
    Serial.println(F("ArduinoHA is already initialized"));
#endif

        return false;
    }

    _mqtt->begin(serverIp, serverPort, _netClient);

    *_serverIp = serverIp;
    _serverPort = serverPort;
    _username = username;
    _password = password;
    _randomNumber = generateRandomSeed();
    _initialized = true;

    connectToServer();
    return true;
}

void HAMqtt::loop()
{
    if (!_initialized) {
        return;
    }

    _mqtt->loop();

    if (!_mqtt->connected()) {
        connectToServer();
    }
}

void HAMqtt::connectToServer()
{
    if (_lastConnectionAttemptAt > 0 &&
            abs(millis() - _lastConnectionAttemptAt) < ReconnectInterval) {
        return;
    }

    bool result;
    String clientId = generateClientId();

    #ifdef ARDUINOHA_DEBUG
        Serial.print("Connecting to the MQTT broker... Client ID: ");
        Serial.print(clientId);
        Serial.println();
    #endif

    if (_username == nullptr || _password == nullptr) {
        result = _mqtt->connect(clientId.c_str());
    } else {
        result = _mqtt->connect(clientId.c_str(), _username, _password);
    }

    if (result) {
#ifdef ARDUINOHA_DEBUG
        Serial.println(F("Connected to the broker"));
#endif

        // to do: discovery
    } else {
#ifdef ARDUINOHA_DEBUG
        Serial.println(F("Failed to connect to the broker"));
#endif
    }

    _lastConnectionAttemptAt = millis();
}

String HAMqtt::generateClientId() const
{
    if (_hasDevice) {
        //return _device.getUniqueId();
    }

    return "aha-" + String(_randomNumber, DEC);
}
