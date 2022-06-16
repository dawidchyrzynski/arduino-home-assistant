#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackState = false; \
    commandCallbackSwitchPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedCommand, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedCommand, commandCallbackState); \
    assertEqual(callerPtr, commandCallbackSwitchPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSwitch";
static const char* configTopic = "homeassistant/switch/testDevice/uniqueSwitch/config";
static const char* commandTopic = "testData/testDevice/uniqueSwitch/cmd_t";

static bool commandCallbackCalled = false;
static bool commandCallbackState = false;
static HASwitch* commandCallbackSwitchPtr = nullptr;

void onCommandReceived(bool state, HASwitch* sender)
{
    commandCallbackCalled = true;
    commandCallbackState = state;
    commandCallbackSwitchPtr = sender;
}

test(SwitchTest, invalid_unique_id) {
    prepareTest

    HASwitch testSwitch(nullptr, false);
    testSwitch.buildSerializerTest();
    HASerializer* serializer = testSwitch.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SwitchTest, default_params) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"opt\":false,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
    assertEqual(2, mock->getFlushedMessagesNb());
}

test(SwitchTest, command_subscription) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertStringCaseEqual(commandTopic, mock->getSubscriptions()[0].topic);
}

test(SwitchTest, availability) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HASwitch
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSwitch/avty_t",
        "online",
        true
    )
}

test(SwitchTest, publish_last_known_state) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setCurrentState(true);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSwitch/stat_t",
        "ON",
        true
    )
}

test(SwitchTest, publish_nothing_if_retained) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setRetain(true);
    testSwitch.setCurrentState(true);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(SwitchTest, name_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setName("testName");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSwitch\",\"opt\":false,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

test(SwitchTest, device_class) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"dev_cla\":\"testClass\",\"opt\":false,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

test(SwitchTest, icon_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setIcon("testIcon");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"ic\":\"testIcon\",\"opt\":false,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

test(SwitchTest, retain_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setRetain(true);

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"ret\":true,\"opt\":false,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

test(SwitchTest, optimistic_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setOptimistic(true);

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"opt\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}
test(SwitchTest, current_state_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.setCurrentState(true);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(true, testSwitch.getCurrentState());
}

test(SwitchTest, publish_state_on) {
    prepareTest

    mock->connectDummy();
    HASwitch testSwitch(testUniqueId, false);
    bool result = testSwitch.setState(true);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueSwitch/stat_t",
        "ON",
        true
    )
    assertTrue(result);
}

test(SwitchTest, publish_state_off) {
    prepareTest

    mock->connectDummy();
    HASwitch testSwitch(testUniqueId, true);
    bool result = testSwitch.setState(false);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueSwitch/stat_t",
        "OFF",
        true
    )
    assertTrue(result);
}

test(SwitchTest, command_on) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "ON");

    assertCallback(true, true, &testSwitch)
}

test(SwitchTest, command_off) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "OFF");

    assertCallback(true, false, &testSwitch)
}

test(SwitchTest, different_switch_command) {
    prepareTest

    HASwitch testSwitch(testUniqueId, false);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(
        "testData/testDevice/uniqueSwitchDifferent/cmd_t",
        "CLOSE"
    );

    assertCallback(false, false, nullptr)
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    TestRunner::run();
    delay(1);
}
