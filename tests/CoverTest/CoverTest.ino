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

#define assertSetPositionCallbackCalled(expectedPosition, callerPtr) \
    assertTrue(lastSetPositionCallbackCall.called); \
    assertEqual((uint8_t)expectedPosition, lastSetPositionCallbackCall.position); \
    assertEqual(callerPtr, lastSetPositionCallbackCall.caller);

#define assertSetPositionCallbackNotCalled() \
    assertFalse(lastSetPositionCallbackCall.called);

#define assertTiltCallbackCalled(expectedTilt, callerPtr) \
    assertTrue(lastTiltCallbackCall.called); \
    assertEqual((uint8_t)expectedTilt, lastTiltCallbackCall.tilt); \
    assertEqual(callerPtr, lastTiltCallbackCall.caller);

#define assertTiltCallbackNotCalled() \
    assertFalse(lastTiltCallbackCall.called);

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

struct SetPositionCallback {
    bool called = false;
    uint8_t position = 0;
    HACover* caller = nullptr;

    void reset() {
        called = false;
        position = 0;
        caller = nullptr;
    }
};

struct TiltCallback {
    bool called = false;
    uint8_t tilt = 0;
    HACover* caller = nullptr;

    void reset() {
        called = false;
        tilt = 0;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCover";
static CommandCallback lastCommandCallbackCall;
static SetPositionCallback lastSetPositionCallbackCall;
static TiltCallback lastTiltCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/cover/testDevice/uniqueCover/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueCover/stat_t"};
const char PositionTopic[] PROGMEM = {"testData/testDevice/uniqueCover/pos_t"};
const char SetPositionCommandTopic[] PROGMEM = {"testData/testDevice/uniqueCover/set_pos_t"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueCover/cmd_t"};
const char TiltStatusTopic[] PROGMEM = {"testData/testDevice/uniqueCover/tilt_status_t"};
const char TiltCommandTopic[] PROGMEM = {"testData/testDevice/uniqueCover/tilt_cmd_t"};

void onCommandReceived(HACover::CoverCommand command, HACover* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.command = command;
    lastCommandCallbackCall.caller = caller;
}

void onSetPositionCommandReceived(uint8_t position, HACover* caller)
{
    lastSetPositionCallbackCall.called = true;
    lastSetPositionCallbackCall.position = position;
    lastSetPositionCallbackCall.caller = caller;
}

void onTiltCommandReceived(uint8_t tilt, HACover* caller)
{
    lastTiltCallbackCall.called = true;
    lastTiltCallbackCall.tilt = tilt;
    lastTiltCallbackCall.caller = caller;
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

AHA_TEST(CoverTest, default_params_with_set_position) {
    prepareTest

    HACover cover(testUniqueId, HACover::PositionFeature | HACover::SetPositionFeature);
    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\","
            "\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\","
            "\"set_pos_t\":\"testData/testDevice/uniqueCover/set_pos_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, default_params_with_tilt) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    assertEntityConfig(
        mock,
        cover,
        (
            "{"
            "\"uniq_id\":\"uniqueCover\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\","
            "\"tilt_status_t\":\"testData/testDevice/uniqueCover/tilt_status_t\","
            "\"tilt_cmd_t\":\"testData/testDevice/uniqueCover/tilt_cmd_t\""
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

AHA_TEST(CoverTest, set_position_command_subscription) {
    prepareTest

    HACover cover(testUniqueId, HACover::SetPositionFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(SetPositionCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(CoverTest, tilt_command_subscription) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(TiltCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
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

AHA_TEST(CoverTest, publish_last_known_state_with_tilt) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.setCurrentState(HACover::StateClosed);
    cover.setCurrentTilt(100);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "closed", true)
    assertMqttMessage(2, AHATOFSTR(TiltStatusTopic), "100", true)
}

AHA_TEST(CoverTest, publish_nothing_if_retained) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setRetain(true);
    cover.setCurrentState(HACover::StateClosed);
    cover.setCurrentPosition(100);
    cover.setCurrentTilt(100);
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

AHA_TEST(CoverTest, current_tilt_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setCurrentTilt(500);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(500, cover.getCurrentTilt());
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

AHA_TEST(CoverTest, publish_tilt) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::TiltFeature);

    assertTrue(cover.setTilt(250));
    assertSingleMqttMessage(AHATOFSTR(TiltStatusTopic), "250", true)
}

AHA_TEST(CoverTest, publish_tilt_debounce) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.setCurrentTilt(250);

    // it shouldn't publish data if state doesn't change
    assertTrue(cover.setTilt(250));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(CoverTest, publish_tilt_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.setCurrentTilt(250);

    assertTrue(cover.setTilt(250, true));
    assertSingleMqttMessage(AHATOFSTR(TiltStatusTopic), "250", true)
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

AHA_TEST(CoverTest, set_position_command_min) {
    prepareTest

    HACover cover(testUniqueId, HACover::SetPositionFeature);
    cover.onSetPositionCommand(onSetPositionCommandReceived);
    mock->fakeMessage(AHATOFSTR(SetPositionCommandTopic), F("0"));

    assertSetPositionCallbackCalled(0, &cover)
}

AHA_TEST(CoverTest, set_position_command_max) {
    prepareTest

    HACover cover(testUniqueId, HACover::SetPositionFeature);
    cover.onSetPositionCommand(onSetPositionCommandReceived);
    mock->fakeMessage(AHATOFSTR(SetPositionCommandTopic), F("100"));

    assertSetPositionCallbackCalled(100, &cover)
}

AHA_TEST(CoverTest, set_position_command_invalid) {
    prepareTest

    HACover cover(testUniqueId, HACover::SetPositionFeature);
    cover.onSetPositionCommand(onSetPositionCommandReceived);
    mock->fakeMessage(AHATOFSTR(SetPositionCommandTopic), F("INVALID"));

    assertSetPositionCallbackNotCalled()
}

AHA_TEST(CoverTest, set_position_command_different_cover) {
    prepareTest

    HACover cover(testUniqueId, HACover::SetPositionFeature);
    cover.onSetPositionCommand(onSetPositionCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueCoverDifferent/set_pos_t"),
        F("100")
    );

    assertSetPositionCallbackNotCalled()
}

AHA_TEST(CoverTest, tilt_command_min) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.onTiltCommand(onTiltCommandReceived);
    mock->fakeMessage(AHATOFSTR(TiltCommandTopic), F("0"));

    assertTiltCallbackCalled(0, &cover)
}

AHA_TEST(CoverTest, tilt_command_max) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.onTiltCommand(onTiltCommandReceived);
    mock->fakeMessage(AHATOFSTR(TiltCommandTopic), F("100"));

    assertTiltCallbackCalled(100, &cover)
}

AHA_TEST(CoverTest, tilt_command_invalid) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.onTiltCommand(onTiltCommandReceived);
    mock->fakeMessage(AHATOFSTR(TiltCommandTopic), F("INVALID"));

    assertTiltCallbackNotCalled()
}

AHA_TEST(CoverTest, tilt_command_different_cover) {
    prepareTest

    HACover cover(testUniqueId, HACover::TiltFeature);
    cover.onTiltCommand(onTiltCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueCoverDifferent/tilt_cmd_t"),
        F("100")
    );

    assertTiltCallbackNotCalled()
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
