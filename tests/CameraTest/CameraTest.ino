#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCamera";
static const char* configTopic = "homeassistant/camera/testDevice/uniqueCamera/config";
static const char* dataTopic = "testData/testDevice/uniqueCamera/t";

test(CameraTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HACamera camera(nullptr);
    camera.buildSerializerTest();
    HASerializer* serializer = camera.getSerializer();

    assertTrue(serializer == nullptr);
}

test(CameraTest, default_params) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    assertEntityConfig(
        mock,
        camera,
        "{\"uniq_id\":\"uniqueCamera\",\"dev\":{\"ids\":\"testDevice\"},\"t\":\"testData/testDevice/uniqueCamera/t\"}"
    )
}

test(CameraTest, availability) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACamera
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueCamera/avty_t",
        "online",
        true
    )
}

test(CameraTest, icon_setter) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setIcon("testIcon");

    assertEntityConfig(
        mock,
        camera,
        "{\"uniq_id\":\"uniqueCamera\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"t\":\"testData/testDevice/uniqueCamera/t\"}"
    )
}

test(CameraTest, encoding_base64) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setEncoding(HACamera::EncodingBase64);

    assertEntityConfig(
        mock,
        camera,
        "{\"uniq_id\":\"uniqueCamera\",\"e\":\"b64\",\"dev\":{\"ids\":\"testDevice\"},\"t\":\"testData/testDevice/uniqueCamera/t\"}"
    )
}

test(CameraTest, publish_nullptr) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HACamera camera(testUniqueId);

    bool result = camera.publishImage(nullptr);

    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertFalse(result);
}

test(CameraTest, publish_image) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HACamera camera(testUniqueId);

    bool result = camera.publishImage("IMAGE CONTENT");

    assertSingleMqttMessage(dataTopic, "IMAGE CONTENT", true)
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
