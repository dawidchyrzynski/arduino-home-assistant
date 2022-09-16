#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackNumber = HANumber::StateNone; \
    commandCallbackPrecision = 0; \
    commandCallbackNumberPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedNumber, expectedPrecision, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual((HAUtils::Number)expectedNumber, commandCallbackNumber); \
    assertEqual(expectedPrecision, commandCallbackPrecision); \
    assertEqual(callerPtr, commandCallbackNumberPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueNumber";

const char ConfigTopic[] PROGMEM = {"homeassistant/number/testDevice/uniqueNumber/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/cmd_t"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/stat_t"};

static bool commandCallbackCalled = false;
static HAUtils::Number commandCallbackNumber = HANumber::StateNone;
static uint8_t commandCallbackPrecision = 0;
static HANumber* commandCallbackNumberPtr = nullptr;

void onCommandReceived(HAUtils::Number number, uint8_t precision, HANumber* sender)
{
    commandCallbackCalled = true;
    commandCallbackNumber = number;
    commandCallbackPrecision = precision;
    commandCallbackNumberPtr = sender;
}

AHA_TEST(NumberTest, invalid_unique_id) {
    prepareTest

    HANumber number(nullptr);
    number.buildSerializerTest();
    HASerializer* serializer = number.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(NumberTest, default_params) {
    prepareTest

    HANumber number(testUniqueId);
    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(NumberTest, command_subscription) {
    prepareTest

    HANumber number(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(NumberTest, availability) {
    prepareTest

    HANumber number(testUniqueId);
    number.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HANumber
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueNumber/avty_t"),
        "online",
        true
    )
}

AHA_TEST(NumberTest, publish_state_none) {
    prepareTest

    HANumber number(testUniqueId);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "None", true)
}

AHA_TEST(NumberTest, publish_last_known_state_p0) {
    prepareTest

    HANumber number(testUniqueId);
    number.setCurrentState(50);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "50", true)
}

AHA_TEST(NumberTest, publish_last_known_state_p1) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setCurrentState(50);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "50.0", true)
}

AHA_TEST(NumberTest, publish_last_known_state_p2) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setCurrentState(8888);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "8888.00", true)
}

AHA_TEST(NumberTest, publish_last_known_state_p3) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setCurrentState(1.234);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "1.234", true)
}

AHA_TEST(NumberTest, publish_nothing_if_retained) {
    prepareTest

    HANumber number(testUniqueId);
    number.setRetain(true);
    number.setCurrentState(50);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(NumberTest, name_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setName("testName");

    assertEntityConfig(
        mock,
        number,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueNumber\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, device_class) {
    prepareTest

    HANumber number(testUniqueId);
    number.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, icon_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setIcon("testIcon");

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, retain_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setRetain(true);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, optimistic_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setOptimistic(true);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"opt\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, mode_setter_box) {
    prepareTest

    HANumber number(testUniqueId);
    number.setMode(HANumber::ModeBox);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"mode\":\"box\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, mode_setter_slider) {
    prepareTest

    HANumber number(testUniqueId);
    number.setMode(HANumber::ModeSlider);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"mode\":\"slider\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"unit_of_meas\":\"%\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, min_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setMin(2);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"min\":2,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, min_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setMin(2.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**1}}\",\"min\":2.5,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, min_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setMin(95467.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**2}}\",\"min\":95467.50,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, min_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setMin(50.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**3}}\",\"min\":50.500,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, max_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setMax(2);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"max\":2,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, max_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setMax(2.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**1}}\",\"max\":2.5,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, max_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setMax(95467.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**2}}\",\"max\":95467.50,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, max_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setMax(50.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**3}}\",\"max\":50.500,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, step_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setStep(2);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"step\":2,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, step_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setStep(2.5);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**1}}\",\"step\":2.5,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, step_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setStep(0.01);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**2}}\",\"step\":0.01,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, step_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setStep(0.001);

    assertEntityConfig(
        mock,
        number,
        "{\"uniq_id\":\"uniqueNumber\",\"cmd_tpl\":\"{{float(value)/10**3}}\",\"step\":0.001,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\"}"
    )
}

AHA_TEST(NumberTest, command_none_p0) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("None"));

    assertCallback(true, HANumber::StateNone, 0, &number)
}

AHA_TEST(NumberTest, command_none_p1) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("None"));

    assertCallback(true, HANumber::StateNone, 1, &number)
}

AHA_TEST(NumberTest, command_none_p2) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("None"));

    assertCallback(true, HANumber::StateNone, 2, &number)
}

AHA_TEST(NumberTest, command_none_p3) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("None"));

    assertCallback(true, HANumber::StateNone, 3, &number)
}

AHA_TEST(NumberTest, command_number_zero) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("0"));

    assertCallback(true, 0, 0, &number)
}

AHA_TEST(NumberTest, command_number_unsigned) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("1234"));

    assertCallback(true, 1234, 0, &number)
}

AHA_TEST(NumberTest, command_number_signed) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("-1234"));

    assertCallback(true, -1234, 0, &number)
}

AHA_TEST(NumberTest, command_number_invalid) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("abc"));

    assertCallback(false, HANumber::StateNone, 0, nullptr)
}

AHA_TEST(NumberTest, different_number_command) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueCoverDifferent/cmd_t"),
        F("123")
    );

    assertCallback(false, HANumber::StateNone, 0, nullptr)
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
