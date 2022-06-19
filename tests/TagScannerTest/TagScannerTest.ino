#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueScanner";
static const char* configTopic = "homeassistant/tag/testDevice/uniqueScanner/config";

test(TagScannerTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HATagScanner scanner(nullptr);
    scanner.buildSerializerTest();
    HASerializer* serializer = scanner.getSerializer();

    assertTrue(serializer == nullptr);
}

test(TagScannerTest, default_params) {
    initMqttTest(testDeviceId)

    HATagScanner scanner(testUniqueId);
    assertEntityConfig(
        mock,
        scanner,
        "{\"dev\":{\"ids\":\"testDevice\"},\"t\":\"testData/testDevice/uniqueScanner/t\"}"
    )
}

test(TagScannerTest, nullptr_tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);
    bool result = scanner.tagScanned(nullptr);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertFalse(result);
}

test(TagScannerTest, empty_tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);
    bool result = scanner.tagScanned("");

    assertEqual(0, mock->getFlushedMessagesNb());
    assertFalse(result);
}

test(TagScannerTest, tag_scanned) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HATagScanner scanner(testUniqueId);
    bool result = scanner.tagScanned("helloTag");

    assertSingleMqttMessage(
        "testData/testDevice/uniqueScanner/t",
        "helloTag",
        false
    )
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
