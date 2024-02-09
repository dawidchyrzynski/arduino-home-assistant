#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastPressCallbackCall.reset();

#define assertPressCallbackCalled(callerPtr) \
    assertTrue(lastPressCallbackCall.called); \
    assertEqual(callerPtr, lastPressCallbackCall.caller);

#define assertPressCallbackNotCalled() \
    assertFalse(lastPressCallbackCall.called);

using aunit::TestRunner;

struct PressCallback {
    bool called = false;
    HAButton* caller = nullptr;

    void reset() {
        called = false;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueButton";
static PressCallback lastPressCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/button/testDevice/uniqueButton/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueButton/cmd_t"};
const char CommandMessage[] PROGMEM = {"PRESS"};

void onCommandReceived(HAButton* caller)
{
    lastPressCallbackCall.called = true;
    lastPressCallbackCall.caller = caller;
}

AHA_TEST(ButtonTest, invalid_unique_id) {
    prepareTest

    HAButton button(nullptr);
    button.buildSerializerTest();
    HASerializer* serializer = button.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(ButtonTest, default_params) {
    prepareTest

    HAButton button(testUniqueId);
    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"uniq_id\":\"uniqueButton\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HAButton button(testUniqueId);
    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueButton\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, command_subscription) {
    prepareTest

    HAButton button(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(ButtonTest, availability) {
    prepareTest

    HAButton button(testUniqueId);
    button.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAButton
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueButton/avty_t"),
        "online",
        true
    )
}

AHA_TEST(ButtonTest, name_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setName("testName");

    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueButton\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, object_id_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setObjectId("testId");

    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueButton\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, device_class) {
    prepareTest

    HAButton button(testUniqueId);
    button.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"uniq_id\":\"uniqueButton\","
            "\"dev_cla\":\"testClass\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, icon_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setIcon("testIcon");

    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"uniq_id\":\"uniqueButton\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, retain_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setRetain(true);

    assertEntityConfig(
        mock,
        button,
        (
            "{"
            "\"uniq_id\":\"uniqueButton\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ButtonTest, command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    button.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertPressCallbackCalled(&button)
}

AHA_TEST(ButtonTest, no_command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertPressCallbackNotCalled()
}

AHA_TEST(ButtonTest, different_button_command) {
    prepareTest

    HAButton button(testUniqueId);
    button.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueButtonDifferent/cmd_t"),
        AHATOFSTR(CommandMessage)
    );

    assertPressCallbackNotCalled()
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
