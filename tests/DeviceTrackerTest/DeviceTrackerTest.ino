#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueTracker";
static const char* configTopic = "homeassistant/device_tracker/testDevice/uniqueTracker/config";
static const char* stateTopic = "testData/testDevice/uniqueTracker/stat_t";

test(DeviceTrackerTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(nullptr);
    tracker.buildSerializerTest();
    HASerializer* serializer = tracker.getSerializer();

    assertTrue(serializer == nullptr);
}

test(DeviceTrackerTest, default_params) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, source_type_gps) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeGPS);
    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"src_type\":\"gps\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, source_type_router) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeRouter);
    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"src_type\":\"router\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, source_type_bluetooth) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeBluetooth);
    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"src_type\":\"bluetooth\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, source_type_bluetooth_le) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setSourceType(HADeviceTracker::SourceTypeBluetoothLE);
    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"src_type\":\"bluetooth_le\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, availability) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HADeviceTracker
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueTracker/avty_t",
        "online",
        true
    )
}

test(DeviceTrackerTest, publish_initial_state) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    mqtt.loop();

    assertMqttMessage(
        1,
        "testData/testDevice/uniqueTracker/stat_t",
        "home",
        true
    )
}

test(DeviceTrackerTest, name_setter) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setName("testName");

    assertEntityConfig(
        mock,
        tracker,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueTracker\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, icon_setter) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setIcon("testIcon");

    assertEntityConfig(
        mock,
        tracker,
        "{\"uniq_id\":\"uniqueTracker\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueTracker/stat_t\"}"
    )
}

test(DeviceTrackerTest, default_state_unknown) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    assertEqual(HADeviceTracker::StateUnknown, tracker.getState());
}

test(DeviceTrackerTest, default_state) {
    initMqttTest(testDeviceId)

    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateNotAvailable);
    assertEqual(HADeviceTracker::StateNotAvailable, tracker.getState());
}

test(DeviceTrackerTest, publish_state_home) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateHome);

    assertSingleMqttMessage(stateTopic, "home", true)
    assertTrue(result);
}

test(DeviceTrackerTest, publish_state_not_home) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateNotHome);

    assertSingleMqttMessage(stateTopic, "not_home", true)
    assertTrue(result);
}

test(DeviceTrackerTest, publish_state_not_available) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    bool result = tracker.setState(HADeviceTracker::StateNotAvailable);

    assertSingleMqttMessage(stateTopic, "offline", true)
    assertTrue(result);
}

test(DeviceTrackerTest, publish_state_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    bool result = tracker.setState(HADeviceTracker::StateHome);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

test(DeviceTrackerTest, publish_state_debounce_skip) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTracker tracker(testUniqueId);
    tracker.setCurrentState(HADeviceTracker::StateHome);
    bool result = tracker.setState(HADeviceTracker::StateHome, true);

    assertSingleMqttMessage(stateTopic, "home", true)
    assertTrue(result);
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
