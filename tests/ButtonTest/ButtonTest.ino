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
static bool commandCallbackCalled = false;
static HAButton* commandCallbackButtonPtr = nullptr;

const char ConfigTopic[] PROGMEM = {"homeassistant/button/testDevice/uniqueButton/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueButton/cmd_t"};
const char CommandMessage[] PROGMEM = {"PRESS"};

void onCommandReceived(HAButton* button)
{
    commandCallbackCalled = true;
    commandCallbackButtonPtr = button;
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
        "{\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
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
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

AHA_TEST(ButtonTest, device_class) {
    prepareTest

    HAButton button(testUniqueId);
    button.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

AHA_TEST(ButtonTest, icon_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setIcon("testIcon");

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

AHA_TEST(ButtonTest, retain_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setRetain(true);

    assertEntityConfig(
        mock,
        button,
        "{\"uniq_id\":\"uniqueButton\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"testData/testDevice/uniqueButton/cmd_t\"}"
    )
}

AHA_TEST(ButtonTest, command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    button.onPress(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertCallback(true, &button)
}

AHA_TEST(ButtonTest, no_command_callback) {
    prepareTest

    HAButton button(testUniqueId);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertCallback(false, nullptr)
}

AHA_TEST(ButtonTest, different_button_command) {
    prepareTest

    HAButton button(testUniqueId);
    button.onPress(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueButtonDifferent/cmd_t"),
        AHATOFSTR(CommandMessage)
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
