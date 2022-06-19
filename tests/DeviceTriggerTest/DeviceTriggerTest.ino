#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* triggerType = "myType";
static const char* triggerSubtype = "mySubtype";
static const char* configTopic = "homeassistant/device_automation/testDevice/myType_mySubtype/config";
static const char* triggerTopic = "testData/testDevice/myType_mySubtype/t";

test(DeviceTriggerTest, invalid_type) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(nullptr, triggerSubtype);
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
}

test(DeviceTriggerTest, invalid_type_progmem) {
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

test(DeviceTriggerTest, invalid_subtype) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, nullptr);
    trigger.buildSerializerTest();
    HASerializer* serializer = trigger.getSerializer();

    assertTrue(serializer == nullptr);
}

test(DeviceTriggerTest, invalid_subtype_progmem) {
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

test(DeviceTriggerTest, unique_id_generator) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, triggerSubtype);
    assertStringCaseEqual(trigger.uniqueId(), "myType_mySubtype");
}

test(DeviceTriggerTest, default_params) {
    initMqttTest(testDeviceId)

    HADeviceTrigger trigger(triggerType, triggerSubtype);
    assertEntityConfig(
        mock,
        trigger,
        "{\"atype\":\"trigger\",\"type\":\"myType\",\"stype\":\"mySubtype\",\"dev\":{\"ids\":\"testDevice\"},\"t\":\"testData/testDevice/myType_mySubtype/t\"}"
    )
}

test(DeviceTriggerTest, trigger) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HADeviceTrigger trigger(triggerType, triggerSubtype);
    bool result = trigger.trigger();

    assertSingleMqttMessage(triggerTopic, "", false)
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
