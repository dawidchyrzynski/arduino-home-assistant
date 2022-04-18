#include <AUnit.h>
#include <ArduinoHATests.h>

using aunit::TestRunner;

test(UtilsTest, ends_with_null_str) {
    assertFalse(HAUtils::endsWith(nullptr, "test"));
}

test(UtilsTest, ends_with_null_suffix) {
    assertFalse(HAUtils::endsWith("test", nullptr));
}

test(UtilsTest, ends_with_valid_1) {
    assertTrue(HAUtils::endsWith("test", "st"));
}

test(UtilsTest, ends_with_valid_2) {
    assertTrue(HAUtils::endsWith("test/abc", "/abc"));
}

test(UtilsTest, ends_with_valid_3) {
    assertTrue(HAUtils::endsWith("test123", "3"));
}

test(UtilsTest, ends_with_invalid_1) {
    assertFalse(HAUtils::endsWith("test", "ST"));
}

test(UtilsTest, ends_with_invalid_2) {
    assertFalse(HAUtils::endsWith("test", ""));
}

test(UtilsTest, ends_with_invalid_3) {
    assertFalse(HAUtils::endsWith("test123", "2"));
}

test(UtilsTest, ends_with_invalid_4) {
    assertFalse(HAUtils::endsWith("test", "testtest"));
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
