#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueId";

const char AvailabilityTopic[] PROGMEM = {"testData/testDevice/uniqueId/avty_t"};
const char SharedAvailabilityTopic[] PROGMEM = {"testData/testDevice/avty_t"};
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

AHA_TEST(BaseDeviceTypeTest, constructor_params) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertEqual(AHATOFSTR(ComponentNameStr), deviceType.componentName());
    assertEqual(testUniqueId, deviceType.uniqueId());
}

AHA_TEST(BaseDeviceTypeTest, register_mqtt_type) {
    HADevice device(testDeviceId);
    HAMqtt mqtt(nullptr, device);
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);

    assertEqual((uint8_t)1, mqtt.getDevicesTypesNb());
    assertEqual(&deviceType, mqtt.getDevicesTypes()[0]);
}

AHA_TEST(BaseDeviceTypeTest, default_name) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertEqual((const char*)nullptr, deviceType.getName());
}

AHA_TEST(BaseDeviceTypeTest, name_setter) {
    const char* name = "testName";
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    deviceType.setName(name);
    assertEqual(name, deviceType.getName());
}

AHA_TEST(BaseDeviceTypeTest, object_id_setter) {
    const char* objectId = "testId";
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    deviceType.setObjectId(objectId);
    assertEqual(objectId, deviceType.getObjectId());
}

AHA_TEST(BaseDeviceTypeTest, default_availability) {
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);
    assertFalse(deviceType.isAvailabilityConfigured());
}

AHA_TEST(BaseDeviceTypeTest, publish_nothing_if_not_configured) {
    prepareTest

    mqtt.loop();
    assertNoMqttMessage();
}

AHA_TEST(BaseDeviceTypeTest, publish_availability_online_runtime) {
    prepareTest

    mock->connectDummy();
    deviceType.setAvailability(true);
    assertSingleMqttMessage(AHATOFSTR(AvailabilityTopic), "online", true)
}

AHA_TEST(BaseDeviceTypeTest, publish_availability_offline_runtime) {
    prepareTest

    mock->connectDummy();
    deviceType.setAvailability(false);
    assertSingleMqttMessage(AHATOFSTR(AvailabilityTopic), "offline", true)
}

AHA_TEST(BaseDeviceTypeTest, publish_shared_availability_on_connect) {
    prepareTest

    device.enableSharedAvailability();
    mqtt.loop();
    assertSingleMqttMessage(AHATOFSTR(SharedAvailabilityTopic), "online", true)
}

AHA_TEST(BaseDeviceTypeTest, publish_shared_availability_runtime) {
    prepareTest

    device.enableSharedAvailability();
    mock->connectDummy();
    device.setAvailability(false);

    assertSingleMqttMessage(AHATOFSTR(SharedAvailabilityTopic), "offline", true)
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
