#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackCommand = unknownCommand; \
    commandCallbackCoverPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedCommand, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedCommand, commandCallbackCommand); \
    assertEqual(callerPtr, commandCallbackCoverPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCover";
static const char* configTopic = "homeassistant/cover/testDevice/uniqueCover/config";
static const char* stateTopic = "testData/testDevice/uniqueCover/stat_t";
static const char* positionTopic = "testData/testDevice/uniqueCover/pos_t";
static const char* commandTopic = "testData/testDevice/uniqueCover/cmd_t";
static const HACover::CoverCommand unknownCommand = static_cast<HACover::CoverCommand>(0);

static bool commandCallbackCalled = false;
static HACover::CoverCommand commandCallbackCommand = unknownCommand;
static HACover* commandCallbackCoverPtr = nullptr;

void onCommandReceived(HACover::CoverCommand command, HACover* cover)
{
    commandCallbackCalled = true;
    commandCallbackCommand = command;
    commandCallbackCoverPtr = cover;
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
        "{\"uniq_id\":\"uniqueCover\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(CoverTest, command_subscription) {
    prepareTest

    HACover cover(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(commandTopic, mock->getSubscriptions()[0]->topic);
}

AHA_TEST(CoverTest, availability) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACover
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueCover/avty_t",
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

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(1, stateTopic, "closed", true)
    assertMqttMessage(2, positionTopic, "100", true)
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
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueCover\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

AHA_TEST(CoverTest, device_class) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

AHA_TEST(CoverTest, icon_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setIcon("testIcon");

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

AHA_TEST(CoverTest, retain_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setRetain(true);

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

AHA_TEST(CoverTest, optimistic_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setOptimistic(true);

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"opt\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
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
    bool result = cover.setState(HACover::StateClosed);

    assertSingleMqttMessage(stateTopic, "closed", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_closing) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    bool result = cover.setState(HACover::StateClosing);

    assertSingleMqttMessage(stateTopic, "closing", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_open) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    bool result = cover.setState(HACover::StateOpen);

    assertSingleMqttMessage(stateTopic, "open", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_opening) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    bool result = cover.setState(HACover::StateOpening);

    assertSingleMqttMessage(stateTopic, "opening", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_stopped) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    bool result = cover.setState(HACover::StateStopped);

    assertSingleMqttMessage(stateTopic, "stopped", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateStopped);
    bool result = cover.setState(HACover::StateStopped);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentState(HACover::StateStopped);
    bool result = cover.setState(HACover::StateStopped, true);

    assertSingleMqttMessage(stateTopic, "stopped", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_position) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    bool result = cover.setPosition(250);

    assertSingleMqttMessage(positionTopic, "250", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_position_debounce) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentPosition(250);
    bool result = cover.setPosition(250);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

AHA_TEST(CoverTest, publish_position_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HACover cover(testUniqueId);
    cover.setCurrentPosition(250);
    bool result = cover.setPosition(250, true);

    assertSingleMqttMessage(positionTopic, "250", true)
    assertTrue(result);
}

AHA_TEST(CoverTest, command_open) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "OPEN");

    assertCallback(true, HACover::CommandOpen, &cover)
}

AHA_TEST(CoverTest, command_close) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "CLOSE");

    assertCallback(true, HACover::CommandClose, &cover)
}

AHA_TEST(CoverTest, command_stop) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "STOP");

    assertCallback(true, HACover::CommandStop, &cover)
}

AHA_TEST(CoverTest, command_invalid) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "NOT_SUPPORTED");

    assertCallback(false, unknownCommand, nullptr)
}

AHA_TEST(CoverTest, different_cover_command) {
    prepareTest

    HACover cover(testUniqueId);
    cover.onCommand(onCommandReceived);
    mock->fakeMessage(
        "testData/testDevice/uniqueCoverDifferent/cmd_t",
        "CLOSE"
    );

    assertCallback(false, unknownCommand, nullptr)
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
