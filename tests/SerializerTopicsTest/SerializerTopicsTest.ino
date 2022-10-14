#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

const char* deviceId = "testDevice";
const char* dataPrefix = "dataPrefix";
const char* discoveryPrefix = "discoveryPrefix";

const char DummyProgmemStr[] PROGMEM = {"dummyProgmem"};
const char ComponentNameStr[] PROGMEM = {"componentName"};

char tmpBuffer[64];

void clearTmpBuffer() {
    memset(tmpBuffer, 0, sizeof(tmpBuffer));
}

AHA_TEST(SerializerTopicsTest, calculate_config_no_mqtt) {
    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            AHATOFSTR(ComponentNameStr),
            "objectId"
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_config_invalid_component) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);

    // it should return 0 if componentName is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            nullptr,
            "objectId"
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_config_invalid_object) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);

    // it should return 0 if objectId is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            AHATOFSTR(ComponentNameStr),
            nullptr
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_config_invalid_prefix) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDiscoveryPrefix(nullptr);

    // it should return 0 if discovery prefix is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            AHATOFSTR(ComponentNameStr),
            "objectId"
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_config) {
    const char* objectId = "objectId";
    const char* expectedTopic = "discoveryPrefix/componentName/testDevice/objectId/config";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDiscoveryPrefix(discoveryPrefix);

    // it should return valid length of the topic including 1 extra byte for null terminator
    assertEqual(
        (uint16_t)(strlen(expectedTopic) + 1),
        HASerializer::calculateConfigTopicLength(
            AHATOFSTR(ComponentNameStr),
            objectId
        )
    );
}

AHA_TEST(SerializerTopicsTest, generate_config_no_mqtt) {
    clearTmpBuffer();

    // it should return false if there is no HAMqtt instance (singleton)
    assertFalse(HASerializer::generateConfigTopic(
        tmpBuffer,
        AHATOFSTR(ComponentNameStr),
        "objectId"
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_config_invalid_component) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    clearTmpBuffer();

    // it should return false if componentName is null
    assertFalse(HASerializer::generateConfigTopic(
        tmpBuffer,
        nullptr,
        "objectId"
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_config_invalid_object) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    clearTmpBuffer();

    // it should return false if objectId is null
    assertFalse(HASerializer::generateConfigTopic(
        tmpBuffer,
        AHATOFSTR(ComponentNameStr),
        nullptr
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_config_invalid_prefix) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDiscoveryPrefix(nullptr);
    clearTmpBuffer();

    // it should return false if discovery prefix is null
    assertFalse(HASerializer::generateConfigTopic(
        tmpBuffer,
        AHATOFSTR(ComponentNameStr),
        "objectId"
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_config) {
    const char* objectId = "objectId";
    const char* expectedTopic = "discoveryPrefix/componentName/testDevice/objectId/config";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDiscoveryPrefix(discoveryPrefix);
    clearTmpBuffer();

    // it should generate valid topic
    assertTrue(HASerializer::generateConfigTopic(
        tmpBuffer,
        AHATOFSTR(ComponentNameStr),
        objectId
    ));
    assertEqual(expectedTopic, tmpBuffer);
}

AHA_TEST(SerializerTopicsTest, calculate_data_no_mqtt) {
    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            AHATOFSTR(DummyProgmemStr)
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_data_invalid_topic) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);

    // it should return 0 if topicP is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            nullptr
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_data_invalid_prefix) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(nullptr);

    // it should return 0 if data prefix is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            AHATOFSTR(DummyProgmemStr)
        )
    );
}

AHA_TEST(SerializerTopicsTest, calculate_data_partial) {
    const char* objectId = nullptr;
    const char* expectedTopic = "dataPrefix/testDevice/dummyProgmem";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);

    // it should return valid length of the topic including 1 extra byte for null terminator
    assertEqual(
        (uint16_t)strlen(expectedTopic) + 1,
        HASerializer::calculateDataTopicLength(
            objectId,
            AHATOFSTR(DummyProgmemStr)
        )
    );
}

AHA_TEST(SerializerTopicsTest, generate_data_no_mqtt) {
    clearTmpBuffer();

    // it should return false if there is no HAMqtt instance (singleton)
    assertFalse(HASerializer::generateDataTopic(
        tmpBuffer,
        "objectId",
        AHATOFSTR(DummyProgmemStr)
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_data_invalid_topic) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    clearTmpBuffer();

    // it should return false if topicP is null
    assertFalse(HASerializer::generateDataTopic(
        tmpBuffer,
        "objectId",
        nullptr
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_data_invalid_prefix) {
    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(nullptr);
    clearTmpBuffer();

    // it should return false if data prefix is null
    assertFalse(HASerializer::generateDataTopic(
        tmpBuffer,
        "objectId",
        AHATOFSTR(DummyProgmemStr)
    ));
    assertTrue(strlen(tmpBuffer) == 0);
}

AHA_TEST(SerializerTopicsTest, generate_data_partial) {
    const char* objectId = nullptr;
    const char* expectedTopic = "dataPrefix/testDevice/dummyProgmem";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);
    clearTmpBuffer();

    // it should generate valid partial data topic (without objectId)
    assertTrue(HASerializer::generateDataTopic(
        tmpBuffer,
        objectId,
        AHATOFSTR(DummyProgmemStr)
    ));
    assertEqual(tmpBuffer, expectedTopic);
}

AHA_TEST(SerializerTopicsTest, generate_data_full) {
    const char* objectId = "objectId";
    const char* expectedTopic = "dataPrefix/testDevice/objectId/dummyProgmem";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);
    clearTmpBuffer();

    // it should generate valid full data topic
    assertTrue(HASerializer::generateDataTopic(
        tmpBuffer,
        objectId,
        AHATOFSTR(DummyProgmemStr)
    ));
    assertEqual(tmpBuffer, expectedTopic);
}

AHA_TEST(SerializerTopicsTest, compare_invalid_topic) {
    const char* topic = nullptr;
    const char* objectId = "objectId";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);

    assertFalse(HASerializer::compareDataTopics(
        topic,
        objectId,
        AHATOFSTR(DummyProgmemStr)
    ));
}

AHA_TEST(SerializerTopicsTest, compare_matching_topics) {
    const char* topic = "dataPrefix/testDevice/objectId/dummyProgmem";
    const char* objectId = "objectId";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);

    assertTrue(HASerializer::compareDataTopics(
        topic,
        objectId,
        AHATOFSTR(DummyProgmemStr)
    ));
}

AHA_TEST(SerializerTopicsTest, compare_not_matching_topics) {
    const char* topic = "dataPrefix/testDevice/objectId/Progmem";
    const char* objectId = "objectId";

    HADevice device(deviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(dataPrefix);

    assertFalse(HASerializer::compareDataTopics(
        topic,
        objectId,
        AHATOFSTR(DummyProgmemStr)
    ));
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
