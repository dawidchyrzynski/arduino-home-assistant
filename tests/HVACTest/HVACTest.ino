#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId)

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueHVAC";

const char ConfigTopic[] PROGMEM = {"homeassistant/climate/testDevice/uniqueHVAC/config"};
const char CurrentTemperatureTopic[] PROGMEM = {"testData/testDevice/uniqueHVAC/ctt"};

AHA_TEST(HVACTest, invalid_unique_id) {
    prepareTest

    HAHVAC hvac(nullptr);
    hvac.buildSerializerTest();
    HASerializer* serializer = hvac.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(HVACTest, default_params) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // config
}

AHA_TEST(HVACTest, availability) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAFan
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueHVAC/avty_t"),
        "online",
        true
    )
}

AHA_TEST(HVACTest, name_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setName("testName");

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, icon_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setIcon("testIcon");

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ic\":\"testIcon\","
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, retain_setter) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setRetain(true);

    assertEntityConfig(
        mock,
        hvac,
        (
            "{"
            "\"uniq_id\":\"uniqueHVAC\","
            "\"ret\":true,"
            "\"ctt\":\"testData/testDevice/uniqueHVAC/ctt\","
            "\"dev\":{\"ids\":\"testDevice\"}"
            "}"
        )
    )
}

AHA_TEST(HVACTest, publish_data_on_connect) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
}

AHA_TEST(HVACTest, publish_nothing_if_retained) {
    prepareTest

    HAHVAC hvac(testUniqueId);
    hvac.setRetain(true);
    hvac.setCurrentTemperature(21.5);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(HVACTest, publish_current_temperature) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);

    assertTrue(hvac.setCurrentTemperature(21.5));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.5", true) 
}

AHA_TEST(HVACTest, publish_current_temperature_debounce) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);

    // it shouldn't publish data if state doesn't change
    assertTrue(hvac.setCurrentTemperature(21.5));
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

AHA_TEST(HVACTest, publish_current_temperature_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HAHVAC hvac(testUniqueId);
    hvac.setCurrentCurrentTemperature(21.5);

    assertTrue(hvac.setCurrentTemperature(21.5, true));
    assertSingleMqttMessage(AHATOFSTR(CurrentTemperatureTopic), "21.5", true)
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
