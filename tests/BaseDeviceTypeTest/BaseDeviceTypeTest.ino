#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueId";
static const char* availabilityTopic = "testData/testDevice/uniqueId/avty_t";
static const char* sharedAvailabilityTopic = "testData/testDevice/avty_t";

const char ComponentNameStr[] PROGMEM = {"componentName"};

class DummyDeviceType : public HABaseDeviceType
{
public:
    DummyDeviceType(const __FlashStringHelper* componentName, const char* uniqueId) :
        HABaseDeviceType(componentName, uniqueId) { }

protected:
    virtual void onMqttConnected() override {
        publishAvailability();
    }
};

test(BaseDeviceTypeTest, constructor_params) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertEqual(AHATOFSTR(ComponentNameStr), deviceType.componentName());
    assertEqual(testUniqueId, deviceType.uniqueId());
}

test(BaseDeviceTypeTest, register_mqtt_type) {
    HADevice device(testDeviceId);
    HAMqtt mqtt(nullptr, device);
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);

    assertEqual((uint8_t)1, mqtt.getDevicesTypesNb());
    assertEqual(&deviceType, mqtt.getDevicesTypes()[0]);
}

test(BaseDeviceTypeTest, default_name) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertEqual((const char*)nullptr, deviceType.getName());
}

test(BaseDeviceTypeTest, name_setter) {
    const char* name = "testName";
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    deviceType.setName(name);
    assertEqual(name, deviceType.getName());
}

test(BaseDeviceTypeTest, default_availability) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertFalse(deviceType.isAvailabilityConfigured());
}

test(BaseDeviceTypeTest, publish_nothing_if_not_configured) {
    prepareTest

    mqtt.loop();
    assertNoMqttMessage();
}

test(BaseDeviceTypeTest, publish_availability_online_runtime) {
    prepareTest

    mock->connectDummy();
    deviceType.setAvailability(true);
    assertSingleMqttMessage(availabilityTopic, "online", true)
}

test(BaseDeviceTypeTest, publish_availability_offline_runtime) {
    prepareTest

    mock->connectDummy();
    deviceType.setAvailability(false);
    assertSingleMqttMessage(availabilityTopic, "offline", true)
}

test(BaseDeviceTypeTest, publish_shared_availability_on_connect) {
    prepareTest

    device.enableSharedAvailability();
    mqtt.loop();
    assertSingleMqttMessage(sharedAvailabilityTopic, "online", true)
}

test(BaseDeviceTypeTest, publish_shared_availability_runtime) {
    prepareTest

    device.enableSharedAvailability();
    mock->connectDummy();
    device.setAvailability(false);

    assertSingleMqttMessage(sharedAvailabilityTopic, "offline", true)
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
