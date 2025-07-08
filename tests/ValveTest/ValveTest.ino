#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastCommandCallbackCall.reset();

#define assertCommandCallbackCalled(expectedCommand, callerPtr) \
    assertTrue(lastCommandCallbackCall.called); \
    assertEqual(expectedCommand, lastCommandCallbackCall.command); \
    assertEqual(callerPtr, lastCommandCallbackCall.caller);

#define assertCommandCallbackNotCalled() \
    assertFalse(lastCommandCallbackCall.called);

using aunit::TestRunner;

struct CommandCallback {
    bool called = false;
    int16_t command = 0;
    HAValve* caller = nullptr;

    void reset() {
        called = false;
        command = 0;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueValve";
static CommandCallback lastCommandCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/valve/testDevice/uniqueValve/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueValve/stat_t"};
const char PositionTopic[] PROGMEM = {"testData/testDevice/uniqueValve/pos_t"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueValve/cmd_t"};

void onCommandReceived(int16_t command, HAValve* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.command = command;
    lastCommandCallbackCall.caller = caller;
}

AHA_TEST(ValveTest, invalid_unique_id) {
    prepareTest

    HAValve valve(nullptr);
    valve.buildSerializerTest();
    HASerializer* serializer = valve.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(ValveTest, default_params) {
    prepareTest

    HAValve valve(testUniqueId);
    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HAValve valve(testUniqueId);
    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueValve\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, default_params_with_position) {
    prepareTest

    HAValve valve(testUniqueId, HAValve::PositionFeature);
    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"pos\":\"true\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, default_params_with_stop_feature) {
    prepareTest

    HAValve valve(testUniqueId, HAValve::StopFeature);
    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"pl_stop\":\"STOP\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, default_params_with_all_features) {
    prepareTest

    HAValve valve(testUniqueId, HAValve::PositionFeature|HAValve::StopFeature);
    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"pos\":\"true\","
            "\"pl_stop\":\"STOP\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, command_subscription) {
    prepareTest

    HAValve valve(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(ValveTest, availability) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAValve
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueValve/avty_t"),
        "online",
        true
    )
}

AHA_TEST(ValveTest, publish_last_known_state) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setCurrentState(HAValve::StateClosed);
    valve.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "closed", true)
}

AHA_TEST(ValveTest, publish_last_known_state_with_position) {
    prepareTest

    HAValve valve(testUniqueId, HAValve::PositionFeature);
    valve.setCurrentState(HAValve::StateClosed);
    valve.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "{\"state\":\"closed\",\"position\":100}", true)
    assertMqttMessage(2, AHATOFSTR(StateTopic), "100", true)
}

AHA_TEST(ValveTest, publish_nothing_if_retained) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setRetain(true);
    valve.setCurrentState(HAValve::StateClosed);
    valve.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(ValveTest, name_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setName("testName");

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueValve\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, object_id_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setObjectId("testId");

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueValve\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, device_class) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"dev_cla\":\"testClass\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, position_open) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setPositionOpen(95);

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"pos_open\":95,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, position_closed) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setPositionClosed(5);

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"pos_clsd\":5,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, icon_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setIcon("testIcon");

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, retain_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setRetain(true);

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, optimistic_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setOptimistic(true);

    assertEntityConfig(
        mock,
        valve,
        (
            "{"
            "\"uniq_id\":\"uniqueValve\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueValve/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueValve/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(ValveTest, current_state_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setCurrentState(HAValve::StateClosed);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(HAValve::StateClosed, valve.getCurrentState());
}

AHA_TEST(ValveTest, current_position_setter) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.setCurrentPosition(500);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(500, valve.getCurrentPosition());
}

AHA_TEST(ValveTest, publish_state_closed) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId);

    assertTrue(valve.setState(HAValve::StateClosed));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "closed", true)
}

AHA_TEST(ValveTest, publish_state_closing) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId);

    assertTrue(valve.setState(HAValve::StateClosing));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "closing", true)
}

AHA_TEST(ValveTest, publish_state_open) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId);

    assertTrue(valve.setState(HAValve::StateOpen));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "open", true)
}

AHA_TEST(ValveTest, publish_state_opening) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId);

    assertTrue(valve.setState(HAValve::StateOpening));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "opening", true)
}

AHA_TEST(ValveTest, publish_state_opening_with_position) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId, HAValve::PositionFeature);

    assertTrue(valve.setStateWithPosition(HAValve::StateOpening, 45));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "{\"state\":\"opening\",\"position\":45}", true)
}

AHA_TEST(ValveTest, publish_position) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId, HAValve::PositionFeature);

    assertTrue(valve.setPosition(250));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "250", true)
}

AHA_TEST(ValveTest, publish_position_max) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId, HAValve::PositionFeature);

    assertTrue(valve.setPosition(32767));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "32767", true)
}

AHA_TEST(ValveTest, publish_position_debounce) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId, HAValve::PositionFeature);
    valve.setCurrentPosition(250);

    // it shouldn't publish data if state doesn't change
    assertTrue(valve.setPosition(250));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(ValveTest, publish_position_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAValve valve(testUniqueId, HAValve::PositionFeature);
    valve.setCurrentPosition(250);

    assertTrue(valve.setPosition(250, true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "250", true)
}

AHA_TEST(ValveTest, command_open) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("OPEN"));

    assertCommandCallbackCalled(HAValve::CommandOpen, &valve)
}

AHA_TEST(ValveTest, command_close) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("CLOSE"));

    assertCommandCallbackCalled(HAValve::CommandClose, &valve)
}

AHA_TEST(ValveTest, command_stop) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("STOP"));

    assertCommandCallbackCalled(HAValve::CommandStop, &valve)
}

AHA_TEST(ValveTest, command_numeric_invalid_without_feature) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("33"));

    assertCommandCallbackNotCalled()
}

AHA_TEST(ValveTest, command_numeric_with_feature) {
    prepareTest

    HAValve valve(testUniqueId, HAValve::PositionFeature);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("33"));

    assertCommandCallbackCalled((int16_t)33, &valve)
}

AHA_TEST(ValveTest, command_invalid) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("NOT_SUPPORTED"));

    assertCommandCallbackNotCalled()
}

AHA_TEST(ValveTest, different_valve_command) {
    prepareTest

    HAValve valve(testUniqueId);
    valve.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueValveDifferent/cmd_t"),
        F("CLOSE")
    );

    assertCommandCallbackNotCalled()
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
