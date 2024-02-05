#include <AUnit.h>
#include <ArduinoHA.h>

#define assertJson(expectedJson, array) \
    const __FlashStringHelper* expectedJsonP = F(expectedJson); \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    assertTrue(array.serialize(tmpBuffer)); \
    assertEqual(tmpBuffer, expectedJsonP); \
    assertEqual((uint16_t)strlen_P(reinterpret_cast<const char *>(expectedJsonP)), array.calculateSize());

using aunit::TestRunner;

char tmpBuffer[32];

AHA_TEST(SerializerArrayTest, empty_array) {
    HASerializerArray array(0);

    assertEqual(0, array.getItemsNb());
    assertJson("[]", array);
}

AHA_TEST(SerializerArrayTest, single_element_progmem) {
    HASerializerArray array(1);
    bool result = array.add(HANameProperty);

    assertTrue(result);
    assertEqual(1, array.getItemsNb());
    assertEqual((uintptr_t)HANameProperty, (uintptr_t)&(*array.getItems()[0]));
    assertJson("[\"name\"]", array);
}

AHA_TEST(SerializerArrayTest, multiple_elements_progmem) {
    HASerializerArray array(3);

    assertTrue(array.add(HANameProperty));
    assertTrue(array.add(HADeviceManufacturerProperty));
    assertTrue(array.add(HAUniqueIdProperty));
    assertEqual(3, array.getItemsNb());

    HASerializerArray::ItemType* items = array.getItems();
    assertEqual((uintptr_t)HANameProperty, (uintptr_t)&(*items[0]));
    assertEqual((uintptr_t)HADeviceManufacturerProperty, (uintptr_t)&(*items[1]));
    assertEqual((uintptr_t)HAUniqueIdProperty, (uintptr_t)&(*items[2]));
    assertJson("[\"name\",\"mf\",\"uniq_id\"]", array);
}

AHA_TEST(SerializerArrayTest, size_overflow_progmem) {
    HASerializerArray array(1);

    assertTrue(array.add(HANameProperty));
    assertFalse(array.add(HAUniqueIdProperty));
    assertEqual(1, array.getItemsNb());

    assertEqual((uintptr_t)HANameProperty, (uintptr_t)&(*array.getItems()[0]));
    assertJson("[\"name\"]", array);
}

AHA_TEST(SerializerArrayTest, single_element_ram) {
    const char* item = "test";
    HASerializerArray array(1, false);
    bool result = array.add(item);

    assertTrue(result);
    assertEqual(1, array.getItemsNb());

    assertEqual((uintptr_t)item, (uintptr_t)&(*array.getItem(0)));
    assertEqual((uintptr_t)item, (uintptr_t)&(*array.getItems()[0]));
    assertJson("[\"test\"]", array);
}

AHA_TEST(SerializerArrayTest, multiple_elements_ram) {
    const char* item0 = "item0";
    const char* item1 = "item1";
    const char* item2 = "item2";
    HASerializerArray array(3, false);

    assertTrue(array.add(item0));
    assertTrue(array.add(item1));
    assertTrue(array.add(item2));
    assertEqual(3, array.getItemsNb());

    HASerializerArray::ItemType* items = array.getItems();

    assertEqual((uintptr_t)nullptr, (uintptr_t)&(*array.getItem(-1)));
    assertEqual((uintptr_t)nullptr, (uintptr_t)&(*array.getItem(3)));

    assertEqual((uintptr_t)item0, (uintptr_t)&(*array.getItem(0)));
    assertEqual((uintptr_t)item0, (uintptr_t)&(*items[0]));

    assertEqual((uintptr_t)item1, (uintptr_t)&(*array.getItem(1)));
    assertEqual((uintptr_t)item1, (uintptr_t)&(*items[1]));

    assertEqual((uintptr_t)item2, (uintptr_t)&(*array.getItem(2)));
    assertEqual((uintptr_t)item2, (uintptr_t)&(*items[2]));
    assertJson("[\"item0\",\"item1\",\"item2\"]", array);
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
