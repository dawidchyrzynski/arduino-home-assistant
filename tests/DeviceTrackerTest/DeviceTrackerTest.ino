#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueTracker";

const char ConfigTopic[] PROGMEM = {
    "homeassistant/device_tracker/testDevice/uniqueTracker/config"
};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueTracker/stat_t"};

AHA_TEST(DeviceTrackerTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(nullptr);
    tracker.buildSerializerTest();
    HASerializer* serializer = tracker.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(DeviceTrackerTest, default_params) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, extended_unique_id) {
    initMqttTest(testDeviceId)

    device.enableExtendedUniqueIds();
    HADeviceTracker tracker(testUniqueId);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueTracker\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, source_type_gps) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeGPS);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"src_type\":\"gps\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, source_type_router) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeRouter);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"src_type\":\"router\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, source_type_bluetooth) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeBluetooth);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"src_type\":\"bluetooth\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, source_type_bluetooth_le) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeBluetoothLE);
    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"src_type\":\"bluetooth_le\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, availability) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HADeviceTracker
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueTracker/avty_t"),
        "online",
        true
    )
}

AHA_TEST(DeviceTrackerTest, publish_initial_state) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(StateTopic), "home", true)
}

AHA_TEST(DeviceTrackerTest, name_setter) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setName("testName");

    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueTracker\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, object_id_setter) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setObjectId("testId");

    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueTracker\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, icon_setter) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setIcon("testIcon");

    assertEntityConfig(
        mock,
        tracker,
        (
            "{"
            "\"uniq_id\":\"uniqueTracker\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTrackerTest, default_state_unknown) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    assertEqual(HADeviceTracker::StateUnknown, tracker.getState());
}

AHA_TEST(DeviceTrackerTest, default_state) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateNotAvailable);
    assertEqual(HADeviceTracker::StateNotAvailable, tracker.getState());
}

AHA_TEST(DeviceTrackerTest, publish_state_home) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateHome);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "home", true)
    assertTrue(result);
}

AHA_TEST(DeviceTrackerTest, publish_state_not_home) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateNotHome);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "not_home", true)
    assertTrue(result);
}

AHA_TEST(DeviceTrackerTest, publish_state_not_available) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateNotAvailable);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "offline", true)
    assertTrue(result);
}

AHA_TEST(DeviceTrackerTest, publish_state_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    bool result = tracker.setState(HADeviceTracker::StateHome);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

AHA_TEST(DeviceTrackerTest, publish_state_debounce_skip) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    bool result = tracker.setState(HADeviceTracker::StateHome, true);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "home", true)
    assertTrue(result);
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
