#include <AUnit.h>
#include <ArduinoHATests.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testComponentName = "testComponent";
static const char* testUniqueId = "uniqueId";

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
}

test(BaseDeviceTypeTest, default_availability) {
    DummyDeviceType deviceType(testComponentName, testUniqueId);
    assertFalse(deviceType.isAvailabilityConfigured());
}

test(BaseDeviceTypeTest, publish_availability_online) {
    const char* availabilityTopic = "testData/testDevice/uniqueId/avty_t";
    PubSubClientMock* mock = new PubSubClientMock();
    HADevice device(testDeviceId);
    HAMqtt mqtt(mock, device);
    mqtt.setDataPrefix("testData");
    DummyDeviceType deviceType(testComponentName, testUniqueId);

    mock->connectDummy();
    deviceType.setAvailability(true);

    assertStringCaseEqual(mock->getMessageTopic(), availabilityTopic);
    assertStringCaseEqual(mock->getMessageBuffer(), F("online"));
    assertTrue(mock->isMessageFlushed());
}

test(BaseDeviceTypeTest, publish_availability_offline) {
    const char* availabilityTopic = "testData/testDevice/uniqueId/avty_t";
    PubSubClientMock* mock = new PubSubClientMock();
    HADevice device(testDeviceId);
    HAMqtt mqtt(mock, device);
    mqtt.setDataPrefix("testData");
    DummyDeviceType deviceType(testComponentName, testUniqueId);

    mock->connectDummy();
    deviceType.setAvailability(false);

    assertStringCaseEqual(mock->getMessageTopic(), availabilityTopic);
    assertStringCaseEqual(mock->getMessageBuffer(), F("offline"));
    assertTrue(mock->isMessageFlushed());
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
