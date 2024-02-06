#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueCamera";

const char ConfigTopic[] PROGMEM = {"homeassistant/camera/testDevice/uniqueCamera/config"};
const char DataTopic[] PROGMEM = {"testData/testDevice/uniqueCamera/t"};

AHA_TEST(CameraTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HACamera camera(nullptr);
    camera.buildSerializerTest();
    HASerializer* serializer = camera.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(CameraTest, default_params) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"uniq_id\":\"uniqueCamera\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}

AHA_TEST(CameraTest, extended_unique_id) {
    initMqttTest(testDeviceId)

    device.enableExtendedUniqueIds();
    HACamera camera(testUniqueId);
    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueCamera\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}

AHA_TEST(CameraTest, availability) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACamera
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueCamera/avty_t"),
        "online",
        true
    )
}

AHA_TEST(CameraTest, name_setter) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setName("testName");

    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueCamera\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}

AHA_TEST(CameraTest, object_id_setter) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setObjectId("testId");

    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueCamera\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}


AHA_TEST(CameraTest, icon_setter) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setIcon("testIcon");

    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"uniq_id\":\"uniqueCamera\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}

AHA_TEST(CameraTest, encoding_base64) {
    initMqttTest(testDeviceId)

    HACamera camera(testUniqueId);
    camera.setEncoding(HACamera::EncodingBase64);

    assertEntityConfig(
        mock,
        camera,
        (
            "{"
            "\"uniq_id\":\"uniqueCamera\","
            "\"e\":\"b64\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"t\":\"testData/testDevice/uniqueCamera/t\""
            "}"
        )
    )
}

AHA_TEST(CameraTest, publish_nullptr) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HACamera camera(testUniqueId);

    bool result = camera.publishImage(nullptr, 0);

    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertFalse(result);
}

AHA_TEST(CameraTest, publish_image) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HACamera camera(testUniqueId);

    const char* data = "IMAGE CONTENT";
    bool result = camera.publishImage((const uint8_t*)data, strlen(data));

    assertSingleMqttMessage(AHATOFSTR(DataTopic), "IMAGE CONTENT", true)
    assertTrue(result);
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
