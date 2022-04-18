#include <AUnit.h>
#include <Ethernet.h>
#include <ArduinoHATests.h>

using aunit::TestRunner;

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
    HADevice device("testDevice");
    assertEqual("testDevice", device.getUniqueId());
}

test(DeviceTest, unique_id_constructor_byte_array) {
    byte uniqueId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
    HADevice device(uniqueId, sizeof(uniqueId));
    assertEqual("11223344aabb", device.getUniqueId());
}

test(DeviceTest, unique_id_setter) {
    byte uniqueId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
    HADevice device;
    assertEqual((const char*)nullptr, device.getUniqueId());

    bool result = device.setUniqueId(uniqueId, sizeof(uniqueId));
    assertTrue(result);
    assertEqual("11223344aabb", device.getUniqueId());
}

test(DeviceTest, unique_id_setter_runtime) {
    byte uniqueId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
    HADevice device("testDevice");
    assertEqual("testDevice", device.getUniqueId());

    // unique ID cannot be changed if it's already set
    bool result = device.setUniqueId(uniqueId, sizeof(uniqueId));
    assertFalse(result);
    assertEqual("testDevice", device.getUniqueId());
}

test(DeviceTest, serializer_no_unique_id) {
    HADevice device;
    HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)0, serializer->getEntriesNb());
}

test(DeviceTest, serializer_unique_id_contructor_char) {
    const char* uniqueId = "testDevice";
    HADevice device(uniqueId);
    HASerializer* serializer = device.getSerializer();

    assertEqual((uint8_t)1, serializer->getEntriesNb());
    assertTrue(serializer->getEntries() != nullptr);

    HASerializer::SerializerEntry* entries = serializer->getEntries();
    HASerializer::SerializerEntry* entry = &entries[0];

    assertSerializerEntry(
        entry,
        HASerializer::PropertyEntryType,
        HASerializer::ConstCharPropertyValue,
        HADeviceIdentifiersProperty,
        uniqueId
    )
}

test(DeviceTest, serializer_unique_id_contructor_byte_array) {
    byte uniqueId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
    HADevice device(uniqueId);
    HASerializer* serializer = device.getSerializer();

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
    byte uniqueId[] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb};
    HADevice device;
    device.setUniqueId(uniqueId, sizeof(uniqueId));
    HASerializer* serializer = device.getSerializer();

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
    HASerializer* serializer = device.getSerializer();

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
    HASerializer* serializer = device.getSerializer();

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
    HASerializer* serializer = device.getSerializer();

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
    HASerializer* serializer = device.getSerializer();

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
