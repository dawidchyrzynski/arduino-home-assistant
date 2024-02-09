#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastStateCallbackCall.reset(); \
    lastSpeedCallbackCall.reset();

#define assertStateCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastStateCallbackCall.called); \
    assertEqual(static_cast<bool>(expectedState), lastStateCallbackCall.state); \
    assertEqual(callerPtr, lastStateCallbackCall.caller);

#define assertStateCallbackNotCalled() \
    assertFalse(lastStateCallbackCall.called);

#define assertSpeedCallbackCalled(expectedSpeed, callerPtr) \
    assertTrue(lastSpeedCallbackCall.called); \
    assertEqual((uint16_t)expectedSpeed, lastSpeedCallbackCall.speed); \
    assertEqual(callerPtr, lastSpeedCallbackCall.caller);

#define assertSpeedCallbackNotCalled() \
    assertFalse(lastSpeedCallbackCall.called);

using aunit::TestRunner;

struct StateCallback {
    bool called = false;
    bool state = false;
    HAFan* caller = nullptr;

    void reset() {
        called = false;
        state = false;
        caller = nullptr;
    }
};

struct SpeedCallback {
    bool called = false;
    uint16_t speed = 0;
    HAFan* caller = nullptr;

    void reset() {
        called = false;
        speed = 0;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueFan";
static StateCallback lastStateCallbackCall;
static SpeedCallback lastSpeedCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/fan/testDevice/uniqueFan/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueFan/stat_t"};
const char SpeedPercentageTopic[] PROGMEM = {"testData/testDevice/uniqueFan/pct_stat_t"};
const char StateCommandTopic[] PROGMEM = {"testData/testDevice/uniqueFan/cmd_t"};
const char SpeedPercentageCommandTopic[] PROGMEM = {"testData/testDevice/uniqueFan/pct_cmd_t"};

void onStateCommandReceived(bool state, HAFan* caller)
{
    lastStateCallbackCall.called = true;
    lastStateCallbackCall.state = state;
    lastStateCallbackCall.caller = caller;
}

void onSpeedCommandReceived(uint16_t speed, HAFan* caller)
{
    lastSpeedCallbackCall.called = true;
    lastSpeedCallbackCall.speed = speed;
    lastSpeedCallbackCall.caller = caller;
}

AHA_TEST(FanTest, invalid_unique_id) {
    prepareTest

    HAFan fan(nullptr);
    fan.buildSerializerTest();
    HASerializer* serializer = fan.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(FanTest, default_params) {
    prepareTest

    HAFan fan(testUniqueId);
    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(FanTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HAFan fan(testUniqueId);
    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueFan\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(FanTest, default_params_with_speed) {
    prepareTest

    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"pct_stat_t\":\"testData/testDevice/uniqueFan/pct_stat_t\","
            "\"pct_cmd_t\":\"testData/testDevice/uniqueFan/pct_cmd_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
    assertEqual(3, mock->getFlushedMessagesNb()); // config + default state + default speed
}

AHA_TEST(FanTest, state_command_subscription) {
    prepareTest

    HAFan fan(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(StateCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(FanTest, speed_command_subscription) {
    prepareTest

    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(SpeedPercentageCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(FanTest, availability) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAFan
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueFan/avty_t"),
        "online",
        true
    )
}

AHA_TEST(FanTest, publish_last_known_state) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setCurrentState(true);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(FanTest, publish_last_known_speed) {
    prepareTest

    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    fan.setCurrentSpeed(50);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(2, AHATOFSTR(SpeedPercentageTopic), "50", true)
}

AHA_TEST(FanTest, publish_nothing_if_retained) {
    prepareTest

    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    fan.setRetain(true);
    fan.setCurrentState(true);
    fan.setCurrentSpeed(50);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(FanTest, name_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setName("testName");

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueFan\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, object_id_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setObjectId("testId");

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueFan\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, icon_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setIcon("testIcon");

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, retain_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setRetain(true);

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, optimistic_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setOptimistic(true);

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, speed_range_setter) {
    prepareTest

    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    fan.setSpeedRangeMin(10);
    fan.setSpeedRangeMax(1000);

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"pct_stat_t\":\"testData/testDevice/uniqueFan/pct_stat_t\","
            "\"pct_cmd_t\":\"testData/testDevice/uniqueFan/pct_cmd_t\","
            "\"spd_rng_max\":1000,"
            "\"spd_rng_min\":10,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, speed_range_setter_feature_disabled) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setSpeedRangeMin(10);
    fan.setSpeedRangeMax(1000);

    assertEntityConfig(
        mock,
        fan,
        (
            "{"
            "\"uniq_id\":\"uniqueFan\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueFan/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueFan/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(FanTest, current_state_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setCurrentState(true);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertTrue(fan.getCurrentState());
}

AHA_TEST(FanTest, current_speed_setter) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.setCurrentSpeed(50);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual((uint16_t)50, fan.getCurrentSpeed());
}

AHA_TEST(FanTest, publish_state) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId);

    assertTrue(fan.setState(true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(FanTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId);
    fan.setCurrentState(true);

    // it shouldn't publish data if state doesn't change
    assertTrue(fan.setState(true));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(FanTest, publish_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId);
    fan.setCurrentState(true);

    assertTrue(fan.setState(true, true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(FanTest, publish_nothing_if_speed_feature_is_disabled) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId);

    assertFalse(fan.setSpeed(50));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(FanTest, publish_speed) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId, HAFan::SpeedsFeature);

    assertTrue(fan.setSpeed(50));
    assertSingleMqttMessage(AHATOFSTR(SpeedPercentageTopic), "50", true)
}

AHA_TEST(FanTest, publish_speed_debounce) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    fan.setCurrentSpeed(50);

    // it shouldn't publish data if state doesn't change
    assertTrue(fan.setSpeed(50));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(FanTest, publish_speed_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAFan fan(testUniqueId, HAFan::SpeedsFeature);
    fan.setCurrentSpeed(50);

    assertTrue(fan.setSpeed(50, true));
    assertSingleMqttMessage(AHATOFSTR(SpeedPercentageTopic), "50", true)
}

AHA_TEST(FanTest, state_command_on) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("ON"));

    assertStateCallbackCalled(true, &fan)
}

AHA_TEST(FanTest, state_command_off) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("OFF"));

    assertStateCallbackCalled(false, &fan)
}

AHA_TEST(FanTest, state_command_different_fan) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueFanDifferent/cmd_t"),
        F("ON")
    );

    assertStateCallbackNotCalled()
}

AHA_TEST(FanTest, speed_command_half) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onSpeedCommand(onSpeedCommandReceived);
    mock->fakeMessage(AHATOFSTR(SpeedPercentageCommandTopic), F("50"));

    assertSpeedCallbackCalled(50, &fan)
}

AHA_TEST(FanTest, speed_command_max) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onSpeedCommand(onSpeedCommandReceived);
    mock->fakeMessage(AHATOFSTR(SpeedPercentageCommandTopic), F("100"));

    assertSpeedCallbackCalled(100, &fan)
}

AHA_TEST(FanTest, speed_command_in_range) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onSpeedCommand(onSpeedCommandReceived);
    fan.setSpeedRangeMin(1000);
    fan.setSpeedRangeMax(50000);
    mock->fakeMessage(AHATOFSTR(SpeedPercentageCommandTopic), F("49999"));

    assertSpeedCallbackCalled(49999, &fan)
}

AHA_TEST(FanTest, speed_command_invalid) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onSpeedCommand(onSpeedCommandReceived);
    mock->fakeMessage(AHATOFSTR(SpeedPercentageCommandTopic), F("INVALID"));

    assertSpeedCallbackNotCalled()
}

AHA_TEST(FanTest, speed_command_different_fan) {
    prepareTest

    HAFan fan(testUniqueId);
    fan.onSpeedCommand(onSpeedCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueFanDifferent/pct_cmd_t"),
        F("50")
    );

    assertSpeedCallbackNotCalled()
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
