#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueId";

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

AHA_TEST(MqttTest, maximum_number_of_device_types) {
    HADevice device(testDeviceId);
    HAMqtt mqtt(nullptr, device, 1);
    DummyDeviceType deviceType(AHATOFSTR(ComponentNameStr), testUniqueId);

    assertEqual((uint8_t)1, mqtt.getDevicesTypesNb());
    assertEqual(&deviceType, mqtt.getDevicesTypes()[0]);
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
