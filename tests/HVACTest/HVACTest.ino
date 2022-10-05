#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId)

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueHVAC";

const char ConfigTopic[] PROGMEM = {"homeassistant/climate/testDevice/uniqueHVAC/config"};
const char CurrentTemperatureTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ctt"};
const char ActionTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/at"};

AHA_TEST(HVACTest, invalid_unique_id) {
    prepareTest

    HAHVAC hvac(nullptr);
    hvac.buildSerializerTest();
    HASerializer* serializer = hvac.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(HVACTest, default_params) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, default_params_with_action) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"at\":\"testData/testDevice/uniqueHVAC/at\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, availability) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAFan
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueHVAC/avty_t"),
        "online",
        true
    )
}

AHA_TEST(HVACTest, name_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setName("testName");

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, icon_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setIcon("testIcon");

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ic\":\"testIcon\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, retain_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setRetain(true);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ret\":true,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, temperature_unit_c_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setTemperatureUnit(HAHVAC::CelsiusUnit);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_unit\":\"C\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, temperature_unit_f_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setTemperatureUnit(HAHVAC::FahrenheitUnit);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_unit\":\"F\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, min_temp_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setMinTemp(25.555);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"min_temp\":25.5,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, min_temp_setter_p2) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP2);
    hvac.setMinTemp(25.555);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"min_temp\":25.55,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, max_temp_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setMaxTemp(25.555);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"max_temp\":25.5,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, max_temp_setter_p2) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP2);
    hvac.setMaxTemp(25.555);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"max_temp\":25.55,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, temp_step_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setTempStep(0.5);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_step\":0.5,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, temp_step_setter_p2) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP2);
    hvac.setTempStep(0.05);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_step\":0.05,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, publish_nothing_if_retained) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);
    hvac.setRetain(true);
    hvac.setCurrentTemperature(21.5);
    hvac.setCurrentAction(HAHVAC::CoolingAction);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(HVACTest, publish_current_temperature_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_current_temperature) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);

    assertTrue(hvac.setCurrentTemperature(21.5));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.5", true) 
}

AHA_TEST(HVACTest, publish_current_temperature_p2) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP2);

    assertTrue(hvac.setCurrentTemperature(21.5));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.50", true) 
}

AHA_TEST(HVACTest, publish_current_temperature_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);

    // it shouldn't publish data if state doesn't change
    assertTrue(hvac.setCurrentTemperature(21.5));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_current_temperature_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);

    assertTrue(hvac.setCurrentTemperature(21.5, true));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_action_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);
    hvac.setCurrentAction(HAHVAC::OffAction);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(ActionTopic), "off", true)
}

AHA_TEST(HVACTest, publish_action_off) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::OffAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "off", true) 
}

AHA_TEST(HVACTest, publish_action_heating) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::HeatingAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "heating", true) 
}

AHA_TEST(HVACTest, publish_action_cooling) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::CoolingAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "cooling", true) 
}

AHA_TEST(HVACTest, publish_action_drying) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::DryingAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "drying", true) 
}

AHA_TEST(HVACTest, publish_action_idle) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::IdleAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "idle", true) 
}

AHA_TEST(HVACTest, publish_action_fan) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);

    assertTrue(hvac.setAction(HAHVAC::FanAction));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "fan", true) 
}

AHA_TEST(HVACTest, publish_action_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);
    hvac.setCurrentAction(HAHVAC::FanAction);
        
    assertTrue(hvac.setAction(HAHVAC::FanAction));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_action_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature);
    hvac.setCurrentAction(HAHVAC::FanAction);

    assertTrue(hvac.setAction(HAHVAC::FanAction, true));
    assertSingleMqttMessage(AHATOFSTR(ActionTopic), "fan", true) 
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
