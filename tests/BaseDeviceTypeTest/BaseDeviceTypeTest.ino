#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    DummyDeviceType deviceType(testComponentName, testUniqueId); \
    mock->connectDummy();

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testComponentName = "testComponent";
static const char* testUniqueId = "uniqueId";
static const char* availabilityTopic = "testData/testDevice/uniqueId/avty_t";

class DummyDeviceType : public BaseDeviceType
{
public:
    DummyDeviceType(const char* componentName, const char* uniqueId) :
        BaseDeviceType(componentName, uniqueId) { }

protected:
    virtual void onMqttConnected() override { }
};

test(BaseDeviceTypeTest, constructor_params) {
    DummyDeviceType deviceType(testComponentName, testUniqueId);
    assertStringCaseEqual(testComponentName, deviceType.componentName());
    assertStringCaseEqual(testUniqueId, deviceType.uniqueId());
}

test(BaseDeviceTypeTest, register_mqtt_type) {
    HADevice device(testDeviceId);
    HAMqtt mqtt(nullptr, device);
    DummyDeviceType deviceType(testComponentName, testUniqueId);

    assertEqual((uint8_t)1, mqtt.getDevicesTypesNb());
    assertEqual(&deviceType, mqtt.getDevicesTypes()[0]);
}

test(BaseDeviceTypeTest, default_name) {
    DummyDeviceType deviceType(testComponentName, testUniqueId);
    assertEqual((const char*)nullptr, deviceType.getName());
}

test(BaseDeviceTypeTest, name_setter) {
    const char* name = "testName";
    DummyDeviceType deviceType(testComponentName, testUniqueId);
    deviceType.setName(name);
    assertEqual(name, deviceType.getName());
}

test(BaseDeviceTypeTest, default_availability) {
    DummyDeviceType deviceType(testComponentName, testUniqueId);
    assertFalse(deviceType.isAvailabilityConfigured());
}

test(BaseDeviceTypeTest, publish_availability_online) {
    prepareTest

    deviceType.setAvailability(true);
    assertSingleMqttMessage(availabilityTopic, "online", true)
}

test(BaseDeviceTypeTest, publish_availability_offline) {
    prepareTest

    deviceType.setAvailability(false);
    assertSingleMqttMessage(availabilityTopic, "offline", true)
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
