#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId)

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCover";
static const char* configTopic = "homeassistant/cover/testDevice/uniqueCover/config";
static const char* commandTopic = "testData/testDevice/uniqueCover/cmd_t";
static const char* commandMessage = "PRESS";

test(CoverTest, invalid_unique_id) {
    prepareTest

    HACover cover(nullptr);
    cover.buildSerializerTest();
    HASerializer* serializer = cover.getSerializer();

    assertTrue(serializer == nullptr);
}

test(CoverTest, default_params) {
    prepareTest

    HACover cover(testUniqueId);
    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

test(CoverTest, command_subscription) {
    prepareTest

    HACover cover(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertStringCaseEqual(commandTopic, mock->getSubscriptions()[0].topic);
}

test(CoverTest, availability) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACover
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueCover/avty_t",
        "online",
        true
    )
}

test(CoverTest, publish_default_position) {
    initMqttTest(testDeviceId)

    HACover cover(testUniqueId, true);
    mqtt.loop();

    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSensor/stat_t",
        "ON",
        true
    )
}


test(CoverTest, name_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setName("testName");

    assertEntityConfig(
        mock,
        cover,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueCover\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

test(CoverTest, device_class) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

test(CoverTest, icon_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setIcon("testIcon");

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

test(CoverTest, retain_setter) {
    prepareTest

    HACover cover(testUniqueId);
    cover.setRetain(true);

    assertEntityConfig(
        mock,
        cover,
        "{\"uniq_id\":\"uniqueCover\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueCover/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueCover/cmd_t\",\"pos_t\":\"testData/testDevice/uniqueCover/pos_t\"}"
    )
}

// to do

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
