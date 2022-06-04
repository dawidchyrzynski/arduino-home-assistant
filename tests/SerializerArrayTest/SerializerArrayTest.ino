#include <AUnit.h>
#include <ArduinoHA.h>

#define assertJson(expectedJson, array) \
    const __FlashStringHelper* expectedJsonP = F(expectedJson); \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    assertTrue(array.serialize(tmpBuffer)); \
    assertStringCaseEqual(tmpBuffer, expectedJsonP); \
    assertEqual((uint16_t)strlen_P(reinterpret_cast<const char *>(expectedJsonP)), array.calculateSize());

using aunit::TestRunner;

char tmpBuffer[32];

test(SerializerArrayTest, empty_array) {
    HASerializerArray array(0);

    assertEqual(0, array.getItemsNb());
    assertJson("[]", array);
}

test(SerializerArrayTest, single_element) {
    HASerializerArray array(1);
    bool result = array.add(HANameProperty);

    assertTrue(result);
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
    assertJson("[\"name\"]", array);
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
    assertJson("[\"name\",\"mf\",\"uniq_id\"]", array);
}

test(SerializerArrayTest, size_overflow) {
    HASerializerArray array(1);

    assertTrue(array.add(HANameProperty));
    assertFalse(array.add(HAUniqueIdProperty));
    assertEqual(1, array.getItemsNb());
    assertEqual(HANameProperty, array.getItems()[0]);
    assertJson("[\"name\"]", array);
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
