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
    HACover::CoverCommand command = static_cast<HACover::CoverCommand>(0);
    HACover* caller = nullptr;

    void reset() {
        called = false;
        command = static_cast<HACover::CoverCommand>(0);
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCover";
static CommandCallback lastCommandCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/cover/testDevice/uniqueCover/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueCover/stat_t"};
const char PositionTopic[] PROGMEM = {"testData/testDevice/uniqueCover/pos_t"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueCover/cmd_t"};

void onCommandReceived(HACover::CoverCommand command, HACover* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.command = command;
    lastCommandCallbackCall.caller = caller;
}

AHA_TEST(CoverTest, invalid_unique_id) {
    prepareTest

    HACover cover(nullptr);
    cover.buildSerializerTest();
    HASerializer* serializer = cover.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(CoverTest, default_params) {
    prepareTest

    HACover cover(testUniqueId);
    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HACover cover(testUniqueId);
    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, default_params_with_position) {
    prepareTest

    HACover cover(testUniqueId, HACover::PositionFeature);
    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\","
            "\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, command_subscription) {
    prepareTest

    HACover cover(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(CoverTest, availability) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACover
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueCover/avty_t"),
        "online",
        true
    )
}

AHA_TEST(CoverTest, publish_last_known_state) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateClosed);
    cover.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "closed", true)
}

AHA_TEST(CoverTest, publish_last_known_state_with_position) {
    prepareTest

    HACover cover(testUniqueId, HACover::PositionFeature);
    cover.setCurrentState(HACover::StateClosed);
    cover.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "closed", true)
    assertMqttMessage(2, AHATOFSTR(PositionTopic), "100", true)
}

AHA_TEST(CoverTest, publish_nothing_if_retained) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setRetain(true);
    cover.setCurrentState(HACover::StateClosed);
    cover.setCurrentPosition(100);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, name_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setName("testName");

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, object_id_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setObjectId("testId");

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, device_class) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"dev_cla\":\"testClass\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, icon_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setIcon("testIcon");

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, retain_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setRetain(true);

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, optimistic_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setOptimistic(true);

    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(CoverTest, current_state_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateStopped);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(HACover::StateStopped, cover.getCurrentState());
}

AHA_TEST(CoverTest, current_position_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setCurrentPosition(500);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(500, cover.getCurrentPosition());
}

AHA_TEST(CoverTest, publish_state_closed) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);

    assertTrue(cover.setState(HACover::StateClosed));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "closed", true)
}

AHA_TEST(CoverTest, publish_state_closing) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);

    assertTrue(cover.setState(HACover::StateClosing));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "closing", true)
}

AHA_TEST(CoverTest, publish_state_open) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);

    assertTrue(cover.setState(HACover::StateOpen));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "open", true)
}

AHA_TEST(CoverTest, publish_state_opening) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);

    assertTrue(cover.setState(HACover::StateOpening));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "opening", true)
}

AHA_TEST(CoverTest, publish_state_stopped) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);

    assertTrue(cover.setState(HACover::StateStopped));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "stopped", true)
}

AHA_TEST(CoverTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateStopped);

    // it shouldn't publish data if state doesn't change
    assertTrue(cover.setState(HACover::StateStopped));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(CoverTest, publish_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateStopped);

    assertTrue(cover.setState(HACover::StateStopped, true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "stopped", true)
}

AHA_TEST(CoverTest, publish_position) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::PositionFeature);

    assertTrue(cover.setPosition(250));
    assertSingleMqttMessage(AHATOFSTR(PositionTopic), "250", true)
}

AHA_TEST(CoverTest, publish_position_max) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::PositionFeature);

    assertTrue(cover.setPosition(32767));
    assertSingleMqttMessage(AHATOFSTR(PositionTopic), "32767", true)
}

AHA_TEST(CoverTest, publish_position_debounce) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::PositionFeature);
    cover.setCurrentPosition(250);

    // it shouldn't publish data if state doesn't change
    assertTrue(cover.setPosition(250));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(CoverTest, publish_position_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::PositionFeature);
    cover.setCurrentPosition(250);

    assertTrue(cover.setPosition(250, true));
    assertSingleMqttMessage(AHATOFSTR(PositionTopic), "250", true)
}

AHA_TEST(CoverTest, command_open) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("OPEN"));

    assertCommandCallbackCalled(HACover::CommandOpen, &cover)
}

AHA_TEST(CoverTest, command_close) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("CLOSE"));

    assertCommandCallbackCalled(HACover::CommandClose, &cover)
}

AHA_TEST(CoverTest, command_stop) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("STOP"));

    assertCommandCallbackCalled(HACover::CommandStop, &cover)
}

AHA_TEST(CoverTest, command_invalid) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("NOT_SUPPORTED"));

    assertCommandCallbackNotCalled()
}

AHA_TEST(CoverTest, different_cover_command) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueCoverDifferent/cmd_t"),
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
