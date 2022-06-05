#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId); \
    DummyDeviceType dummyDeviceType; \
    HASerializer serializer(&dummyDeviceType);

#define flushSerializer(mock, serializer) \
    mock->connectDummy(); \
    mock->beginPublish(testTopic, serializer.calculateSize(), false); \
    serializer.flush(); \
    mock->endPublish();

#define assertSerializerMqttMessage(expectedJson) \
    assertSingleMqttMessage(testTopic, expectedJson, false)

class DummyDeviceType : public BaseDeviceType
{
public:
    DummyDeviceType(): BaseDeviceType("testComponent", "testId") { }

protected:
    virtual void onMqttConnected() override { }
};

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testTopic = "testTopic";

test(SerializerTest, empty_json) {
    prepareTest
    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{}")
}

test(SerializerTest, field_override) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class1");
    serializer.set(HADeviceClassProperty, "Class2");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":\"Class2\"}")
}

test(SerializerTest, skip_null_fields) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, nullptr);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":\"Class\"}")
}

test(SerializerTest, char_field) {
    prepareTest

    serializer.set(HANameProperty, "XYZ");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"XYZ\"}")
}

test(SerializerTest, bool_false_field) {
    prepareTest

    bool value = false;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":false}")
}

test(SerializerTest, bool_true_field) {
    prepareTest

    bool value = true;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":true}")
}

test(SerializerTest, float_zero_field) {
    prepareTest

    float value = 0;
    serializer.set(HANameProperty, &value, HASerializer::FloatP1PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":0.0}")
}

test(SerializerTest, float_truncate_field) {
    prepareTest

    float value = 1.22222;
    serializer.set(HANameProperty, &value, HASerializer::FloatP1PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":1.2}")
}

test(SerializerTest, float_unsigned_field) {
    prepareTest

    float value = 1.15;
    serializer.set(HANameProperty, &value, HASerializer::FloatP2PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":1.15}")
}

test(SerializerTest, float_signed_field) {
    prepareTest

    float value = -11.333;
    serializer.set(HANameProperty, &value, HASerializer::FloatP2PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":-11.33}")
}

test(SerializerTest, number_zero_field) {
    prepareTest

    int32_t value = 0;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":0}")
}

test(SerializerTest, number_signed_field) {
    prepareTest

    int32_t value = -12346756;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":-12346756}")
}

test(SerializerTest, number_unsigned_field) {
    prepareTest

    int32_t value = 312346733;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{\"name\":312346733}");
}

test(SerializerTest, progmem_field) {
    prepareTest

    int32_t value = 0;
    serializer.set(HANameProperty, HAOffline, HASerializer::ProgmemPropertyValue);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"offline\"}")
}

test(SerializerTest, topic_field) {
    prepareTest

    serializer.topic(HAStateTopic);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\"}")
}

test(SerializerTest, topic_duplicate_field) {
    prepareTest

    serializer.topic(HAStateTopic);
    serializer.topic(HAStateTopic);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\"}")
}

test(SerializerTest, topics_field) {
    prepareTest

    serializer.topic(HAStateTopic);
    serializer.topic(HAAvailabilityTopic);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\",\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

test(SerializerTest, device_serialization) {
    prepareTest

    serializer.set(HASerializer::WithDevice);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"}}")
}

test(SerializerTest, device_mixed_serialization) {
    prepareTest

    serializer.set(HASerializer::WithDevice);
    serializer.set(HADeviceClassProperty, "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"},\"dev_cla\":\"Class1\"}")
}

test(SerializerTest, device_type_availability) {
    prepareTest

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

test(SerializerTest, device_type_availability_mixed) {
    prepareTest

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);
    serializer.set(HADeviceClassProperty, "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\",\"dev_cla\":\"Class1\"}")
}

test(SerializerTest, shared_availability) {
    prepareTest

    device.enableSharedAvailability();
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/avty_t\"}")
}

test(SerializerTest, empty_array) {
    prepareTest

    HASerializerArray array(0);
    serializer.set(HADeviceClassProperty, &array, HASerializer::ArrayPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[]}")
}

test(SerializerTest, two_element_array) {
    prepareTest

    HASerializerArray array(2);
    array.add(HADeviceProperty);
    array.add(HAIconProperty);
    serializer.set(HADeviceClassProperty, &array, HASerializer::ArrayPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[\"dev\",\"ic\"]}")
}

test(SerializerTest, mixed_elements) {
    prepareTest

    HASerializerArray array(2);
    array.add(HADeviceProperty);
    array.add(HAIconProperty);

    dummyDeviceType.setAvailability(false);
    serializer.set(HADeviceClassProperty, &array, HASerializer::ArrayPropertyType);
    serializer.set(HASerializer::WithAvailability);
    serializer.set(HASerializer::WithDevice);
    serializer.set(HANameProperty, "TestName");
    serializer.topic(HAStateTopic);

    int32_t intValue = 312346733;
    serializer.set(HAIconProperty, &intValue, HASerializer::Int32PropertyType);

    float floatValue = -11.333;
    serializer.set(HADeviceManufacturerProperty, &floatValue, HASerializer::FloatP2PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[\"dev\",\"ic\"],\"avty_t\":\"testData/testDevice/testId/avty_t\",\"dev\":{\"ids\":\"testDevice\"},\"name\":\"TestName\",\"stat_t\":\"testData/testDevice/testId/stat_t\",\"ic\":312346733,\"mf\":-11.33}")
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
