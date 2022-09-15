#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
const char ConfigTopic[] PROGMEM = {"homeassistant/sensor/testDevice/uniqueSensor/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueSensor/stat_t"};
const char DummyTemplateStr[] PROGMEM = {"dummyTemplate"};

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
        "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
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
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

AHA_TEST(SensorTest, device_class_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

AHA_TEST(SensorTest, force_update_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setForceUpdate(true);

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"frc_upd\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

AHA_TEST(SensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setIcon("testIcon");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

AHA_TEST(SensorTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"unit_of_meas\":\"%\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

AHA_TEST(SensorTest, publish_value) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId);

    assertTrue(sensor.setValue("test123"));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "test123", true)
}

test(SensorNumberTest, publish_value_on_connect) {
    initMqttTest(testDeviceId)

    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(520);
    mqtt.loop();

    assertEqual((int32_t)520, sensor.getCurrentValue());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "520", true)
}

test(SensorNumberTest, publish_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(1555);

    assertTrue(sensor.setValue(1555));
    assertEqual((int32_t)1555, sensor.getCurrentValue());
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

test(SensorNumberTest, publish_force) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(1555);

    assertTrue(sensor.setValue(1555, true));
    assertEqual((int32_t)1555, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "1555", true)
}

test(SensorNumberTest, publish_int_zero) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    sensor.setCurrentValue(50);
    int8_t value = 0;

    assertTrue(sensor.setValue(value));
    assertEqual((int32_t)value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true)
}

test(SensorNumberTest, publish_int8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int8_t value = 127;

    assertTrue(sensor.setValue(value));
    assertEqual((int32_t)value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "127", true)
}

test(SensorNumberTest, publish_uint8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint8_t value = 50;

    assertTrue(sensor.setValue(value));
    assertEqual((int32_t)value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "50", true)
}

test(SensorNumberTest, publish_int16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int16_t value = 32766;

    assertTrue(sensor.setValue(value));
    assertEqual((int32_t)value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "32766", true)
}

test(SensorNumberTest, publish_uint16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    uint16_t value = 65534;

    assertTrue(sensor.setValue(value));
    assertEqual((int32_t)value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "65534", true)
}

test(SensorNumberTest, publish_int32) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int32_t value = 2147483646;

    assertTrue(sensor.setValue(value));
    assertEqual(value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "2147483646", true)
}

test(SensorNumberTest, publish_int32_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId);
    int32_t value = -2147483646;

    assertTrue(sensor.setValue(value));
    assertEqual(value, sensor.getCurrentValue());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-2147483646", true)
}

test(SensorNumberTest, publish_p0) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValueFloat(173.5426));
    assertNear((float)173, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173", true) 
}

test(SensorNumberTest, publish_p0_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValueFloat(0.050, true));
    assertNear((float)0, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true) 
}

test(SensorNumberTest, publish_p0_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP0);

    assertTrue(sensor.setValueFloat(-0.050, true));
    assertNear((float)0, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0", true) 
}

test(SensorNumberTest, publish_p1) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValueFloat(173.5426));
    assertNear(173.5, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.5", true)
}

test(SensorNumberTest, publish_p1_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValueFloat(0.123, true));
    assertNear(0.1, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.1", true) 
}

test(SensorNumberTest, publish_p1_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP1);

    assertTrue(sensor.setValueFloat(-0.123, true));
    assertNear(-0.1, sensor.getCurrentValueAsFloat(), 0.1);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.1", true) 
}

test(SensorNumberTest, publish_p2) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);
 
    assertTrue(sensor.setValueFloat(173.1534));
    assertNear(173.15, sensor.getCurrentValueAsFloat(), 0.01);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.15", true)
}

test(SensorNumberTest, publish_p2_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);

    assertTrue(sensor.setValueFloat(0.123, true));
    assertNear(0.12, sensor.getCurrentValueAsFloat(), 0.01);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.12", true) 
}

test(SensorNumberTest, publish_p2_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP2);

    assertTrue(sensor.setValueFloat(-0.123, true));
    assertNear(-0.12, sensor.getCurrentValueAsFloat(), 0.01);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.12", true) 
}

test(SensorNumberTest, publish_p3) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValueFloat(173.333));
    assertNear(173.333, sensor.getCurrentValueAsFloat(), 0.001);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.333", true)
}

test(SensorNumberTest, publish_p3_zero_unsigned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValueFloat(0.123, true));
    assertNear(0.123, sensor.getCurrentValueAsFloat(), 0.001);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "0.123", true) 
}

test(SensorNumberTest, publish_p3_zero_signed) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValueFloat(-0.123, true));
    assertNear(-0.123, sensor.getCurrentValueAsFloat(), 0.001);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.123", true) 
}

test(SensorNumberTest, publish_p3_smaller) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorNumber sensor(testUniqueId, HASensorNumber::PrecisionP3);

    assertTrue(sensor.setValueFloat(173.3));
    assertNear(173.3, sensor.getCurrentValueAsFloat(), 0.001);
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "173.300", true)
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
