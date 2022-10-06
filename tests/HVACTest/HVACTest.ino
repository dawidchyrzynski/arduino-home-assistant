#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastAuxStateCallbackCall.reset(); \
    lastPowerCallbackCall.reset(); \
    lastFanModeCallbackCall.reset();

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

#define assertFanModeCallbackCalled(expectedMode, callerPtr) \
    assertTrue(lastFanModeCallbackCall.called); \
    assertEqual(expectedMode, lastFanModeCallbackCall.mode); \
    assertEqual(callerPtr, lastFanModeCallbackCall.caller);

#define assertFanModeCallbackNotCalled() \
    assertFalse(lastFanModeCallbackCall.called);

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

struct FanModeCallback {
    bool called = false;
    HAHVAC::FanMode mode = HAHVAC::UnknownFanMode;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        mode = HAHVAC::UnknownFanMode;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueHVAC";
static AuxStateCallback lastAuxStateCallbackCall;
static PowerCallback lastPowerCallbackCall;
static FanModeCallback lastFanModeCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/climate/testDevice/uniqueHVAC/config"};
const char CurrentTemperatureTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ctt"};
const char ActionTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/at"};
const char AuxStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ast"};
const char AuxCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/act"};
const char PowerCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/pow_cmd_t"};
const char FanModeStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/fan_mode_stat_t"};
const char FanModeCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/fan_mode_cmd_t"};

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

void onFanModeCommandReceived(HAHVAC::FanMode mode, HAHVAC* caller)
{
    lastFanModeCallbackCall.called = true;
    lastFanModeCallbackCall.mode = mode;
    lastFanModeCallbackCall.caller = caller;
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

AHA_TEST(HVACTest, config_with_action) {
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

AHA_TEST(HVACTest, config_with_aux) {
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

AHA_TEST(HVACTest, config_with_power) {
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

AHA_TEST(HVACTest, config_with_fan) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
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

AHA_TEST(HVACTest, fan_mode_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(FanModeCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(HVACTest, availability) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAHVAC
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

AHA_TEST(HVACTest, fan_modes_setter_auto_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setFanModes(HAHVAC::AutoFanMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
            "\"fan_modes\":[\"auto\"],"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, fan_modes_setter_low_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setFanModes(HAHVAC::LowFanMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
            "\"fan_modes\":[\"low\"],"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, fan_modes_setter_medium_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setFanModes(HAHVAC::MediumFanMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
            "\"fan_modes\":[\"medium\"],"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, fan_modes_setter_high_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setFanModes(HAHVAC::HighFanMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
            "\"fan_modes\":[\"high\"],"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, fan_modes_setter_mixed) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setFanModes(HAHVAC::AutoFanMode | HAHVAC::MediumFanMode | HAHVAC::HighFanMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"fan_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/fan_mode_cmd_t\","
            "\"fan_mode_stat_t\":\"testData/testDevice/uniqueHVAC/fan_mode_stat_t\","
            "\"fan_modes\":[\"auto\",\"medium\",\"high\"],"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, publish_nothing_if_retained) {
    prepareTest

    HAHVAC hvac(
        testUniqueId,
        HAHVAC::ActionFeature |
            HAHVAC::AuxHeatingFeature |
            HAHVAC::PowerFeature |
            HAHVAC::FanFeature
    );
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

AHA_TEST(HVACTest, publish_fan_mode_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setCurrentFanMode(HAHVAC::HighFanMode);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(FanModeStateTopic), "high", true)
}

AHA_TEST(HVACTest, publish_fan_mode_auto) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);

    assertTrue(hvac.setFanMode(HAHVAC::AutoFanMode));
    assertSingleMqttMessage(AHATOFSTR(FanModeStateTopic), "auto", true) 
}

AHA_TEST(HVACTest, publish_fan_mode_low) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);

    assertTrue(hvac.setFanMode(HAHVAC::LowFanMode));
    assertSingleMqttMessage(AHATOFSTR(FanModeStateTopic), "low", true) 
}

AHA_TEST(HVACTest, publish_fan_mode_medium) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);

    assertTrue(hvac.setFanMode(HAHVAC::MediumFanMode));
    assertSingleMqttMessage(AHATOFSTR(FanModeStateTopic), "medium", true) 
}

AHA_TEST(HVACTest, publish_fan_mode_high) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);

    assertTrue(hvac.setFanMode(HAHVAC::HighFanMode));
    assertSingleMqttMessage(AHATOFSTR(FanModeStateTopic), "high", true) 
}

AHA_TEST(HVACTest, publish_fan_mode_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setCurrentFanMode(HAHVAC::HighFanMode);
        
    assertTrue(hvac.setFanMode(HAHVAC::HighFanMode));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_fan_mode_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.setCurrentFanMode(HAHVAC::HighFanMode);

    assertTrue(hvac.setFanMode(HAHVAC::HighFanMode, true));
    assertSingleMqttMessage(AHATOFSTR(FanModeStateTopic), "high", true) 
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

AHA_TEST(HVACTest, fan_mode_command_auto) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(FanModeCommandTopic), F("auto"));

    assertFanModeCallbackCalled(HAHVAC::AutoFanMode, &hvac)
}

AHA_TEST(HVACTest, fan_mode_command_low) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(FanModeCommandTopic), F("low"));

    assertFanModeCallbackCalled(HAHVAC::LowFanMode, &hvac)
}

AHA_TEST(HVACTest, fan_mode_command_medium) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(FanModeCommandTopic), F("medium"));

    assertFanModeCallbackCalled(HAHVAC::MediumFanMode, &hvac)
}

AHA_TEST(HVACTest, fan_mode_command_high) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(FanModeCommandTopic), F("high"));

    assertFanModeCallbackCalled(HAHVAC::HighFanMode, &hvac)
}

AHA_TEST(HVACTest, fan_mode_command_invalid) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(FanModeCommandTopic), F("INVALID"));

    assertFanModeCallbackNotCalled()
}

AHA_TEST(HVACTest, fan_mode_command_different) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::FanFeature);
    hvac.onFanModeCommand(onFanModeCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueHVACDifferent/fan_mode_cmd_t"),
        F("auto")
    );

    assertFanModeCallbackNotCalled()
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
