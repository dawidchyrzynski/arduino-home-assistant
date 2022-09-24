#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackState = false; \
    commandCallbackBrightness = 0; \
    commandCallbackColorTemperature = 0; \
    commandCallbackLightPtr = nullptr;

#define assertStateCallback(shouldBeCalled, expectedState, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedState, commandCallbackState); \
    assertEqual(callerPtr, commandCallbackLightPtr);

#define assertBrightnessCallback(shouldBeCalled, expectedBrightness, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedBrightness, commandCallbackBrightness); \
    assertEqual(callerPtr, commandCallbackLightPtr);

#define assertColorTempCallback(shouldBeCalled, expectedColorTemp, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedColorTemp, commandCallbackColorTemperature); \
    assertEqual(callerPtr, commandCallbackLightPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueLight";

static bool commandCallbackCalled = false;
static bool commandCallbackState = false;
static uint8_t commandCallbackBrightness = 0;
static uint16_t commandCallbackColorTemperature = 0;
static HALight* commandCallbackLightPtr = nullptr;

const char ConfigTopic[] PROGMEM = {"homeassistant/light/testDevice/uniqueLight/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/stat_t"};
const char BrightnessStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/bri_stat_t"};
const char ColorTemperatureStateTopic[] PROGMEM = {"testData/testDevice/uniqueLight/clr_temp_stat_t"};
const char StateCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/cmd_t"};
const char BrightnessCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/bri_cmd_t"};
const char ColorTemperatureCommandTopic[] PROGMEM = {"testData/testDevice/uniqueLight/clr_temp_cmd_t"};

void onStateCommandReceived(bool state, HALight* light)
{
    commandCallbackCalled = true;
    commandCallbackState = state;
    commandCallbackLightPtr = light;
}

void onBrightnessCommandReceived(uint8_t brightness, HALight* light)
{
    commandCallbackCalled = true;
    commandCallbackBrightness = brightness;
    commandCallbackLightPtr = light;
}

void onColorTemperatureCommandReceived(uint16_t temperature, HALight* light)
{
    commandCallbackCalled = true;
    commandCallbackColorTemperature = temperature;
    commandCallbackLightPtr = light;
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
    assertEqual(3, mock->getFlushedMessagesNb()); // config + default state + default brightness
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
    assertEqual(true, light.getCurrentState());
}

AHA_TEST(LightTest, current_brightness_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentBrightness(50);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(50, light.getCurrentBrightness());
}

AHA_TEST(LightTest, current_color_temperature_setter) {
    prepareTest

    HALight light(testUniqueId);
    light.setCurrentColorTemperature(50);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(50, light.getCurrentColorTemperature());
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

AHA_TEST(LightTest, state_command_on) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("ON"));

    assertStateCallback(true, true, &light)
}

AHA_TEST(LightTest, state_command_off) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(AHATOFSTR(StateCommandTopic), F("OFF"));

    assertStateCallback(true, false, &light)
}

AHA_TEST(LightTest, state_command_different_light) {
    prepareTest

    HALight light(testUniqueId);
    light.onStateCommand(onStateCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/cmd_t"),
        F("ON")
    );

    assertStateCallback(false, false, nullptr)
}

AHA_TEST(LightTest, brightness_command_min) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("0"));

    assertBrightnessCallback(true, 0, &light)
}

AHA_TEST(LightTest, brightness_command_max) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("255"));

    assertBrightnessCallback(true, 255, &light)
}

AHA_TEST(LightTest, brightness_command_overflow) {
    prepareTest

    HALight light(testUniqueId);
    light.setBrightnessScale(30);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("100"));

    assertBrightnessCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, brightness_command_invalid) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(AHATOFSTR(BrightnessCommandTopic), F("INVALID"));

    assertBrightnessCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, brightness_command_different_light) {
    prepareTest

    HALight light(testUniqueId);
    light.onBrightnessCommand(onBrightnessCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/pct_cmd_t"),
        F("50")
    );

    assertBrightnessCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, color_temperature_command) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("200"));

    assertColorTempCallback(true, 200, &light)
}

AHA_TEST(LightTest, color_temperature_command_min) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("153"));

    assertColorTempCallback(true, 153, &light)
}

AHA_TEST(LightTest, color_temperature_command_max) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("500"));

    assertColorTempCallback(true, 500, &light)
}

AHA_TEST(LightTest, color_temperature_command_below_min) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setBrightnessScale(30);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("1"));

    assertColorTempCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, color_temperature_command_above_max) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.setBrightnessScale(30);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("1000"));

    assertColorTempCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, color_temperature_command_invalid) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(AHATOFSTR(ColorTemperatureCommandTopic), F("INVALID"));

    assertColorTempCallback(false, 0, nullptr)
}

AHA_TEST(LightTest, color_temperature_command_different_light) {
    prepareTest

    HALight light(testUniqueId, HALight::ColorTemperatureFeature);
    light.onColorTemperatureCommand(onColorTemperatureCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLightDifferent/clr_temp_cmd_t"),
        F("50")
    );

    assertColorTempCallback(false, 0, nullptr)
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
