#include <AUnit.h>
#include <ArduinoHATests.h>

#define prepareTest \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    HASerializer serializer(nullptr);

#define flushSerializer(mock, serializer) \
    mock->connectDummy(); \
    mock->beginPublish(testTopic, 0, false); \
    serializer.flush(); \
    mock->endPublish();

#define assertJson(mock, serializer, expectedJson) \
{ \
    assertStringCaseEqual(mock->getMessageTopic(), testTopic); \
    assertStringCaseEqual(mock->getMessageBuffer(), expectedJson); \
    assertEqual(strlen_P(reinterpret_cast<const char *>(expectedJson)), serializer.calculateSize()); \
}

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testTopic = "testTopic";

test(SerializerTest, empty_json) {
    prepareTest
    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{}"));
}

test(SerializerTest, field_override) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class1");
    serializer.set(HADeviceClassProperty, "Class2");

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"dev_cla\":\"Class2\"}"));
}

test(SerializerTest, skip_null_fields) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, nullptr);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"dev_cla\":\"Class\"}"));
}

test(SerializerTest, char_field) {
    prepareTest

    serializer.set(HANameProperty, "XYZ");

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":\"XYZ\"}"));
}

test(SerializerTest, bool_false_field) {
    prepareTest

    bool value = false;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":false}"));
}

test(SerializerTest, bool_true_field) {
    prepareTest

    bool value = true;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":true}"));
}

test(SerializerTest, float_zero_field) {
    prepareTest

    float value = 0;
    serializer.set(HANameProperty, &value, HASerializer::FloatP1PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":0.0}"));
}

test(SerializerTest, float_truncate_field) {
    prepareTest

    float value = 1.22222;
    serializer.set(HANameProperty, &value, HASerializer::FloatP1PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":1.2}"));
}

test(SerializerTest, float_unsigned_field) {
    prepareTest

    float value = 1.15;
    serializer.set(HANameProperty, &value, HASerializer::FloatP2PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":1.15}"));
}

test(SerializerTest, float_signed_field) {
    prepareTest

    float value = -11.333;
    serializer.set(HANameProperty, &value, HASerializer::FloatP2PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":-11.33}"));
}

test(SerializerTest, number_zero_field) {
    prepareTest

    int32_t value = 0;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":0}"));
}

test(SerializerTest, number_signed_field) {
    prepareTest

    int32_t value = -12346756;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":-12346756}"));
}

test(SerializerTest, number_unsigned_field) {
    prepareTest

    int32_t value = 312346733;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, F("{\"name\":312346733}"));
}

// to do: number
// to do: array
// to do: topic
// to do: two topics
// to do: topics duplicate call
// to do: with device
// to do: with availability

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
