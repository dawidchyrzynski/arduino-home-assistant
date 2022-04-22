#include <AUnit.h>
#include <ArduinoHATests.h>

using aunit::TestRunner;

test(SerializerArrayTest, empty_array) {
    HASerializerArray array(0);
    assertEqual(0, array.getItemsNb());
}

test(SerializerArrayTest, single_element) {
    HASerializerArray array(1);
    bool result = array.add(HANameProperty);

    assertTrue(result);
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
}

test(SerializerArrayTest, multiple_element) {
    HASerializerArray array(3);

    assertTrue(array.add(HANameProperty));
    assertTrue(array.add(HADeviceManufacturerProperty));
    assertTrue(array.add(HAUniqueIdProperty));
    assertEqual(3, array.getItemsNb());

    const char* items = array.getItems();
    assertEqual(HANameProperty, items[0]);
    assertEqual(HADeviceManufacturerProperty, items[1]);
    assertEqual(HAUniqueIdProperty, items[2]);
}

test(SerializerArrayTest, size_overflow) {
    HASerializerArray array(1);

    assertTrue(array.add(HANameProperty));
    assertFalse(array.add(HAUniqueIdProperty));
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
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
