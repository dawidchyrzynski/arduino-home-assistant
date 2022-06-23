#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
static const char* configTopic = "homeassistant/sensor/testDevice/uniqueSensor/config";
static const char* stateTopic = "testData/testDevice/uniqueSensor/stat_t";

test(SensorTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HASensor sensor(nullptr);
    sensor.buildSerializerTest();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SensorTest, default_params) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, availability) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HASensor
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSensor/avty_t",
        "online",
        true
    )
}

test(SensorTest, name_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setName("testName");

    assertEntityConfig(
        mock,
        sensor,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, device_class_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, force_update_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setForceUpdate(true);

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"frc_upd\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setIcon("testIcon");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"unit_of_meas\":\"%\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, value_template_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setValueTemplate("abc");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"abc\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, publish_value) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId);
    bool result = sensor.setValue("test123");

    assertSingleMqttMessage(stateTopic, "test123", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_value_on_connect) {
    initMqttTest(testDeviceId)

    HASensorInteger sensor(testUniqueId);
    sensor.setCurrentValue(520);
    mqtt.loop();

    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSensor/stat_t",
        "520",
        true
    )
}

test(SensorIntegerTest, publish_int8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    int8_t value = 127;

    bool result = sensor.setValue(value);

    assertSingleMqttMessage(stateTopic, "127", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_uint8) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    uint8_t value = 50;

    bool result = sensor.setValue(value);

    assertSingleMqttMessage(stateTopic, "50", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_int16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    int16_t value = 32766;

    bool result = sensor.setValue(value);

    assertSingleMqttMessage(stateTopic, "32766", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_uint16) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    uint16_t value = 65534;

    bool result = sensor.setValue(value);

    assertSingleMqttMessage(stateTopic, "65534", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_int32) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    int32_t value = 2147483646;

    bool result = sensor.setValue(value);

    assertSingleMqttMessage(stateTopic, "2147483646", true)
    assertTrue(result);
}

test(SensorIntegerTest, publish_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    uint8_t value = 50;

    sensor.setCurrentValue(value);
    bool result = sensor.setValue(value);

    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

test(SensorIntegerTest, publish_force) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorInteger sensor(testUniqueId);
    uint8_t value = 50;

    sensor.setCurrentValue(value);
    bool result = sensor.setValue(value, true);

    assertSingleMqttMessage(stateTopic, "50", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_value_on_connect) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId);
    sensor.setCurrentValue(520.5235);
    mqtt.loop();

    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSensor/stat_t",
        "52052",
        true
    )
}

test(SensorFloatTest, config_p0) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP0);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorFloatTest, config_p1) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP1);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"{{float(value)/10**1}}\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorFloatTest, config_p2) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP2);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"{{float(value)/10**2}}\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorFloatTest, config_p3) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP3);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"{{float(value)/10**3}}\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorFloatTest, config_p4) {
    initMqttTest(testDeviceId)

    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP4);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"{{float(value)/10**4}}\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorFloatTest, publish_p0) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP0);
    bool result = sensor.setValue(173.5426);

    assertSingleMqttMessage(stateTopic, "173", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_p1) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP1);
    bool result = sensor.setValue(173.5426);

    assertSingleMqttMessage(stateTopic, "1735", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_p2) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP2);
    bool result = sensor.setValue(173.1534);

    assertSingleMqttMessage(stateTopic, "17315", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_p3) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP3);
    bool result = sensor.setValue(173.333);

    assertSingleMqttMessage(stateTopic, "173333", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_p4) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId, HASensorFloat::PrecisionP4);
    bool result = sensor.setValue(173.33356);

    assertSingleMqttMessage(stateTopic, "1733335", true)
    assertTrue(result);
}

test(SensorFloatTest, publish_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId);
    sensor.setCurrentValue(1555.555);
    bool result = sensor.setValue(1555.555);

    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

test(SensorFloatTest, publish_force) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensorFloat sensor(testUniqueId);
    sensor.setCurrentValue(1555.555);
    bool result = sensor.setValue(1555.555, true);

    assertSingleMqttMessage(stateTopic, "155555", true)
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
