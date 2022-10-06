#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastAuxStateCallbackCall.reset(); \
    lastPowerCallbackCall.reset();

#define assertAuxStateCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastAuxStateCallbackCall.called); \
    assertEqual(expectedState, lastAuxStateCallbackCall.state); \
    assertEqual(callerPtr, lastAuxStateCallbackCall.caller);

#define assertAuxStateCallbackNotCalled() \
    assertFalse(lastAuxStateCallbackCall.called);

#define assertPowerCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastPowerCallbackCall.called); \
    assertEqual(expectedState, lastPowerCallbackCall.state); \
    assertEqual(callerPtr, lastPowerCallbackCall.caller);

#define assertPowerCallbackNotCalled() \
    assertFalse(lastPowerCallbackCall.called);

using aunit::TestRunner;

struct AuxStateCallback {
    bool called = false;
    bool state = false;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        state = false;
        caller = nullptr;
    }
};

struct PowerCallback {
    bool called = false;
    bool state = false;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        state = false;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueHVAC";
static AuxStateCallback lastAuxStateCallbackCall;
static PowerCallback lastPowerCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/climate/testDevice/uniqueHVAC/config"};
const char CurrentTemperatureTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ctt"};
const char ActionTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/at"};
const char AuxStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ast"};
const char AuxCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/act"};
const char PowerCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/pow_cmd_t"};

void onAuxStateCommandReceived(bool state, HAHVAC* caller)
{
    lastAuxStateCallbackCall.called = true;
    lastAuxStateCallbackCall.state = state;
    lastAuxStateCallbackCall.caller = caller;
}

void onPowerCommandReceived(bool state, HAHVAC* caller)
{
    lastPowerCallbackCall.called = true;
    lastPowerCallbackCall.state = state;
    lastPowerCallbackCall.caller = caller;
}

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

AHA_TEST(HVACTest, default_params_with_aux) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"act\":\"testData/testDevice/uniqueHVAC/act\","
            "\"ast\":\"testData/testDevice/uniqueHVAC/ast\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + aux state
}

AHA_TEST(HVACTest, default_params_with_power) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::PowerFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"pow_cmd_t\":\"testData/testDevice/uniqueHVAC/pow_cmd_t\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, aux_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(AuxCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(HVACTest, power_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::PowerFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(PowerCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
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

    HAHVAC hvac(testUniqueId, HAHVAC::ActionFeature | HAHVAC::AuxHeatingFeature);
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

AHA_TEST(HVACTest, publish_aux_state_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    hvac.setCurrentAuxState(true);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(AuxStateTopic), "ON", true)
}

AHA_TEST(HVACTest, publish_aux_state_on) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);

    assertTrue(hvac.setAuxState(true));
    assertSingleMqttMessage(AHATOFSTR(AuxStateTopic), "ON", true) 
}

AHA_TEST(HVACTest, publish_aux_state_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    hvac.setCurrentAuxState(true);
        
    assertTrue(hvac.setAuxState(true));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_aux_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    hvac.setCurrentAuxState(false);

    assertTrue(hvac.setAuxState(false, true));
    assertSingleMqttMessage(AHATOFSTR(AuxStateTopic), "OFF", true) 
}

AHA_TEST(HVACTest, aux_state_command_on) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    hvac.onAuxStateCommand(onAuxStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(AuxCommandTopic), F("ON"));

    assertAuxStateCallbackCalled(true, &hvac)
}

AHA_TEST(HVACTest, aux_state_command_off) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::AuxHeatingFeature);
    hvac.onAuxStateCommand(onAuxStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(AuxCommandTopic), F("OFF"));

    assertAuxStateCallbackCalled(false, &hvac)
}

AHA_TEST(HVACTest, aux_command_different_fan) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.onAuxStateCommand(onAuxStateCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueHVACDifferent/act"),
        F("ON")
    );

    assertAuxStateCallbackNotCalled()
}

AHA_TEST(HVACTest, power_command_on) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::PowerFeature);
    hvac.onPowerCommand(onPowerCommandReceived);
    mock->fakeMessage(AHATOFSTR(PowerCommandTopic), F("ON"));

    assertPowerCallbackCalled(true, &hvac)
}

AHA_TEST(HVACTest, power_command_off) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::PowerFeature);
    hvac.onPowerCommand(onPowerCommandReceived);
    mock->fakeMessage(AHATOFSTR(PowerCommandTopic), F("OFF"));

    assertPowerCallbackCalled(false, &hvac)
}

AHA_TEST(HVACTest, power_command_different) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::PowerFeature);
    hvac.onPowerCommand(onPowerCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueHVACDifferent/pow_cmd_t"),
        F("ON")
    );

    assertPowerCallbackNotCalled()
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
