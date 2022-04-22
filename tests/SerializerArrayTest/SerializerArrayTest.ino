#include <AUnit.h>
#include <ArduinoHATests.h>

#define assertJson(expectedJson, array) \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    assertTrue(array.serialize(tmpBuffer)); \
    assertStringCaseEqual(tmpBuffer, expectedJson); \
    assertEqual(strlen(expectedJson), array.calculateSize());

using aunit::TestRunner;

char tmpBuffer[32];

test(SerializerArrayTest, empty_array) {
    HASerializerArray array(0);
    const char* expectedJson = "[]";

    assertEqual(0, array.getItemsNb());
    assertJson(expectedJson, array);
}

test(SerializerArrayTest, single_element) {
    HASerializerArray array(1);
    bool result = array.add(HANameProperty);
    const char* expectedJson = "[\"name\"]";

    assertTrue(result);
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
    assertJson(expectedJson, array);
}

test(SerializerArrayTest, multiple_element) {
    HASerializerArray array(3);
    const char* expectedJson = "[\"name\",\"mf\",\"uniq_id\"]";

    assertTrue(array.add(HANameProperty));
    assertTrue(array.add(HADeviceManufacturerProperty));
    assertTrue(array.add(HAUniqueIdProperty));
    assertEqual(3, array.getItemsNb());

    const char* items = array.getItems();
    assertEqual(HANameProperty, items[0]);
    assertEqual(HADeviceManufacturerProperty, items[1]);
    assertEqual(HAUniqueIdProperty, items[2]);
    assertJson(expectedJson, array);
}

test(SerializerArrayTest, size_overflow) {
    HASerializerArray array(1);
    const char* expectedJson = "[\"name\"]";

    assertTrue(array.add(HANameProperty));
    assertFalse(array.add(HAUniqueIdProperty));
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
    assertJson(expectedJson, array);
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
