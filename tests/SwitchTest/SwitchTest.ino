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

const char ConfigTopic[] PROGMEM = {"homeassistant/switch/testDevice/uniqueSwitch/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueSwitch/stat_t"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueSwitch/cmd_t"};

static bool commandCallbackCalled = false;
static bool commandCallbackState = false;
static HASwitch* commandCallbackSwitchPtr = nullptr;

void onCommandReceived(bool state, HASwitch* sender)
{
    commandCallbackCalled = true;
    commandCallbackState = state;
    commandCallbackSwitchPtr = sender;
}

AHA_TEST(SwitchTest, invalid_unique_id) {
    prepareTest

    HASwitch testSwitch(nullptr);
    testSwitch.buildSerializerTest();
    HASerializer* serializer = testSwitch.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(SwitchTest, default_params) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
    assertEqual(2, mock->getFlushedMessagesNb());
}

AHA_TEST(SwitchTest, command_subscription) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(SwitchTest, availability) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HASwitch
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueSwitch/avty_t"),
        "online",
        true
    )
}

AHA_TEST(SwitchTest, publish_last_known_state) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setCurrentState(true);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(SwitchTest, publish_nothing_if_retained) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setRetain(true);
    testSwitch.setCurrentState(true);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(SwitchTest, name_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setName("testName");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSwitch\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

AHA_TEST(SwitchTest, device_class) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

AHA_TEST(SwitchTest, icon_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setIcon("testIcon");

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

AHA_TEST(SwitchTest, retain_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setRetain(true);

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

AHA_TEST(SwitchTest, optimistic_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setOptimistic(true);

    assertEntityConfig(
        mock,
        testSwitch,
        "{\"uniq_id\":\"uniqueSwitch\",\"opt\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSwitch/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSwitch/cmd_t\"}"
    )
}

AHA_TEST(SwitchTest, current_state_setter) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.setCurrentState(true);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(true, testSwitch.getCurrentState());
}

AHA_TEST(SwitchTest, publish_state_on) {
    prepareTest

    mock->connectDummy();
    HASwitch testSwitch(testUniqueId);
    bool result = testSwitch.setState(true);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
    assertTrue(result);
}

AHA_TEST(SwitchTest, publish_state_off) {
    prepareTest

    mock->connectDummy();
    HASwitch testSwitch(testUniqueId);
    testSwitch.setCurrentState(true);
    bool result = testSwitch.setState(false);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "OFF", true)
    assertTrue(result);
}

AHA_TEST(SwitchTest, command_on) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("ON"));

    assertCallback(true, true, &testSwitch)
}

AHA_TEST(SwitchTest, command_off) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("OFF"));

    assertCallback(true, false, &testSwitch)
}

AHA_TEST(SwitchTest, different_switch_command) {
    prepareTest

    HASwitch testSwitch(testUniqueId);
    testSwitch.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueSwitchDifferent/cmd_t"),
        F("CLOSE")
    );

    assertCallback(false, false, nullptr)
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
