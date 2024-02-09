#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";

const char ConfigTopic[] PROGMEM = {"homeassistant/binary_sensor/testDevice/uniqueSensor/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueSensor/stat_t"};

AHA_TEST(BinarySensorTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(nullptr);
    sensor.buildSerializerTest();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(BinarySensorTest, default_params) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, extended_unique_id) {
    initMqttTest(testDeviceId)

    device.enableExtendedUniqueIds();
    HABinarySensor sensor(testUniqueId);
    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueSensor\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, availability) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HABinarySensor
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueSensor/avty_t"),
        "online",
        true
    )
}

AHA_TEST(BinarySensorTest, publish_initial_state) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setCurrentState(true);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(BinarySensorTest, name_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setName("testName");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueSensor\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, object_id_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setObjectId("testId");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueSensor\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, device_class) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"dev_cla\":\"testClass\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setIcon("testIcon");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, expire_after_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setExpireAfter(60);

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"exp_aft\":60,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, expire_after_zero_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setExpireAfter(0);

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(BinarySensorTest, default_state_false) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    assertFalse(sensor.getCurrentState());
}

AHA_TEST(BinarySensorTest, default_state_true) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId);
    sensor.setCurrentState(true);
    assertTrue(sensor.getCurrentState());
}

AHA_TEST(BinarySensorTest, publish_state_on) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId);
    bool result = sensor.setState(!sensor.getCurrentState());

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
    assertTrue(result);
}

AHA_TEST(BinarySensorTest, publish_state_off) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId);
    sensor.setCurrentState(true);
    bool result = sensor.setState(!sensor.getCurrentState());

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "OFF", true)
    assertTrue(result);
}

AHA_TEST(BinarySensorTest, publish_state_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId);
    sensor.setCurrentState(true);
    bool result = sensor.setState(true);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

AHA_TEST(BinarySensorTest, publish_state_debounce_skip) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId);
    sensor.setCurrentState(true);
    bool result = sensor.setState(true, true);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
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
