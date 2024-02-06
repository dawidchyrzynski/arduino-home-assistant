#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastCommandCallbackCall.reset();

#define assertCommandCallbackCalled(expectedNumber, callerPtr) \
    assertTrue(lastCommandCallbackCall.called); \
    assertTrue(expectedNumber == lastCommandCallbackCall.number); \
    assertEqual(callerPtr, lastCommandCallbackCall.caller);

#define assertCommandCallbackNotCalled() \
    assertFalse(lastCommandCallbackCall.called);

using aunit::TestRunner;

struct CommandCallback {
    bool called = false;
    HANumeric number;
    HANumber* caller = nullptr;

    void reset() {
        called = false;
        number.reset();
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueNumber";
static CommandCallback lastCommandCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/number/testDevice/uniqueNumber/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/cmd_t"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueNumber/stat_t"};

void onCommandReceived(HANumeric number, HANumber* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.number = number;
    lastCommandCallbackCall.caller = caller;
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
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(NumberTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HANumber number(testUniqueId);
    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueNumber\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
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

AHA_TEST(NumberTest, publish_nothing_if_retained) {
    prepareTest

    HANumber number(testUniqueId);
    number.setRetain(true);
    number.setCurrentState(50);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(NumberTest, publish_state_none) {
    prepareTest

    HANumber number(testUniqueId);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "None", true)
}

AHA_TEST(NumberTest, set_state_int8) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    int8_t value = -123;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-123", true)
}

AHA_TEST(NumberTest, set_state_int16) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    int16_t value = -12345;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-12345", true)
}

AHA_TEST(NumberTest, set_state_int32) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    int32_t value = -1234567;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-1234567", true)
}

AHA_TEST(NumberTest, set_state_uint8) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    uint8_t value = 254;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "254", true)
}

AHA_TEST(NumberTest, set_state_uint16) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    uint16_t value = 65200;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "65200", true)
}

AHA_TEST(NumberTest, set_state_uint32) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId);
    uint32_t value = 105200;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "105200", true)
}

AHA_TEST(NumberTest, set_state_float_p1) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId, HANumber::PrecisionP1);
    float value = 25.1f;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "25.1", true)
}

AHA_TEST(NumberTest, set_state_float_p2) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId, HANumber::PrecisionP2);
    float value = -25.1f;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-25.10", true)
}

AHA_TEST(NumberTest, set_state_float_p3) {
    prepareTest

    mock->connectDummy();
    HANumber number(testUniqueId, HANumber::PrecisionP3);
    float value = -0.333333f;

    assertTrue(number.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "-0.333", true)
}

AHA_TEST(NumberTest, current_state_setter_getter_int8) {
    prepareTest

    HANumber number(testUniqueId);
    int8_t value = -123;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isInt8());
    assertEqual(value, number.getCurrentState().toInt8());
}

AHA_TEST(NumberTest, current_state_setter_getter_int16) {
    prepareTest

    HANumber number(testUniqueId);
    int16_t value = -1234;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isInt16());
    assertEqual(value, number.getCurrentState().toInt16());
}

AHA_TEST(NumberTest, current_state_setter_getter_int32) {
    prepareTest

    HANumber number(testUniqueId);
    int32_t value = -12345;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isInt32());
    assertEqual(value, number.getCurrentState().toInt32());
}

AHA_TEST(NumberTest, current_state_setter_getter_uint8) {
    prepareTest

    HANumber number(testUniqueId);
    uint8_t value = 254;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isUInt8());
    assertEqual(value, number.getCurrentState().toUInt8());
}

AHA_TEST(NumberTest, current_state_setter_getter_uint16) {
    prepareTest

    HANumber number(testUniqueId);
    uint16_t value = 12345;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isUInt16());
    assertEqual(value, number.getCurrentState().toUInt16());
}

AHA_TEST(NumberTest, current_state_setter_getter_uint32) {
    prepareTest

    HANumber number(testUniqueId);
    uint32_t value = 1234567;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isUInt32());
    assertEqual(value, number.getCurrentState().toUInt32());
}

AHA_TEST(NumberTest, current_state_setter_getter_float) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    float value = 25.0f;

    number.setCurrentState(value);
    assertTrue(number.getCurrentState().isFloat());
    assertNear(value, number.getCurrentState().toFloat(), 0.1);
}

AHA_TEST(NumberTest, name_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setName("testName");

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueNumber\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, object_id_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setObjectId("testId");

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueNumber\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, device_class) {
    prepareTest

    HANumber number(testUniqueId);
    number.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"dev_cla\":\"testClass\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, icon_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setIcon("testIcon");

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, retain_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setRetain(true);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, optimistic_setter) {
    prepareTest

    HANumber number(testUniqueId);
    number.setOptimistic(true);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, mode_setter_box) {
    prepareTest

    HANumber number(testUniqueId);
    number.setMode(HANumber::ModeBox);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"mode\":\"box\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, mode_setter_slider) {
    prepareTest

    HANumber number(testUniqueId);
    number.setMode(HANumber::ModeSlider);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"mode\":\"slider\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"unit_of_meas\":\"%\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, min_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setMin(2);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"min\":2,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, min_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setMin(2.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**1)}}\","
            "\"min\":2.5,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, min_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setMin(95467.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**2)}}\","
            "\"min\":95467.50,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, min_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setMin(50.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**3)}}\","
            "\"min\":50.500,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, max_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setMax(2);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"max\":2,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, max_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setMax(2.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**1)}}\","
            "\"max\":2.5,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, max_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setMax(95467.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**2)}}\","
            "\"max\":95467.50,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, max_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setMax(50.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**3)}}\","
            "\"max\":50.500,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, step_setter_p0) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId);
    number.setStep(2);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"step\":2,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, step_setter_p1) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.setStep(2.5);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**1)}}\","
            "\"step\":2.5,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, step_setter_p2) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.setStep(0.01);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**2)}}\","
            "\"step\":0.01,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, step_setter_p3) {
    initMqttTest(testDeviceId)

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.setStep(0.001);

    assertEntityConfig(
        mock,
        number,
        (
            "{"
            "\"uniq_id\":\"uniqueNumber\","
            "\"cmd_tpl\":\"{{int(float(value)*10**3)}}\","
            "\"step\":0.001,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueNumber/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueNumber/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(NumberTest, command_none) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("None"));

    assertCommandCallbackCalled(HANumeric(), &number)
}

AHA_TEST(NumberTest, command_number_zero) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("0"));


    assertCommandCallbackCalled(HANumeric(0, 0), &number)
}

AHA_TEST(NumberTest, command_number_unsigned) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("1234"));

    assertCommandCallbackCalled(HANumeric(1234, 0), &number)
}

AHA_TEST(NumberTest, command_number_signed) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("-1234"));

    assertCommandCallbackCalled(HANumeric(-1234, 0), &number)
}

AHA_TEST(NumberTest, command_number_float_p1) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP1);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("-1234"));

    assertCommandCallbackCalled(HANumeric(-123.4f, 1), &number)
}

AHA_TEST(NumberTest, command_number_float_p2) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP2);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("-1234"));

    assertCommandCallbackCalled(HANumeric(-12.34f, 2), &number)
}

AHA_TEST(NumberTest, command_number_float_p3) {
    prepareTest

    HANumber number(testUniqueId, HANumber::PrecisionP3);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("-1234"));

    assertCommandCallbackCalled(HANumeric(-1.234f, 3), &number)
}

AHA_TEST(NumberTest, command_number_invalid) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("abc"));

    assertCommandCallbackNotCalled()
}

AHA_TEST(NumberTest, different_number_command) {
    prepareTest

    HANumber number(testUniqueId);
    number.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueCoverDifferent/cmd_t"),
        F("123")
    );

    assertCommandCallbackNotCalled()
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
