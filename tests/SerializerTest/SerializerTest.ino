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

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testTopic = "testTopic";
const char TestComponentStr[] PROGMEM = {"dummyProgmem"};

class DummyDeviceType : public HABaseDeviceType
{
public:
    DummyDeviceType(): HABaseDeviceType(AHATOFSTR(TestComponentStr), "testId") { }

protected:
    virtual void onMqttConnected() override { }
};

AHA_TEST(SerializerTest, empty_json) {
    prepareTest(0)
    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{}")
}

AHA_TEST(SerializerTest, skip_null_fields) {
    prepareTest(2)

    serializer.set(AHATOFSTR(HADeviceClassProperty), "Class");
    serializer.set(AHATOFSTR(HAIconProperty), nullptr);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":\"Class\"}")
}

AHA_TEST(SerializerTest, char_field) {
    prepareTest(1)

    serializer.set(AHATOFSTR(HANameProperty), "XYZ");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"XYZ\"}")
}

AHA_TEST(SerializerTest, bool_false_field) {
    prepareTest(1)

    bool value = false;
    serializer.set(
        AHATOFSTR(HANameProperty),
        &value,
        HASerializer::BoolPropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":false}")
}

AHA_TEST(SerializerTest, bool_true_field) {
    prepareTest(1)

    bool value = true;
    serializer.set(
        AHATOFSTR(HANameProperty),
        &value,
        HASerializer::BoolPropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":true}")
}

AHA_TEST(SerializerTest, number_zero_field) {
    prepareTest(1)

    int32_t value = 0;
    serializer.set(
        AHATOFSTR(HANameProperty),
        &value,
        HASerializer::Int32PropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":0}")
}

AHA_TEST(SerializerTest, number_signed_field) {
    prepareTest(1)

    int32_t value = -12346756;
    serializer.set(
        AHATOFSTR(HANameProperty),
        &value,
        HASerializer::Int32PropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":-12346756}")
}

AHA_TEST(SerializerTest, number_unsigned_field) {
    prepareTest(1)

    int32_t value = 312346733;
    serializer.set(
        AHATOFSTR(HANameProperty),
        &value,
        HASerializer::Int32PropertyType
    );

    flushSerializer(mock, serializer);
    assertSerializerMqttMessage("{\"name\":312346733}");
}

AHA_TEST(SerializerTest, progmem_field) {
    prepareTest(1)

    serializer.set(
        AHATOFSTR(HANameProperty),
        HAOffline,
        HASerializer::ProgmemPropertyValue
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"name\":\"offline\"}")
}

AHA_TEST(SerializerTest, topic_field) {
    prepareTest(1)

    serializer.topic(AHATOFSTR(HAStateTopic));

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\"}")
}

AHA_TEST(SerializerTest, topics_field) {
    prepareTest(2)

    serializer.topic(AHATOFSTR(HAStateTopic));
    serializer.topic(AHATOFSTR(HAAvailabilityTopic));

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"stat_t\":\"testData/testDevice/testId/stat_t\",\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

AHA_TEST(SerializerTest, device_serialization) {
    prepareTest(1)

    serializer.set(HASerializer::WithDevice);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"}}")
}

AHA_TEST(SerializerTest, device_mixed_serialization) {
    prepareTest(2)

    serializer.set(HASerializer::WithDevice);
    serializer.set(AHATOFSTR(HADeviceClassProperty), "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev\":{\"ids\":\"testDevice\"},\"dev_cla\":\"Class1\"}")
}

AHA_TEST(SerializerTest, device_type_availability) {
    prepareTest(1)

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\"}")
}

AHA_TEST(SerializerTest, device_type_availability_mixed) {
    prepareTest(2)

    dummyDeviceType.setAvailability(false);
    serializer.set(HASerializer::WithAvailability);
    serializer.set(AHATOFSTR(HADeviceClassProperty), "Class1");

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/testId/avty_t\",\"dev_cla\":\"Class1\"}")
}

AHA_TEST(SerializerTest, shared_availability) {
    prepareTest(1)

    device.enableSharedAvailability();
    serializer.set(HASerializer::WithAvailability);

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"avty_t\":\"testData/testDevice/avty_t\"}")
}

AHA_TEST(SerializerTest, empty_array) {
    prepareTest(1)

    HASerializerArray array(0);
    serializer.set(
        AHATOFSTR(HADeviceClassProperty),
        &array,
        HASerializer::ArrayPropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[]}")
}

AHA_TEST(SerializerTest, two_element_array) {
    prepareTest(1)

    HASerializerArray array(2);
    array.add(HADeviceProperty);
    array.add(HAIconProperty);
    serializer.set(
        AHATOFSTR(HADeviceClassProperty),
        &array,
        HASerializer::ArrayPropertyType
    );

    flushSerializer(mock, serializer)
    assertSerializerMqttMessage("{\"dev_cla\":[\"dev\",\"ic\"]}")
}

AHA_TEST(SerializerTest, mixed_elements) {
    prepareTest(6)

    HASerializerArray array(2);
    array.add(HADeviceProperty);
    array.add(HAIconProperty);

    dummyDeviceType.setAvailability(false);
    serializer.set(
        AHATOFSTR(HADeviceClassProperty),
        &array,
        HASerializer::ArrayPropertyType
    );
    serializer.set(HASerializer::WithAvailability);
    serializer.set(HASerializer::WithDevice);
    serializer.set(AHATOFSTR(HANameProperty), "TestName");
    serializer.topic(AHATOFSTR(HAStateTopic));

    int32_t intValue = 312346733;
    serializer.set(
        AHATOFSTR(HAIconProperty),
        &intValue,
        HASerializer::Int32PropertyType
    );

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
