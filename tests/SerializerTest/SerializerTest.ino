#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest(maxEntriesNb) \
    initMqttTest(testDeviceId); \
    DummyDeviceType dummyDeviceType; \
    HASerializer serializer(&dummyDeviceType, maxEntriesNb);

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
    prepareTest(0)
    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{}")
}

test(SerializerTest, skip_null_fields) {
    prepareTest(2)

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, nullptr);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":\"Class\"}")
}

test(SerializerTest, char_field) {
    prepareTest(1)

    serializer.set(HANameProperty, "XYZ");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"XYZ\"}")
}

test(SerializerTest, bool_false_field) {
    prepareTest(1)

    bool value = false;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":false}")
}

test(SerializerTest, bool_true_field) {
    prepareTest(1)

    bool value = true;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":true}")
}

test(SerializerTest, number_zero_field) {
    prepareTest(1)

    int32_t value = 0;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":0}")
}

test(SerializerTest, number_signed_field) {
    prepareTest(1)

    int32_t value = -12346756;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":-12346756}")
}

test(SerializerTest, number_unsigned_field) {
    prepareTest(1)

    int32_t value = 312346733;
    serializer.set(HANameProperty, &value, HASerializer::Int32PropertyType);

    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{\"name\":312346733}");
}

test(SerializerTest, progmem_field) {
    prepareTest(1)

    serializer.set(HANameProperty, HAOffline, HASerializer::ProgmemPropertyValue);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"offline\"}")
}

test(SerializerTest, topic_field) {
    prepareTest(1)

    serializer.topic(HAStateTopic);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\"}")
}

test(SerializerTest, topics_field) {
    prepareTest(2)

    serializer.topic(HAStateTopic);
    serializer.topic(HAAvailabilityTopic);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\",\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

test(SerializerTest, device_serialization) {
    prepareTest(1)

    serializer.set(HASerializer::WithDevice);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"}}")
}

test(SerializerTest, device_mixed_serialization) {
    prepareTest(2)

    serializer.set(HASerializer::WithDevice);
    serializer.set(HADeviceClassProperty, "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"},\"dev_cla\":\"Class1\"}")
}

test(SerializerTest, device_type_availability) {
    prepareTest(1)

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

test(SerializerTest, device_type_availability_mixed) {
    prepareTest(2)

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);
    serializer.set(HADeviceClassProperty, "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\",\"dev_cla\":\"Class1\"}")
}

test(SerializerTest, shared_availability) {
    prepareTest(1)

    device.enableSharedAvailability();
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/avty_t\"}")
}

test(SerializerTest, empty_array) {
    prepareTest(1)

    HASerializerArray array(0);
    serializer.set(HADeviceClassProperty, &array, HASerializer::ArrayPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[]}")
}

test(SerializerTest, two_element_array) {
    prepareTest(1)

    HASerializerArray array(2);
    array.add(HADeviceProperty);
    array.add(HAIconProperty);
    serializer.set(HADeviceClassProperty, &array, HASerializer::ArrayPropertyType);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[\"dev\",\"ic\"]}")
}

test(SerializerTest, mixed_elements) {
    prepareTest(7)

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

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[\"dev\",\"ic\"],\"avty_t\":\"testData/testDevice/testId/avty_t\",\"dev\":{\"ids\":\"testDevice\"},\"name\":\"TestName\",\"stat_t\":\"testData/testDevice/testId/stat_t\",\"ic\":312346733}")
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
