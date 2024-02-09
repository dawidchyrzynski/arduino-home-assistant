#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static byte testDeviceByteId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
static const char* testDeviceByteIdChar = "11223344aabb";
static const char* dummyTopic = "dummyTopic";

const char AvailabilityTopic[] PROGMEM = {"testData/testDevice/avty_t"};

#define prepareMqttTest \
    initMqttTest(testDeviceId) \
    mock->connectDummy();

#define assertSerializerEntry(entry, eType, eSubtype, eProperty, eValue) \
    assertEqual(eType, entry->type); \
    assertEqual(eSubtype, entry->subtype); \
    assertEqual(eProperty, entry->property); \
    assertEqual(eValue, entry->value);

#define flushSerializer(mock, serializer) \
    mock->connectDummy(); \
    mock->beginPublish(dummyTopic, serializer->calculateSize(), false); \
    serializer->flush(); \
    mock->endPublish();

#define assertSerializerMqttMessage(expectedJson) \
    assertSingleMqttMessage(dummyTopic, expectedJson, false)

AHA_TEST(DeviceTest, default_unique_id) {
    HADevice device;
    assertEqual((const char*)nullptr, device.getUniqueId());
}

AHA_TEST(DeviceTest, unique_id_constructor_char) {
    HADevice device(testDeviceId);
    assertEqual(testDeviceId, device.getUniqueId());
}

AHA_TEST(DeviceTest, unique_id_constructor_byte_array) {
    HADevice device(testDeviceByteId, sizeof(testDeviceByteId));
    assertEqual(testDeviceByteIdChar, device.getUniqueId());
}

AHA_TEST(DeviceTest, unique_id_setter) {
    HADevice device;
    assertEqual((const char*)nullptr, device.getUniqueId());

    bool result = device.setUniqueId(testDeviceByteId, sizeof(testDeviceByteId));
    assertTrue(result);
    assertEqual(testDeviceByteIdChar, device.getUniqueId());
}

AHA_TEST(DeviceTest, unique_id_setter_runtime) {
    HADevice device(testDeviceId);
    assertEqual(testDeviceId, device.getUniqueId());

    // unique ID cannot be changed if it's already set
    bool result = device.setUniqueId(testDeviceByteId, sizeof(testDeviceByteId));
    assertFalse(result);
    assertEqual(testDeviceId, device.getUniqueId());
}

AHA_TEST(DeviceTest, serializer_no_unique_id) {
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)0, serializer->getEntriesNb());
}

AHA_TEST(DeviceTest, serializer_unique_id_constructor_char) {
    HADevice device(testDeviceId);
    const HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceIdentifiersProperty,
        testDeviceId
    )
}

AHA_TEST(DeviceTest, serializer_unique_id_constructor_byte_array) {
    HADevice device(testDeviceId);
    const HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceIdentifiersProperty,
        device.getUniqueId()
    )
}

AHA_TEST(DeviceTest, serializer_unique_id_setter) {
    HADevice device;
    device.setUniqueId(testDeviceByteId, sizeof(testDeviceByteId));
    const HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceIdentifiersProperty,
        device.getUniqueId()
    )
}

AHA_TEST(DeviceTest, serializer_manufacturer) {
    const char* manufacturer = "testManufacturer";
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    device.setManufacturer(manufacturer);

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceManufacturerProperty,
        manufacturer
    )
}

AHA_TEST(DeviceTest, serializer_model) {
    const char* model = "testModel";
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    device.setModel(model);

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceModelProperty,
        model
    )
}

AHA_TEST(DeviceTest, serializer_name) {
    const char* name = "testName";
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    device.setName(name);

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HANameProperty,
        name
    )
}

AHA_TEST(DeviceTest, serializer_software_version) {
    const char* softwareVersion = "softwareVersion";
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    device.setSoftwareVersion(softwareVersion);

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceSoftwareVersionProperty,
        softwareVersion
    )
}

AHA_TEST(DeviceTest, default_availability) {
    HADevice device(testDeviceId);

    assertTrue(device.isAvailable());
    assertFalse(device.isSharedAvailabilityEnabled());
    assertEqual((const char*)nullptr, device.getAvailabilityTopic());
}

AHA_TEST(DeviceTest, enable_availability) {
    prepareMqttTest

    assertTrue(device.enableSharedAvailability());
    assertTrue(device.isSharedAvailabilityEnabled());
    assertEqual(AHATOFSTR(AvailabilityTopic), device.getAvailabilityTopic());
    assertNoMqttMessage()
}

AHA_TEST(DeviceTest, enable_availability_no_unique_id) {
    HADevice device;
    bool result = device.enableSharedAvailability();

    assertFalse(result);
    assertFalse(device.isSharedAvailabilityEnabled());
    assertEqual((const char*)nullptr, device.getAvailabilityTopic());
}

AHA_TEST(DeviceTest, availability_publish_offline) {
    prepareMqttTest

    device.enableSharedAvailability();
    device.setAvailability(false);

    assertSingleMqttMessage(AHATOFSTR(AvailabilityTopic), "offline", true)
}

AHA_TEST(DeviceTest, availability_publish_online) {
    prepareMqttTest

    device.enableSharedAvailability();
    device.setAvailability(true);

    assertSingleMqttMessage(AHATOFSTR(AvailabilityTopic), "online", true)
}

AHA_TEST(DeviceTest, extended_unique_ids_disabled) {
    prepareMqttTest

    assertFalse(device.isExtendedUniqueIdsEnabled());
}

AHA_TEST(DeviceTest, enable_extended_unique_ids) {
    prepareMqttTest

    device.enableExtendedUniqueIds();

    assertTrue(device.isExtendedUniqueIdsEnabled());
    assertNoMqttMessage()
}

AHA_TEST(DeviceTest, lwt_disabled) {
    prepareMqttTest

    assertEqual((const char*)nullptr, mock->getLastWill().topic);
    assertEqual((const char*)nullptr, mock->getLastWill().message);
}

AHA_TEST(DeviceTest, lwt_enabled) {
    initMqttTest(testDeviceId)

    device.enableSharedAvailability();
    device.enableLastWill();
    mqtt.loop();

    assertEqual(AHATOFSTR(AvailabilityTopic), mock->getLastWill().topic);
    assertEqual("offline", mock->getLastWill().message);
    assertTrue(mock->getLastWill().retain);
}

AHA_TEST(DeviceTest, full_serialization) {
    initMqttTest("myDeviceId");

    device.setManufacturer("myManufacturer");
    device.setModel("myModel");
    device.setName("myName");
    device.setSoftwareVersion("myVersion");
    device.setConfigurationUrl("http://1.1.1.1:1234");

    const HASerializer* serializer = device.getSerializer();
    flushSerializer(mock, serializer)
    assertSerializerMqttMessage(
        (
            "{"
            "\"ids\":\"myDeviceId\","
            "\"mf\":\"myManufacturer\","
            "\"mdl\":\"myModel\","
            "\"name\":\"myName\","
            "\"sw\":\"myVersion\","
            "\"cu\":\"http://1.1.1.1:1234\""
            "}"
        )
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
