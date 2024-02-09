#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
const char ConfigTopic[] PROGMEM = {"homeassistant/sensor/testDevice/uniqueSensor/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueSensor/stat_t"};
const char JsonAttributesTopic[] PROGMEM = {"testData/testDevice/uniqueSensor/json_attr_t"};

AHA_TEST(SensorTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HASensor sensor(nullptr);
    sensor.buildSerializerTest();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(SensorTest, default_params) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, extended_unique_id) {
    initMqttTest(testDeviceId)

    device.enableExtendedUniqueIds();
    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, availability) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HASensor
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueSensor/avty_t"),
        "online",
        true
    )
}

AHA_TEST(SensorTest, name_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, object_id_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, device_class_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, state_class_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setStateClass("measurement");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"stat_cla\":\"measurement\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(SensorTest, force_update_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setForceUpdate(true);

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"frc_upd\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(SensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"unit_of_meas\":\"%\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(SensorTest, expire_after_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, expire_after_zero_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
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

AHA_TEST(SensorTest, json_attributes_topic) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId, HASensor::JsonAttributesFeature);

    assertEntityConfig(
        mock,
        sensor,
        (
            "{"
            "\"uniq_id\":\"uniqueSensor\","
            "\"json_attr_t\":\"testData/testDevice/uniqueSensor/json_attr_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\""
            "}"
        )
    )
}

AHA_TEST(SensorTest, publish_value) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId);

    assertTrue(sensor.setValue("test123"));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "test123", true)
}

AHA_TEST(SensorTest, publish_null_value) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId);

    assertTrue(sensor.setValue(nullptr));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "None", true)
}

AHA_TEST(SensorTest, publish_json_attributes) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId, HASensor::JsonAttributesFeature);

    assertTrue(sensor.setJsonAttributes("{\"dummy\": 1}"));
    assertSingleMqttMessage(AHATOFSTR(JsonAttributesTopic), "{\"dummy\": 1}", true)
}

test(SensorNumberTest, publish_value_on_connect) {
    initMqttTest(testDeviceId)

    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(520);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(StateTopic), "520", true)
}

test(SensorNumberTest, dont_publish_default_value_on_connect) {
    initMqttTest(testDeviceId)

    HASensorNumber sensor(testUniqueId);
    mqtt.loop();

    assertFalse(sensor.getCurrentValue().isSet());
    assertEqual(mock->getFlushedMessagesNb(), 1); // config only
}

test(SensorNumberTest, publish_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint16_t value = 1555;
    sensor.setCurrentValue(value);

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toUInt16());
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

test(SensorNumberTest, publish_force) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint16_t value = 1555;
    sensor.setCurrentValue(value);

    assertTrue(sensor.setValue(value, true));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toUInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "1555", true)
}

test(SensorNumberTest, publish_int_zero) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(50);
    int8_t value = 0;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toInt8());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true)
}

test(SensorNumberTest, publish_int8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int8_t value = 127;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toInt8());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "127", true)
}

test(SensorNumberTest, publish_uint8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint8_t value = 50;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toUInt8());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "50", true)
}

test(SensorNumberTest, publish_int16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int16_t value = 32766;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "32766", true)
}

test(SensorNumberTest, publish_uint16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint16_t value = 65534;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toUInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "65534", true)
}

test(SensorNumberTest, publish_int32) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int32_t value = 2147483646;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toInt32());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "2147483646", true)
}

test(SensorNumberTest, publish_int32_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int32_t value = -2147483646;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toInt32());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-2147483646", true)
}

test(SensorNumberTest, publish_uint32) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint32_t value = 4294967295;

    assertTrue(sensor.setValue(value));
    assertTrue(sensor.getCurrentValue().isSet());
    assertEqual(value, sensor.getCurrentValue().toUInt32());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "4294967295", true)
}

test(SensorNumberTest, publish_p0) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValue(173.5426f));
    assertFalse(sensor.getCurrentValue().isFloat());
    assertEqual((uint16_t)173, sensor.getCurrentValue().toUInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173", true)
}

test(SensorNumberTest, publish_p0_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValue(0.050f, true));
    assertFalse(sensor.getCurrentValue().isFloat());
    assertEqual((uint16_t)0, sensor.getCurrentValue().toUInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true)
}

test(SensorNumberTest, publish_p0_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValue(-0.050f, true));
    assertFalse(sensor.getCurrentValue().isFloat());
    assertEqual((uint16_t)0, sensor.getCurrentValue().toUInt16());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true)
}

test(SensorNumberTest, publish_p1) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValue(173.5426f));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(173.5f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.5", true)
}

test(SensorNumberTest, publish_p1_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValue(0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(0.1f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.1", true)
}

test(SensorNumberTest, publish_p1_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValue(-0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(-0.1f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.1", true)
}

test(SensorNumberTest, publish_p2) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);

    assertTrue(sensor.setValue(173.1534f));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(173.15f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.15", true)
}

test(SensorNumberTest, publish_p2_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);

    assertTrue(sensor.setValue(0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(0.12f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.12", true)
}

test(SensorNumberTest, publish_p2_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);

    assertTrue(sensor.setValue(-0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(-0.12f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.12", true)
}

test(SensorNumberTest, publish_p3) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValue(173.333f));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(173.333f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.333", true)
}

test(SensorNumberTest, publish_p3_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValue(0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(0.123f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.123", true)
}

test(SensorNumberTest, publish_p3_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValue(-0.123f, true));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(-0.123f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.123", true)
}

test(SensorNumberTest, publish_p3_smaller) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValue(173.3f));
    assertTrue(sensor.getCurrentValue().isFloat());
    assertNear(173.3f, sensor.getCurrentValue().toFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.300", true)
}

test(SensorNumberTest, publish_precision_mismatch) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertFalse(sensor.setValue(HANumeric(25.0f, 1)));
    assertFalse(sensor.getCurrentValue().isSet());
    assertEqual(mock->getFlushedMessagesNb(), 0);
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
