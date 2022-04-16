#include <AUnit.h>
#include <Ethernet.h>
#include <ArduinoHATests.h>

using aunit::TestRunner;

EthernetClient dummyClient;
const char DummyProgmemStr[] PROGMEM = {"dummyProgmem"};

test(SerializerTopicsTest, calculate_config_no_mqtt) {
    HAMqtt::clearInstance();

    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            "componentName",
            "objectId"
        )
    );
}

test(SerializerTopicsTest, calculate_config_invalid_component) {
    HAMqtt::clearInstance();
    HADevice device("testDevice");
    HAMqtt mqtt(dummyClient, device);

    // it should return 0 if componentName is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            nullptr,
            "objectId"
        )
    );
}

test(SerializerTopicsTest, calculate_config_invalid_object) {
    HAMqtt::clearInstance();
    HADevice device("testDevice");
    HAMqtt mqtt(dummyClient, device);

    // it should return 0 if objectId is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            "componentName",
            nullptr
        )
    );
}

test(SerializerTopicsTest, calculate_config_invalid_prefix) {
    HAMqtt::clearInstance();
    HADevice device("testDevice");
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDiscoveryPrefix(nullptr);

    // it should return 0 if discovery prefix is null
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateConfigTopicLength(
            "componentName",
            "objectId"
        )
    );
}

test(SerializerTopicsTest, calculate_config_without_null_terminator) {
    const char* deviceId = "testDevice";
    const char* discoveryPrefix = "discoveryPrefix";
    const char* componentName = "componentName";
    const char* objectId = "objectId";
    const char* expectedTopic = "discoveryPrefix/componentName/testDevice/objectId/config";

    HAMqtt::clearInstance();
    HADevice device(deviceId);
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDiscoveryPrefix(discoveryPrefix);

    // it should return valid length of the topic
    assertEqual(
        (uint16_t)strlen(expectedTopic),
        HASerializer::calculateConfigTopicLength(
            componentName,
            objectId,
            false
        )
    );
}

test(SerializerTopicsTest, calculate_config_with_null_terminator) {
    const char* deviceId = "testDevice";
    const char* discoveryPrefix = "discoveryPrefix";
    const char* componentName = "componentName";
    const char* objectId = "objectId";
    const char* expectedTopic = "discoveryPrefix/componentName/testDevice/objectId/config";

    HAMqtt::clearInstance();
    HADevice device(deviceId);
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDiscoveryPrefix(discoveryPrefix);

    // it should return valid length of the topic including 1 extra byte for null terminator
    assertEqual(
        (uint16_t)(strlen(expectedTopic) + 1),
        HASerializer::calculateConfigTopicLength(
            componentName,
            objectId,
            true
        )
    );
}

test(SerializerTopicsTest, calculate_data_no_mqtt) {
    HAMqtt::clearInstance();

    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            "topicP"
        )
    );
}

test(SerializerTopicsTest, calculate_data_invalid_topic) {
    HAMqtt::clearInstance();
    HADevice device("testDevice");
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDataPrefix("dataPrefix");

    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            nullptr
        )
    );
}

test(SerializerTopicsTest, calculate_data_invalid_prefix) {
    HAMqtt::clearInstance();
    HADevice device("testDevice");
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDataPrefix(nullptr);

    // it should return 0 if there is no HAMqtt instance (singleton)
    assertEqual(
        (uint16_t)0,
        HASerializer::calculateDataTopicLength(
            "objectId",
            "topicP"
        )
    );
}

test(SerializerTopicsTest, calculate_data_partial_without_null_terminator) {
    const char* deviceId = "testDevice";
    const char* dataPrefix = "dataPrefix";
    const char* objectId = nullptr;
    const char* expectedTopic = "dataPrefix/testDevice/dummyProgmem";

    HAMqtt::clearInstance();
    HADevice device(deviceId);
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDataPrefix(dataPrefix);

    // it should return valid length of the topic
    assertEqual(
        (uint16_t)strlen(expectedTopic),
        HASerializer::calculateDataTopicLength(
            objectId,
            DummyProgmemStr,
            false
        )
    );
}

test(SerializerTopicsTest, calculate_data_partial_with_null_terminator) {
    const char* deviceId = "testDevice";
    const char* dataPrefix = "dataPrefix";
    const char* objectId = nullptr;
    const char* expectedTopic = "dataPrefix/testDevice/dummyProgmem";

    HAMqtt::clearInstance();
    HADevice device(deviceId);
    HAMqtt mqtt(dummyClient, device);
    mqtt.setDataPrefix(dataPrefix);

    // it should return valid length of the topic including 1 extra byte for null terminator
    assertEqual(
        (uint16_t)strlen(expectedTopic) + 1,
        HASerializer::calculateDataTopicLength(
            objectId,
            DummyProgmemStr,
            true
        )
    );
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
