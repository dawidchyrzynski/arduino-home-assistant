#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastAuxStateCallbackCall.reset(); \
    lastPowerCallbackCall.reset(); \
    lastFanModeCallbackCall.reset(); \
    lastSwingModeCallbackCall.reset(); \
    lastModeCallbackCall.reset(); \
    lastTargetTempCallbackCall.reset();

#define assertAuxStateCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastAuxStateCallbackCall.called); \
    assertEqual(static_cast<bool>(expectedState), lastAuxStateCallbackCall.state); \
    assertEqual(callerPtr, lastAuxStateCallbackCall.caller);

#define assertAuxStateCallbackNotCalled() \
    assertFalse(lastAuxStateCallbackCall.called);

#define assertPowerCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastPowerCallbackCall.called); \
    assertEqual(static_cast<bool>(expectedState), lastPowerCallbackCall.state); \
    assertEqual(callerPtr, lastPowerCallbackCall.caller);

#define assertPowerCallbackNotCalled() \
    assertFalse(lastPowerCallbackCall.called);

#define assertFanModeCallbackCalled(expectedMode, callerPtr) \
    assertTrue(lastFanModeCallbackCall.called); \
    assertEqual(expectedMode, lastFanModeCallbackCall.mode); \
    assertEqual(callerPtr, lastFanModeCallbackCall.caller);

#define assertFanModeCallbackNotCalled() \
    assertFalse(lastFanModeCallbackCall.called);

#define assertSwingModeCallbackCalled(expectedMode, callerPtr) \
    assertTrue(lastSwingModeCallbackCall.called); \
    assertEqual(expectedMode, lastSwingModeCallbackCall.mode); \
    assertEqual(callerPtr, lastSwingModeCallbackCall.caller);

#define assertSwingModeCallbackNotCalled() \
    assertFalse(lastSwingModeCallbackCall.called);

#define assertModeCallbackCalled(expectedMode, callerPtr) \
    assertTrue(lastModeCallbackCall.called); \
    assertEqual(expectedMode, lastModeCallbackCall.mode); \
    assertEqual(callerPtr, lastModeCallbackCall.caller);

#define assertModeCallbackNotCalled() \
    assertFalse(lastModeCallbackCall.called);

#define assertTargetTempCallbackCalled(expectedTemperature, callerPtr) \
    assertTrue(lastTargetTempCallbackCall.called); \
    assertTrue(expectedTemperature == lastTargetTempCallbackCall.temperature); \
    assertEqual(callerPtr, lastTargetTempCallbackCall.caller);

#define assertTargetTempCallbackNotCalled() \
    assertFalse(lastTargetTempCallbackCall.called);

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

struct SwingModeCallback {
    bool called = false;
    HAHVAC::SwingMode mode = HAHVAC::UnknownSwingMode;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        mode = HAHVAC::UnknownSwingMode;
        caller = nullptr;
    }
};

struct ModeCallback {
    bool called = false;
    HAHVAC::Mode mode = HAHVAC::UnknownMode;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        mode = HAHVAC::UnknownMode;
        caller = nullptr;
    }
};

struct TargetTempCallback {
    bool called = false;
    HANumeric temperature;
    HAHVAC* caller = nullptr;

    void reset() {
        called = false;
        temperature.reset();
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueHVAC";
static AuxStateCallback lastAuxStateCallbackCall;
static PowerCallback lastPowerCallbackCall;
static FanModeCallback lastFanModeCallbackCall;
static SwingModeCallback lastSwingModeCallbackCall;
static ModeCallback lastModeCallbackCall;
static TargetTempCallback lastTargetTempCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/climate/testDevice/uniqueHVAC/config"};
const char CurrentTemperatureTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/curr_temp_t"};
const char ActionTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/act_t"};
const char AuxStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/aux_stat_t"};
const char AuxCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/aux_cmd_t"};
const char PowerCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/pow_cmd_t"};
const char FanModeStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/fan_mode_stat_t"};
const char FanModeCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/fan_mode_cmd_t"};
const char SwingModeStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/swing_mode_stat_t"};
const char SwingModeCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/swing_mode_cmd_t"};
const char ModeStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/mode_stat_t"};
const char ModeCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/mode_cmd_t"};
const char TemperatureStateTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/temp_stat_t"};
const char TemperatureCommandTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/temp_cmd_t"};

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

void onSwingModeCommandReceived(HAHVAC::SwingMode mode, HAHVAC* caller)
{
    lastSwingModeCallbackCall.called = true;
    lastSwingModeCallbackCall.mode = mode;
    lastSwingModeCallbackCall.caller = caller;
}

void onModeCommandReceived(HAHVAC::Mode mode, HAHVAC* caller)
{
    lastModeCallbackCall.called = true;
    lastModeCallbackCall.mode = mode;
    lastModeCallbackCall.caller = caller;
}

void onTargetTemperatureCommandReceived(HANumeric temperature, HAHVAC* caller)
{
    lastTargetTempCallbackCall.called = true;
    lastTargetTempCallbackCall.temperature = temperature;
    lastTargetTempCallbackCall.caller = caller;
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HAHVAC hvac(testUniqueId);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueHVAC\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"act_t\":\"testData/testDevice/uniqueHVAC/act_t\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"aux_cmd_t\":\"testData/testDevice/uniqueHVAC/aux_cmd_t\","
            "\"aux_stat_t\":\"testData/testDevice/uniqueHVAC/aux_stat_t\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, config_with_swing) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"swing_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/swing_mode_cmd_t\","
            "\"swing_mode_stat_t\":\"testData/testDevice/uniqueHVAC/swing_mode_stat_t\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, config_with_modes) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, config_with_target_temperature_p1) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_cmd_t\":\"testData/testDevice/uniqueHVAC/temp_cmd_t\","
            "\"temp_stat_t\":\"testData/testDevice/uniqueHVAC/temp_stat_t\","
            "\"temp_cmd_tpl\":\"{{int(float(value)*10**1)}}\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, config_with_target_temperature_p2) {
    prepareTest

    HAHVAC hvac(
        testUniqueId,
        HAHVAC::TargetTemperatureFeature,
        HAHVAC::PrecisionP2
    );
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_cmd_t\":\"testData/testDevice/uniqueHVAC/temp_cmd_t\","
            "\"temp_stat_t\":\"testData/testDevice/uniqueHVAC/temp_stat_t\","
            "\"temp_cmd_tpl\":\"{{int(float(value)*10**2)}}\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, config_with_target_temperature_p3) {
    prepareTest

    HAHVAC hvac(
        testUniqueId,
        HAHVAC::TargetTemperatureFeature,
        HAHVAC::PrecisionP3
    );
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"temp_cmd_t\":\"testData/testDevice/uniqueHVAC/temp_cmd_t\","
            "\"temp_stat_t\":\"testData/testDevice/uniqueHVAC/temp_stat_t\","
            "\"temp_cmd_tpl\":\"{{int(float(value)*10**3)}}\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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

AHA_TEST(HVACTest, swing_mode_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(SwingModeCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(HVACTest, mode_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(ModeCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(HVACTest, target_temperature_command_subscription) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(TemperatureCommandTopic),
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, object_id_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setObjectId("testId");

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueHVAC\","
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, swing_modes_setter_on_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.setSwingModes(HAHVAC::OnSwingMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"swing_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/swing_mode_cmd_t\","
            "\"swing_mode_stat_t\":\"testData/testDevice/uniqueHVAC/swing_mode_stat_t\","
            "\"swing_modes\":[\"on\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, swing_modes_setter_off_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.setSwingModes(HAHVAC::OffSwingMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"swing_mode_cmd_t\":\"testData/testDevice/uniqueHVAC/swing_mode_cmd_t\","
            "\"swing_mode_stat_t\":\"testData/testDevice/uniqueHVAC/swing_mode_stat_t\","
            "\"swing_modes\":[\"off\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_auto_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::AutoMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"auto\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_off_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::OffMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"off\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_cool_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::CoolMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"cool\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_heat_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::HeatMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"heat\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_dry_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::DryMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"dry\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_fan_only_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::FanOnlyMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"fan_only\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, modes_setter_mixed) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setModes(HAHVAC::AutoMode | HAHVAC::HeatMode | HAHVAC::DryMode);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"mode_cmd_t\":\"testData/testDevice/uniqueHVAC/mode_cmd_t\","
            "\"mode_stat_t\":\"testData/testDevice/uniqueHVAC/mode_stat_t\","
            "\"modes\":[\"auto\",\"heat\",\"dry\"],"
            "\"curr_temp_t\":\"testData/testDevice/uniqueHVAC/curr_temp_t\","
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
            HAHVAC::FanFeature |
            HAHVAC::SwingFeature |
            HAHVAC::ModesFeature
    );
    hvac.setRetain(true);
    hvac.setCurrentTemperature(21.5f);
    hvac.setCurrentAction(HAHVAC::CoolingAction);
    hvac.setCurrentFanMode(HAHVAC::AutoFanMode);
    hvac.setCurrentSwingMode(HAHVAC::OnSwingMode);
    hvac.setCurrentMode(HAHVAC::HeatMode);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(HVACTest, publish_current_temperature_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5f);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_current_temperature) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);

    assertTrue(hvac.setCurrentTemperature(21.5f));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_current_temperature_p2) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP2);

    assertTrue(hvac.setCurrentTemperature(21.5f));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.50", true)
}

AHA_TEST(HVACTest, publish_current_temperature_p3) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::DefaultFeatures, HAHVAC::PrecisionP3);

    assertTrue(hvac.setCurrentTemperature(21.555f));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.555", true)
}

AHA_TEST(HVACTest, publish_current_temperature_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5f);

    // it shouldn't publish data if state doesn't change
    assertTrue(hvac.setCurrentTemperature(21.5f));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_current_temperature_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5f);

    assertTrue(hvac.setCurrentTemperature(21.5f, true));
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

AHA_TEST(HVACTest, publish_swing_mode_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.setCurrentSwingMode(HAHVAC::OnSwingMode);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(SwingModeStateTopic), "on", true)
}

AHA_TEST(HVACTest, publish_swing_mode_on) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);

    assertTrue(hvac.setSwingMode(HAHVAC::OnSwingMode));
    assertSingleMqttMessage(AHATOFSTR(SwingModeStateTopic), "on", true)
}

AHA_TEST(HVACTest, publish_swing_mode_off) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);

    assertTrue(hvac.setSwingMode(HAHVAC::OffSwingMode));
    assertSingleMqttMessage(AHATOFSTR(SwingModeStateTopic), "off", true)
}

AHA_TEST(HVACTest, publish_swing_mode_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.setCurrentSwingMode(HAHVAC::OnSwingMode);

    assertTrue(hvac.setSwingMode(HAHVAC::OnSwingMode));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_swing_mode_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.setCurrentSwingMode(HAHVAC::OnSwingMode);

    assertTrue(hvac.setSwingMode(HAHVAC::OnSwingMode, true));
    assertSingleMqttMessage(AHATOFSTR(SwingModeStateTopic), "on", true)
}

AHA_TEST(HVACTest, publish_mode_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.setCurrentMode(HAHVAC::HeatMode);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(ModeStateTopic), "heat", true)
}

AHA_TEST(HVACTest, publish_mode_auto) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::AutoMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "auto", true)
}

AHA_TEST(HVACTest, publish_mode_off) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::OffMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "off", true)
}

AHA_TEST(HVACTest, publish_mode_cool) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::CoolMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "cool", true)
}

AHA_TEST(HVACTest, publish_mode_heat) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::HeatMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "heat", true)
}

AHA_TEST(HVACTest, publish_mode_dry) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::DryMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "dry", true)
}

AHA_TEST(HVACTest, publish_mode_fan_only) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);

    assertTrue(hvac.setMode(HAHVAC::FanOnlyMode));
    assertSingleMqttMessage(AHATOFSTR(ModeStateTopic), "fan_only", true)
}

AHA_TEST(HVACTest, publish_target_temperature_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.setCurrentTargetTemperature(21.5f);
    mqtt.loop();

    assertMqttMessage(1, AHATOFSTR(TemperatureStateTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_target_temperature) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);

    assertTrue(hvac.setTargetTemperature(21.5f));
    assertSingleMqttMessage(AHATOFSTR(TemperatureStateTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_target_temperature_p2) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature, HAHVAC::PrecisionP2);

    assertTrue(hvac.setTargetTemperature(21.5f));
    assertSingleMqttMessage(AHATOFSTR(TemperatureStateTopic), "21.50", true)
}

AHA_TEST(HVACTest, publish_target_temperature_p3) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature, HAHVAC::PrecisionP3);

    assertTrue(hvac.setTargetTemperature(21.555f));
    assertSingleMqttMessage(AHATOFSTR(TemperatureStateTopic), "21.555", true)
}

AHA_TEST(HVACTest, publish_target_temperature_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.setCurrentTargetTemperature(21.5f);

    // it shouldn't publish data if state doesn't change
    assertTrue(hvac.setTargetTemperature(21.5f));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_target_temperature_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.setCurrentTargetTemperature(21.5f);

    assertTrue(hvac.setTargetTemperature(21.5f, true));
    assertSingleMqttMessage(AHATOFSTR(TemperatureStateTopic), "21.5", true)
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

AHA_TEST(HVACTest, swing_mode_command_on) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.onSwingModeCommand(onSwingModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(SwingModeCommandTopic), F("on"));

    assertSwingModeCallbackCalled(HAHVAC::OnSwingMode, &hvac)
}

AHA_TEST(HVACTest, swing_mode_command_off) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.onSwingModeCommand(onSwingModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(SwingModeCommandTopic), F("off"));

    assertSwingModeCallbackCalled(HAHVAC::OffSwingMode, &hvac)
}

AHA_TEST(HVACTest, swing_mode_command_invalid) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.onSwingModeCommand(onSwingModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(SwingModeCommandTopic), F("INVALID"));

    assertSwingModeCallbackNotCalled()
}

AHA_TEST(HVACTest, swing_mode_command_different) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::SwingFeature);
    hvac.onSwingModeCommand(onSwingModeCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueHVACDifferent/swing_mode_cmd_t"),
        F("on")
    );

    assertSwingModeCallbackNotCalled()
}

AHA_TEST(HVACTest, mode_command_auto) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("auto"));

    assertModeCallbackCalled(HAHVAC::AutoMode, &hvac)
}

AHA_TEST(HVACTest, mode_command_off) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("off"));

    assertModeCallbackCalled(HAHVAC::OffMode, &hvac)
}

AHA_TEST(HVACTest, mode_command_cool) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("cool"));

    assertModeCallbackCalled(HAHVAC::CoolMode, &hvac)
}

AHA_TEST(HVACTest, mode_command_heat) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("heat"));

    assertModeCallbackCalled(HAHVAC::HeatMode, &hvac)
}

AHA_TEST(HVACTest, mode_command_dry) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("dry"));

    assertModeCallbackCalled(HAHVAC::DryMode, &hvac)
}

AHA_TEST(HVACTest, mode_command_fan_only) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::ModesFeature);
    hvac.onModeCommand(onModeCommandReceived);
    mock->fakeMessage(AHATOFSTR(ModeCommandTopic), F("fan_only"));

    assertModeCallbackCalled(HAHVAC::FanOnlyMode, &hvac)
}

AHA_TEST(HVACTest, target_temperature_command_p1) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.onTargetTemperatureCommand(onTargetTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(TemperatureCommandTopic), F("215"));

    assertTargetTempCallbackCalled(HANumeric(21.5f, 1), &hvac)
}

AHA_TEST(HVACTest, target_temperature_command_p2) {
    prepareTest

    HAHVAC hvac(
        testUniqueId,
        HAHVAC::TargetTemperatureFeature,
        HAHVAC::PrecisionP2
    );
    hvac.onTargetTemperatureCommand(onTargetTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(TemperatureCommandTopic), F("215"));

    assertTargetTempCallbackCalled(HANumeric(2.15f, 2), &hvac)
}

AHA_TEST(HVACTest, target_temperature_command_p3) {
    prepareTest

    HAHVAC hvac(
        testUniqueId,
        HAHVAC::TargetTemperatureFeature,
        HAHVAC::PrecisionP3
    );
    hvac.onTargetTemperatureCommand(onTargetTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(TemperatureCommandTopic), F("215"));

    assertTargetTempCallbackCalled(HANumeric(0.215f, 3), &hvac)
}

AHA_TEST(HVACTest, target_temperature_command_invalid) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.onTargetTemperatureCommand(onTargetTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(TemperatureCommandTopic), F("21.5"));

    assertTargetTempCallbackNotCalled()
}

AHA_TEST(HVACTest, target_temperature_command_different) {
    prepareTest

    HAHVAC hvac(testUniqueId, HAHVAC::TargetTemperatureFeature);
    hvac.onTargetTemperatureCommand(onTargetTemperatureCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueHVACDifferent/temp_cmd_t"),
        F("215")
    );

    assertTargetTempCallbackNotCalled()
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
