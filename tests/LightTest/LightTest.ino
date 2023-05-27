#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastStateCallbackCall.reset(); \
    lastBrightnessCallbackCall.reset(); \
    lastColorTempCallbackCall.reset(); \
    lastRGBColorCallbackCall.reset(); \
    lastEffectCallbackCall.reset();

#define assertStateCallbackCalled(expectedState, callerPtr) \
    assertTrue(lastStateCallbackCall.called); \
    assertEqual(static_cast<bool>(expectedState), lastStateCallbackCall.state); \
    assertEqual(callerPtr, lastStateCallbackCall.caller);

#define assertStateCallbackNotCalled() \
    assertFalse(lastStateCallbackCall.called);

#define assertBrightnessCallbackCalled(expectedBrightness, callerPtr) \
    assertTrue(lastBrightnessCallbackCall.called); \
    assertEqual((uint8_t)expectedBrightness, lastBrightnessCallbackCall.brightness); \
    assertEqual(callerPtr, lastBrightnessCallbackCall.caller);

#define assertBrightnessCallbackNotCalled() \
    assertFalse(lastBrightnessCallbackCall.called);

#define assertColorTempCallbackCalled(expectedColorTemp, callerPtr) \
    assertTrue(lastColorTempCallbackCall.called); \
    assertEqual((uint16_t)expectedColorTemp, lastColorTempCallbackCall.temperature); \
    assertEqual(callerPtr, lastColorTempCallbackCall.caller);

#define assertColorTempCallbackNotCalled() \
    assertFalse(lastColorTempCallbackCall.called);

#define assertRGBColorCallbackCalled(expectedColor, callerPtr) \
    assertTrue(lastRGBColorCallbackCall.called); \
    assertTrue(expectedColor == lastRGBColorCallbackCall.color); \
    assertTrue(lastRGBColorCallbackCall.color.isSet); \
    assertEqual(callerPtr, lastRGBColorCallbackCall.caller);

#define assertRGBColorCallbackNotCalled() \
    assertFalse(lastRGBColorCallbackCall.called);

#define assertEffectCallbackCalled(expectedIndex, callerPtr) \
    assertTrue(lastEffectCallbackCall.called); \
    assertEqual(expectedIndex, lastEffectCallbackCall.index); \
    assertEqual(callerPtr, lastEffectCallbackCall.caller);

#define assertEffectCallbackNotCalled() \
    assertFalse(lastEffectCallbackCall.called);

using aunit::TestRunner;

struct StateCallback {
    bool called = false;
    bool state = false;
    HALight* caller = nullptr;

    void reset() {
        called = false;
        state = false;
        caller = nullptr;
    }
};

struct BrightnessCallback {
    bool called = false;
    uint8_t brightness = 0;
    HALight* caller = nullptr;

    void reset() {
        called = false;
        brightness = 0;
        caller = nullptr;
    }
};

struct ColorTemperatureCallback {
    bool called = false;
    uint16_t temperature = 0;
    HALight* caller = nullptr;

    void reset() {
        called = false;
        temperature = 0;
        caller = nullptr;
    }
};

struct RGBCommandCallback {
    bool called = false;
    HALight::RGBColor color = HALight::RGBColor();
    HALight* caller = nullptr;

    void reset() {
        called = false;
        color = HALight::RGBColor();
        caller = nullptr;
    }
};

struct EffectCallback {
    bool called = false;
    uint8_t index = 0;
    HALight* caller = nullptr;

    void reset() {
        called = false;
        index = 0;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueLight";
static StateCallback lastStateCallbackCall;
static BrightnessCallback lastBrightnessCallbackCall;
static ColorTemperatureCallback lastColorTempCallbackCall;
static RGBCommandCallback lastRGBColorCallbackCall;
static EffectCallback lastEffectCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/light/testDevice/uniqueLight/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/stat_t"};
const char BrightnessStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/bri_stat_t"};
const char ColorTemperatureStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/clr_temp_stat_t"};
const char StateCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/cmd_t"};
const char BrightnessCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/bri_cmd_t"};
const char ColorTemperatureCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/clr_temp_cmd_t"};
const char RGBCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/rgb_cmd_t"};
const char RGBStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/rgb_stat_t"};
const char EffectCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/fx_cmd_t"};
const char EffectStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/fx_stat_t"};

void onStateCommandReceived(bool state, HALight* caller)
{
    lastStateCallbackCall.called = true;
    lastStateCallbackCall.state = state;
    lastStateCallbackCall.caller = caller;
}

void onBrightnessCommandReceived(uint8_t brightness, HALight* caller)
{
    lastBrightnessCallbackCall.called = true;
    lastBrightnessCallbackCall.brightness = brightness;
    lastBrightnessCallbackCall.caller = caller;
}

void onColorTemperatureCommandReceived(uint16_t temperature, HALight* caller)
{
    lastColorTempCallbackCall.called = true;
    lastColorTempCallbackCall.temperature = temperature;
    lastColorTempCallbackCall.caller = caller;
}

void onRGBColorCommand(HALight::RGBColor color, HALight* caller)
{
    lastRGBColorCallbackCall.called = true;
    lastRGBColorCallbackCall.color = color;
    lastRGBColorCallbackCall.caller = caller;
}

void onEffectCommandReceived(uint8_t index, HALight* caller)
{
    lastEffectCallbackCall.called = true;
    lastEffectCallbackCall.index = index;
    lastEffectCallbackCall.caller = caller;
}

AHA_TEST(LightTest, invalid_unique_id) {
    prepareTest

    HALight light(nullptr);
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(LightTest, default_params) {
    prepareTest

    HALight light(testUniqueId);
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(LightTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HALight light(testUniqueId);
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(LightTest, default_params_with_brightness) {
    prepareTest

    HALight light(testUniqueId, HALight::BrightnessFeature);
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"bri_stat_t\":\"testData/testDevice/uniqueLight/bri_stat_t\","
            "\"bri_cmd_t\":\"testData/testDevice/uniqueLight/bri_cmd_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
    assertEqual(3, mock->getFlushedMessagesNb()); // config + default state + default brightness
}

AHA_TEST(LightTest, default_params_with_color_temp) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"clr_temp_stat_t\":\"testData/testDevice/uniqueLight/clr_temp_stat_t\","
            "\"clr_temp_cmd_t\":\"testData/testDevice/uniqueLight/clr_temp_cmd_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )

    // config + default state + default color temp
    assertEqual(3, mock->getFlushedMessagesNb());
}

AHA_TEST(LightTest, default_params_with_rgb) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"rgb_cmd_t\":\"testData/testDevice/uniqueLight/rgb_cmd_t\","
            "\"rgb_stat_t\":\"testData/testDevice/uniqueLight/rgb_stat_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(LightTest, default_params_with_brightness_and_color_temp) {
    prepareTest

    HALight light(
        testUniqueId,
        HALight::BrightnessFeature | HALight::ColorTemperatureFeature
    );
    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"bri_stat_t\":\"testData/testDevice/uniqueLight/bri_stat_t\","
            "\"bri_cmd_t\":\"testData/testDevice/uniqueLight/bri_cmd_t\","
            "\"clr_temp_stat_t\":\"testData/testDevice/uniqueLight/clr_temp_stat_t\","
            "\"clr_temp_cmd_t\":\"testData/testDevice/uniqueLight/clr_temp_cmd_t\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )

    // config + default state + default brightness + default color temp
    assertEqual(4, mock->getFlushedMessagesNb());
}

AHA_TEST(LightTest, default_params_with_no_effects_set) {
    prepareTest

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(light.getEffects() == nullptr);
}

AHA_TEST(LightTest, default_params_with_invalid_effects_disabled_feature) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    HALight light(testUniqueId);
    light.setEffects(effects, 2);
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer != nullptr);
    assertTrue(light.getEffects() == nullptr);
}

AHA_TEST(LightTest, default_params_with_invalid_effects_nullptr) {
    prepareTest

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(nullptr, 1);
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(light.getEffects() == nullptr);
}

AHA_TEST(LightTest, default_params_with_invalid_effects_zero_size) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 0);
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(light.getEffects() == nullptr);
}

AHA_TEST(LightTest, default_params_with_effects_single_init) {
    prepareTest

    const char* const effects[] = { "Effect 1" };
    const char* const notAbleToSetEffects[] = { "Effect 2" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 1);
    light.setEffects(notAbleToSetEffects, 1); // effects already initialized at previous line
    light.buildSerializerTest();
    HASerializer* serializer = light.getSerializer();

    assertTrue(serializer != nullptr);

    HASerializerArray* lightEffects = light.getEffects();

    assertTrue(lightEffects != nullptr);
    assertEqual(1, lightEffects->getItemsNb());
    assertEqual(0, strcmp(effects[0], lightEffects->getItems()[0]));
}

AHA_TEST(LightTest, default_params_with_effects) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 2);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"fx_stat_t\":\"testData/testDevice/uniqueLight/fx_stat_t\","
            "\"fx_cmd_t\":\"testData/testDevice/uniqueLight/fx_cmd_t\","
            "\"fx_list\":[\"Effect 1\",\"Effect 2\"],"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )

    // config + default state + default effect
    assertEqual(3, mock->getFlushedMessagesNb());
}

AHA_TEST(LightTest, state_command_subscription) {
    prepareTest

    HALight light(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(StateCommandTopic),
        mock->getSubscriptions()[0]->topic
    );
}

AHA_TEST(LightTest, brightness_command_subscription) {
    prepareTest

    HALight light(testUniqueId, HALight::BrightnessFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(BrightnessCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(LightTest, color_temperature_command_subscription) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(ColorTemperatureCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(LightTest, rgb_command_subscription) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(RGBCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(LightTest, effect_command_subscription) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 2);
    mqtt.loop();

    assertEqual(2, mock->getSubscriptionsNb());
    assertEqual(
        AHATOFSTR(EffectCommandTopic),
        mock->getSubscriptions()[1]->topic
    );
}

AHA_TEST(LightTest, availability) {
    prepareTest

    HALight light(testUniqueId);
    light.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HALight
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueLight/avty_t"),
        "online",
        true
    )
}

AHA_TEST(LightTest, publish_last_known_state) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentState(true);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(LightTest, publish_last_known_brightness) {
    prepareTest

    HALight light(testUniqueId, HALight::BrightnessFeature);
    light.setCurrentBrightness(50);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(2, AHATOFSTR(BrightnessStateTopic), "50", true)
}

AHA_TEST(LightTest, publish_last_known_color_temperature) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setCurrentColorTemperature(200);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(2, AHATOFSTR(ColorTemperatureStateTopic), "200", true)
}

AHA_TEST(LightTest, publish_last_known_rgb_color) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.setCurrentRGBColor(HALight::RGBColor(255,123,15));
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(2, AHATOFSTR(RGBStateTopic), "255,123,15", true)
}

AHA_TEST(LightTest, publish_last_known_effect) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 2);
    light.setCurrentEffect(1);
    mqtt.loop();

    assertEqual(3, mock->getFlushedMessagesNb());
    assertMqttMessage(2, AHATOFSTR(EffectStateTopic), "Effect 2", true)
}

AHA_TEST(LightTest, publish_nothing_if_retained) {
    prepareTest

    HALight light(testUniqueId, HALight::BrightnessFeature);
    light.setRetain(true);
    light.setCurrentState(true);
    light.setCurrentBrightness(50);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(LightTest, name_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setName("testName");

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, object_id_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setObjectId("testId");

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, icon_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setIcon("testIcon");

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, retain_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setRetain(true);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, optimistic_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setOptimistic(true);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, brightness_scale_setter) {
    prepareTest

    HALight light(testUniqueId, HALight::BrightnessFeature);
    light.setBrightnessScale(10);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"bri_stat_t\":\"testData/testDevice/uniqueLight/bri_stat_t\","
            "\"bri_cmd_t\":\"testData/testDevice/uniqueLight/bri_cmd_t\","
            "\"bri_scl\":10,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, brightness_scale_feature_disabled) {
    prepareTest

    HALight light(testUniqueId);
    light.setBrightnessScale(10);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, color_temperature_range_setter) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setMinMireds(50);
    light.setMaxMireds(600);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"clr_temp_stat_t\":\"testData/testDevice/uniqueLight/clr_temp_stat_t\","
            "\"clr_temp_cmd_t\":\"testData/testDevice/uniqueLight/clr_temp_cmd_t\","
            "\"min_mirs\":50,"
            "\"max_mirs\":600,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, color_temperature_range_setter_feature_disabled) {
    prepareTest

    HALight light(testUniqueId);
    light.setMinMireds(50);
    light.setMaxMireds(600);

    assertEntityConfig(
        mock,
        light,
        (
            "{"
            "\"uniq_id\":\"uniqueLight\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLight/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLight/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LightTest, current_state_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentState(true);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertTrue(light.getCurrentState());
}

AHA_TEST(LightTest, current_brightness_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentBrightness(50);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual((uint8_t)50, light.getCurrentBrightness());
}

AHA_TEST(LightTest, current_color_temperature_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentColorTemperature(50);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual((uint16_t)50, light.getCurrentColorTemperature());
}

AHA_TEST(LightTest, current_rgb_color_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentRGBColor(HALight::RGBColor(255,123,111));

    assertEqual(0, mock->getFlushedMessagesNb());
    assertTrue(HALight::RGBColor(255,123,111) == light.getCurrentRGBColor());
}

AHA_TEST(LightTest, current_effect_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentEffect(3);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual((uint8_t)3, light.getCurrentEffect());
}

AHA_TEST(LightTest, publish_state) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);

    assertTrue(light.setState(true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(LightTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);
    light.setCurrentState(true);

    // it shouldn't publish data if state doesn't change
    assertTrue(light.setState(true));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);
    light.setCurrentState(true);

    assertTrue(light.setState(true, true));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "ON", true)
}

AHA_TEST(LightTest, publish_nothing_if_brightness_feature_is_disabled) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);

    assertFalse(light.setBrightness(50));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_brightness) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::BrightnessFeature);

    assertTrue(light.setBrightness(50));
    assertSingleMqttMessage(AHATOFSTR(BrightnessStateTopic), "50", true)
}

AHA_TEST(LightTest, publish_brightness_debounce) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::BrightnessFeature);
    light.setCurrentBrightness(50);

    // it shouldn't publish data if value doesn't change
    assertTrue(light.setBrightness(50));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_brightness_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::BrightnessFeature);
    light.setCurrentBrightness(50);

    assertTrue(light.setBrightness(50, true));
    assertSingleMqttMessage(AHATOFSTR(BrightnessStateTopic), "50", true)
}

AHA_TEST(LightTest, publish_nothing_if_color_temperature_feature_is_disabled) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);

    assertFalse(light.setColorTemperature(200));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_color_temperature) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::ColorTemperatureFeature);

    assertTrue(light.setColorTemperature(200));
    assertSingleMqttMessage(AHATOFSTR(ColorTemperatureStateTopic), "200", true)
}

AHA_TEST(LightTest, publish_color_temperature_debounce) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setCurrentColorTemperature(200);

    // it shouldn't publish data if value doesn't change
    assertTrue(light.setColorTemperature(200));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_color_temperature_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setCurrentColorTemperature(200);

    assertTrue(light.setColorTemperature(200, true));
    assertSingleMqttMessage(AHATOFSTR(ColorTemperatureStateTopic), "200", true)
}

AHA_TEST(LightTest, publish_rgb_color) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::RGBFeature);

    assertTrue(light.setRGBColor(HALight::RGBColor(255,123,111)));
    assertSingleMqttMessage(AHATOFSTR(RGBStateTopic), "255,123,111", true)
}

AHA_TEST(LightTest, publish_rgb_color_debounce) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::RGBFeature);
    light.setCurrentRGBColor(HALight::RGBColor(255,123,111));

    // it shouldn't publish data if value doesn't change
    assertTrue(light.setRGBColor(HALight::RGBColor(255,123,111)));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_rgb_color_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId, HALight::RGBFeature);
    light.setCurrentRGBColor(HALight::RGBColor(255,123,111));

    assertTrue(light.setRGBColor(HALight::RGBColor(255,123,111), true));
    assertSingleMqttMessage(AHATOFSTR(RGBStateTopic), "255,123,111", true)
}

AHA_TEST(LightTest, publish_nothing_if_effects_feature_is_disabled) {
    prepareTest

    mock->connectDummy();
    HALight light(testUniqueId);

    assertFalse(light.setEffect(5));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_effect_first) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2", "Effect 3" };

    mock->connectDummy();

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);

    assertTrue(light.setEffect(0));
    // First effect is a default one, shouldn't publish
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_effect_last) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2", "Effect 3" };

    mock->connectDummy();

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);

    assertTrue(light.setEffect(2));
    assertSingleMqttMessage(AHATOFSTR(EffectStateTopic), "Effect 3", true)
}

AHA_TEST(LightTest, publish_effect_only) {
    prepareTest

    const char* const effects[] = { "Effect 1" };

    mock->connectDummy();

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 1);

    assertTrue(light.setEffect(0));
    // First effect is a default one, shouldn't publish
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_effect_debounce) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    mock->connectDummy();
    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 2);
    light.setCurrentEffect(1);

    // it shouldn't publish data if value doesn't change
    assertTrue(light.setEffect(1));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(LightTest, publish_effect_debounce_skip) {
    prepareTest

    const char* const effects[] = { "Effect 1", "Effect 2" };

    mock->connectDummy();
    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 2);
    light.setCurrentEffect(1);

    assertTrue(light.setEffect(1, true));
    assertSingleMqttMessage(AHATOFSTR(EffectStateTopic), "Effect 2", true)
}

AHA_TEST(LightTest, state_command_on) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("ON"));

    assertStateCallbackCalled(true, &light)
}

AHA_TEST(LightTest, state_command_off) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("OFF"));

    assertStateCallbackCalled(false, &light)
}

AHA_TEST(LightTest, state_command_different_light) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/cmd_t"),
        F("ON")
    );

    assertStateCallbackNotCalled()
}

AHA_TEST(LightTest, brightness_command_min) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("0"));

    assertBrightnessCallbackCalled(0, &light)
}

AHA_TEST(LightTest, brightness_command_max) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("255"));

    assertBrightnessCallbackCalled(255, &light)
}

AHA_TEST(LightTest, brightness_command_overflow) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("300"));

    assertBrightnessCallbackNotCalled()
}

AHA_TEST(LightTest, brightness_command_invalid) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("INVALID"));

    assertBrightnessCallbackNotCalled()
}

AHA_TEST(LightTest, brightness_command_different_light) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/pct_cmd_t"),
        F("50")
    );

    assertBrightnessCallbackNotCalled()
}

AHA_TEST(LightTest, color_temperature_command) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("200"));

    assertColorTempCallbackCalled(200, &light)
}

AHA_TEST(LightTest, color_temperature_command_min) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("153"));

    assertColorTempCallbackCalled(153, &light)
}

AHA_TEST(LightTest, color_temperature_command_max) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("500"));

    assertColorTempCallbackCalled(500, &light)
}

AHA_TEST(LightTest, color_temperature_command_invalid) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("INVALID"));

    assertColorTempCallbackNotCalled()
}

AHA_TEST(LightTest, color_temperature_command_different_light) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/clr_temp_cmd_t"),
        F("180")
    );

    assertColorTempCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_min_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("0,0,0"));

    assertRGBColorCallbackCalled(HALight::RGBColor(0,0,0), &light)
}

AHA_TEST(LightTest, rgb_color_small_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("1,2,3"));

    assertRGBColorCallbackCalled(HALight::RGBColor(1,2,3), &light)
}

AHA_TEST(LightTest, rgb_color_max_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("255,255,255"));

    assertRGBColorCallbackCalled(HALight::RGBColor(255,255,255), &light)
}

AHA_TEST(LightTest, rgb_color_mix_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("255,12,1"));

    assertRGBColorCallbackCalled(HALight::RGBColor(255,12,1), &light)
}

AHA_TEST(LightTest, rgb_color_invalid_1_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("255,12"));

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_invalid_2_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F(",,"));

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_invalid_3_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), "");

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_invalid_4_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("255,,"));

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_invalid_5_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("thisIsNotTheColor"));

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_invalid_6_command) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(AHATOFSTR(RGBCommandTopic), F("256,123,2"));

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, rgb_color_command_different_light) {
    prepareTest

    HALight light(testUniqueId, HALight::RGBFeature);
    light.onRGBColorCommand(onRGBColorCommand);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/rgb_cmd_t"),
        F("255,12,1")
    );

    assertRGBColorCallbackNotCalled()
}

AHA_TEST(LightTest, effect_command) {
    prepareTest

    const char* const effects[] = { "Fire", "Rainbow", "Snowflakes" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);
    light.onEffectCommand(onEffectCommandReceived);
    mock->fakeMessage(AHATOFSTR(EffectCommandTopic), F("Rainbow"));

    assertEffectCallbackCalled(1, &light)
}

AHA_TEST(LightTest, effect_command_first) {
    prepareTest

    const char* const effects[] = { "Fire", "Rainbow", "Snowflakes" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);
    light.onEffectCommand(onEffectCommandReceived);
    mock->fakeMessage(AHATOFSTR(EffectCommandTopic), F("Fire"));

    assertEffectCallbackCalled(0, &light)
}

AHA_TEST(LightTest, effect_command_last) {
    prepareTest

    const char* const effects[] = { "Fire", "Rainbow", "Snowflakes" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);
    light.onEffectCommand(onEffectCommandReceived);
    mock->fakeMessage(AHATOFSTR(EffectCommandTopic), F("Snowflakes"));

    assertEffectCallbackCalled(2, &light)
}

AHA_TEST(LightTest, effect_command_non_existing) {
    prepareTest

    const char* const effects[] = { "Fire", "Rainbow", "Snowflakes" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);
    light.onEffectCommand(onEffectCommandReceived);
    mock->fakeMessage(AHATOFSTR(EffectCommandTopic), F("NON_EXISTING"));

    assertEffectCallbackNotCalled()
}

AHA_TEST(LightTest, effect_command_different_light) {
    prepareTest

    const char* const effects[] = { "Fire", "Rainbow", "Snowflakes" };

    HALight light(testUniqueId, HALight::EffectsFeature);
    light.setEffects(effects, 3);
    light.onEffectCommand(onEffectCommandReceived);
    mock->fakeMessage(
            F("testData/testDevice/uniqueLightDifferent/fx_cmd_t"),
            F("Fire")
    );

    assertEffectCallbackNotCalled()
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
