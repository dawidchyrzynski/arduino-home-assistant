#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackButtonPtr = nullptr;

#define assertCallback(shouldBeCalled, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(callerPtr, commandCallbackButtonPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueButton";
static const char* configTopic = "homeassistant/button/testDevice/uniqueButton/config";
static const char* commandTopic = "testData/testDevice/uniqueButton/cmd_t";
static const char* commandMessage = "PRESS";
static bool commandCallbackCalled = false;
static HAButton* commandCallbackButtonPtr = nullptr;

void onCommandReceived(HAButton* button)
{
    commandCallbackCalled = true;
    commandCallbackButtonPtr = button;
}

test(ButtonTest, invalid_unique_id) {
    prepareTest

    HAButton button(nullptr);
    button.buildSerializerTest();
    HASerializer* serializer = button.getSerializer();

    assertTrue(serializer == nullptr);
}

test(ButtonTest, default_params) {
    prepareTest

    HAButton button(testUniqueId);
    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

test(ButtonTest, command_subscription) {
    prepareTest

    HAButton button(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertStringCaseEqual(commandTopic, mock->getSubscriptions()[0].topic);
}

test(ButtonTest, availability) {
    prepareTest

    HAButton button(testUniqueId);
    button.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAButton
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueButton/avty_t",
        "online",
        true
    )
}

test(ButtonTest, name_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setName("testName");

    assertEntityConfig(
        mock,
        button,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

test(ButtonTest, device_class) {
    prepareTest

    HAButton button(testUniqueId);
    button.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

test(ButtonTest, icon_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setIcon("testIcon");

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

test(ButtonTest, retain_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setRetain(true);

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

test(ButtonTest, command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    button.onPress(onCommandReceived);
    mock->fakeMessage(commandTopic, commandMessage);

    assertCallback(true, &button)
}

test(ButtonTest, no_command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    mock->fakeMessage(commandTopic, commandMessage);

    assertCallback(false, nullptr)
}

test(ButtonTest, different_button_command) {
    prepareTest

    HAButton button(testUniqueId);
    button.onPress(onCommandReceived);
    mock->fakeMessage(
        "testData/testDevice/uniqueButtonDifferent/cmd_t",
        commandMessage
    );

    assertCallback(false, nullptr)
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
