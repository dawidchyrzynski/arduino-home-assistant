#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueScanner";

const char ConfigTopic[] PROGMEM = {"homeassistant/tag/testDevice/uniqueScanner/config"};

AHA_TEST(TagScannerTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HATagScanner scanner(nullptr);
    scanner.buildSerializerTest();
    HASerializer* serializer = scanner.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(TagScannerTest, default_params) {
    initMqttTest(testDeviceId)

    HATagScanner scanner(testUniqueId);
    assertEntityConfig(
        mock,
        scanner,
        (
            "{"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueScanner/t\""
            "}"
        )
    )
}

AHA_TEST(TagScannerTest, nullptr_tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);

    assertFalse(scanner.tagScanned(nullptr));
    assertEqual(0, mock->getFlushedMessagesNb());
}

AHA_TEST(TagScannerTest, empty_tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);

    assertFalse(scanner.tagScanned(""));
    assertEqual(0, mock->getFlushedMessagesNb());
}

AHA_TEST(TagScannerTest, tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);

    assertTrue(scanner.tagScanned("helloTag"));
    assertSingleMqttMessage(
        F("testData/testDevice/uniqueScanner/t"),
        "helloTag",
        false
    )
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
