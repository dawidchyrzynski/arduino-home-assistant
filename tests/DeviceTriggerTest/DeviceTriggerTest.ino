#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* triggerType = "myType";
static const char* triggerSubtype = "mySubtype";

const char ConfigTopic[] PROGMEM = {
    "homeassistant/device_automation/testDevice/myType_mySubtype/config"
};

AHA_TEST(DeviceTriggerTest, invalid_type) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(nullptr, triggerSubtype);
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(DeviceTriggerTest, invalid_type_progmem) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(
        static_cast<HADeviceTrigger::TriggerType>(50),
        triggerSubtype
    );
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(trigger.getType() == nullptr);
    assertTrue(trigger.isProgmemType());
}

AHA_TEST(DeviceTriggerTest, invalid_subtype) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, nullptr);
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(DeviceTriggerTest, invalid_subtype_progmem) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(
        triggerType,
        static_cast<HADeviceTrigger::TriggerSubtype>(50)
    );
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(trigger.getSubtype() == nullptr);
    assertTrue(trigger.isProgmemSubtype());
}

AHA_TEST(DeviceTriggerTest, unique_id_generator) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, triggerSubtype);
    assertEqual(trigger.uniqueId(), "myType_mySubtype");
}

AHA_TEST(DeviceTriggerTest, string_type_string_subtype) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, triggerSubtype);
    assertEntityConfig(
        mock,
        trigger,
        (
            "{"
            "\"atype\":\"trigger\","
            "\"type\":\"myType\","
            "\"stype\":\"mySubtype\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/myType_mySubtype/t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTriggerTest, progmem_type_string_subtype) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(HADeviceTrigger::ButtonShortPressType, triggerSubtype);
    assertEntityConfigOnTopic(
        mock,
        trigger,
        F("homeassistant/device_automation/testDevice/button_short_press_mySubtype/config"),
        (
            "{"
            "\"atype\":\"trigger\","
            "\"type\":\"button_short_press\","
            "\"stype\":\"mySubtype\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/button_short_press_mySubtype/t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTriggerTest, string_type_progmem_subtype) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, HADeviceTrigger::Button1Subtype);
    assertEntityConfigOnTopic(
        mock,
        trigger,
        F("homeassistant/device_automation/testDevice/myType_button_1/config"),
        (
            "{"
            "\"atype\":\"trigger\","
            "\"type\":\"myType\","
            "\"stype\":\"button_1\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/myType_button_1/t\""
            "}"
        )
    )
}

AHA_TEST(DeviceTriggerTest, trigger) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTrigger trigger(triggerType, triggerSubtype);
    bool result = trigger.trigger();

    assertSingleMqttMessage(
        F("testData/testDevice/myType_mySubtype/t"),
        "",
        false
    )
    assertTrue(result);
}

AHA_TEST(DeviceTriggerTest, trigger_progmem_type) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTrigger trigger(HADeviceTrigger::ButtonShortPressType, triggerSubtype);
    bool result = trigger.trigger();

    assertSingleMqttMessage(
        F("testData/testDevice/button_short_press_mySubtype/t"),
        "",
        false
    )
    assertTrue(result);
}

AHA_TEST(DeviceTriggerTest, trigger_progmem_subtype) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTrigger trigger(triggerType, HADeviceTrigger::TurnOnSubtype);
    bool result = trigger.trigger();

    assertSingleMqttMessage(
        F("testData/testDevice/myType_turn_on/t"),
        "",
        false
    )
    assertTrue(result);
}

AHA_TEST(DeviceTriggerTest, trigger_progmem_type_subtype) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonShortPressType,
        HADeviceTrigger::TurnOnSubtype
    );
    bool result = trigger.trigger();

    assertSingleMqttMessage(
        F("testData/testDevice/button_short_press_turn_on/t"),
        "",
        false
    )
    assertTrue(result);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_short_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonShortPressType,
        nullptr
    );

    assertEqual(HAButtonShortPressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_short_release) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonShortReleaseType,
        nullptr
    );

    assertEqual(HAButtonShortReleaseType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_long_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonLongPressType,
        nullptr
    );

    assertEqual(HAButtonLongPressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_long_release) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonLongReleaseType,
        nullptr
    );

    assertEqual(HAButtonLongReleaseType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_double_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonDoublePressType,
        nullptr
    );

    assertEqual(HAButtonDoublePressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_triple_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonTriplePressType,
        nullptr
    );

    assertEqual(HAButtonTriplePressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_quadruple_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonQuadruplePressType,
        nullptr
    );

    assertEqual(HAButtonQuadruplePressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, type_progmem_button_quintuple_press) {
    HADeviceTrigger trigger(
        HADeviceTrigger::ButtonQuintuplePressType,
        nullptr
    );

    assertEqual(HAButtonQuintuplePressType, trigger.getType());
    assertTrue(trigger.isProgmemType());
    assertTrue(trigger.getSubtype() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_turn_on) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::TurnOnSubtype
    );

    assertEqual(HATurnOnSubtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_turn_off) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::TurnOffSubtype
    );

    assertEqual(HATurnOffSubtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_1) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button1Subtype
    );

    assertEqual(HAButton1Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_2) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button2Subtype
    );

    assertEqual(HAButton2Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_3) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button3Subtype
    );

    assertEqual(HAButton3Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_4) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button4Subtype
    );

    assertEqual(HAButton4Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_5) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button5Subtype
    );

    assertEqual(HAButton5Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
}

AHA_TEST(DeviceTriggerTest, subtype_progmem_button_6) {
    HADeviceTrigger trigger(
        nullptr,
        HADeviceTrigger::Button6Subtype
    );

    assertEqual(HAButton6Subtype, trigger.getSubtype());
    assertTrue(trigger.isProgmemSubtype());
    assertTrue(trigger.getType() == nullptr);
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
