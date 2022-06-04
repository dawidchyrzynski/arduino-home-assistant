#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device);

#define assertConfig(mock, expectedJson) \
{ \
    mock->connectDummy(); \
    button.publishConfigTest(); \
    assertStringCaseEqual(mock->getMessageTopic(), configTopic); \
    assertStringCaseEqual(mock->getMessageBuffer(), F(expectedJson)); \
    assertEqual(mock->getMessageLength(), (size_t)strlen_P(reinterpret_cast<const char *>(expectedJson))); \
    assertTrue(mock->isMessageRetained()); \
    assertTrue(mock->isMessageFlushed()); \
    assertTrue(button.getSerializer() == nullptr); \
}

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueButton";
static const char* configTopic = "homeassistant/button/testDevice/uniqueButton/config";

test(ButtonTest, invalid_unique_id) {
    prepareTest

    HAButton button(nullptr);
    button.buildSerializerTest();
    HASerializer* serializer = button.getSerializer();

    assertTrue(serializer == nullptr);
}

test(ButtonTest, default_params) {
    prepareTest

    HAButton button(testUniqueId);
    assertConfig(mock, "{\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"aha/testDevice/uniqueButton/cmd_t\"}");
}

test(ButtonTest, name_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setName("testName");

    assertConfig(mock, "{\"name\":\"testName\",\"uniq_id\":\"uniqueButton\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"aha/testDevice/uniqueButton/cmd_t\"}");
}

test(ButtonTest, device_class) {
    prepareTest

    HAButton button(testUniqueId);
    button.setDeviceClass("testClass");

    assertConfig(mock, "{\"uniq_id\":\"uniqueButton\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"aha/testDevice/uniqueButton/cmd_t\"}");
}

test(ButtonTest, icon_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setIcon("testIcon");

    assertConfig(mock, "{\"uniq_id\":\"uniqueButton\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"aha/testDevice/uniqueButton/cmd_t\"}");
}

test(ButtonTest, retain_setter) {
    prepareTest

    HAButton button(testUniqueId);
    button.setRetain(true);

    assertConfig(mock, "{\"uniq_id\":\"uniqueButton\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"cmd_t\":\"aha/testDevice/uniqueButton/cmd_t\"}");
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
