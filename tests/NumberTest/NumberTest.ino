#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackNumber = HANumber::StateNone; \
    commandCallbackNumberPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedNumber, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedNumber, commandCallbackNumber); \
    assertEqual(callerPtr, commandCallbackNumberPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueNumber";

const char ConfigTopic[] PROGMEM = {"homeassistant/number/testDevice/uniqueNumber/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/cmd_t"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/stat_t"};

static bool commandCallbackCalled = false;
static float commandCallbackNumber = HANumber::StateNone;
static HANumber* commandCallbackNumberPtr = nullptr;

void onCommandReceived(float number, HANumber* sender)
{
    commandCallbackCalled = true;
    commandCallbackNumber = number;
    commandCallbackNumberPtr = sender;
}

AHA_TEST(NumberTest, invalid_unique_id) {
    prepareTest

    HANumber number(nullptr);
    number.buildSerializerTest();
    HASerializer* serializer = number.getSerializer();

    assertTrue(serializer == nullptr);
}

void setup()
{
    delay(1000);
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    TestRunner::run();
    delay(1);
}
