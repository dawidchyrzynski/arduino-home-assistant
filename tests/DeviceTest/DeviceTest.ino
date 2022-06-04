#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static byte testDeviceByteId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
static const char* testDeviceByteIdChar = "11223344aabb";
static const char* mqttDataPrefix = "dataPrefix";
static const char* availabilityTopic = "dataPrefix/testDevice/avty_t";

#define assertSerializerEntry(entry, eType, eSubtype, eProperty, eValue) \
    assertEqual(eType, entry->type); \
    assertEqual(eSubtype, entry->subtype); \
    assertEqual(eProperty, entry->property); \
    assertEqual(eValue, entry->value);

test(DeviceTest, default_unique_id) {
    HADevice device;
    assertEqual((const char*)nullptr, device.getUniqueId());
}

test(DeviceTest, unique_id_constructor_char) {
    HADevice device(testDeviceId);
    assertStringCaseEqual(testDeviceId, device.getUniqueId());
}

test(DeviceTest, unique_id_constructor_byte_array) {
    HADevice device(testDeviceByteId, sizeof(testDeviceByteId));
    assertStringCaseEqual(testDeviceByteIdChar, device.getUniqueId());
}

test(DeviceTest, unique_id_setter) {
    HADevice device;
    assertEqual((const char*)nullptr, device.getUniqueId());

    bool result = device.setUniqueId(testDeviceByteId, sizeof(testDeviceByteId));
    assertTrue(result);
    assertStringCaseEqual(testDeviceByteIdChar, device.getUniqueId());
}

test(DeviceTest, unique_id_setter_runtime) {
    HADevice device(testDeviceId);
    assertStringCaseEqual(testDeviceId, device.getUniqueId());

    // unique ID cannot be changed if it's already set
    bool result = device.setUniqueId(testDeviceByteId, sizeof(testDeviceByteId));
    assertFalse(result);
    assertStringCaseEqual(testDeviceId, device.getUniqueId());
}

test(DeviceTest, serializer_no_unique_id) {
    HADevice device;
    const HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)0, serializer->getEntriesNb());
}

test(DeviceTest, serializer_unique_id_contructor_char) {
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

test(DeviceTest, serializer_unique_id_contructor_byte_array) {
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

test(DeviceTest, serializer_unique_id_setter) {
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

test(DeviceTest, serializer_manufacturer) {
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

test(DeviceTest, serializer_model) {
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

test(DeviceTest, serializer_name) {
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

test(DeviceTest, serializer_software_version) {
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

test(DeviceTest, default_availability) {
    HADevice device(testDeviceId);

    assertTrue(device.isAvailable());
    assertFalse(device.isSharedAvailabilityEnabled());
    assertEqual((const char*)nullptr, device.getAvailabilityTopic());
}

test(DeviceTest, enable_availability) {
    HADevice device(testDeviceId);
    HAMqtt mqtt(nullptr, device);
    mqtt.setDataPrefix(mqttDataPrefix);
    bool result = device.enableSharedAvailability();

    assertTrue(result);
    assertTrue(device.isSharedAvailabilityEnabled());
    assertStringCaseEqual(availabilityTopic, device.getAvailabilityTopic());
}

test(DeviceTest, enable_availability_no_unique_id) {
    HADevice device;
    bool result = device.enableSharedAvailability();

    assertFalse(result);
    assertFalse(device.isSharedAvailabilityEnabled());
    assertEqual((const char*)nullptr, device.getAvailabilityTopic());
}

test(DeviceTest, availability_publish_offline) {
    PubSubClientMock* mock = new PubSubClientMock();
    HADevice device(testDeviceId);
    HAMqtt mqtt(mock, device);
    mqtt.setDataPrefix(mqttDataPrefix);

    device.enableSharedAvailability();
    device.setAvailability(false);
    mock->connectDummy();

    assertStringCaseEqual(mock->getMessageTopic(), availabilityTopic);
    assertStringCaseEqual(mock->getMessageBuffer(), F("offline"));
    assertTrue(mock->isMessageFlushed());
}

test(DeviceTest, availability_publish_online) {
    PubSubClientMock* mock = new PubSubClientMock();
    HADevice device(testDeviceId);
    HAMqtt mqtt(mock, device);
    mqtt.setDataPrefix(mqttDataPrefix);

    device.enableSharedAvailability();
    device.setAvailability(true);
    mock->connectDummy();

    assertStringCaseEqual(mock->getMessageTopic(), availabilityTopic);
    assertStringCaseEqual(mock->getMessageBuffer(), F("online"));
    assertTrue(mock->isMessageFlushed());
}

// to do: last will test
// to do: serializer json data

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
