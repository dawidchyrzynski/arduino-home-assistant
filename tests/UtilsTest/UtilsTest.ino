#include <AUnit.h>
#include <ArduinoHA.h>

#define numberToStrAssert(value, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    HAUtils::numberToStr(tmpBuffer, value); \
    assertStringCaseEqual(F(expectedStr), tmpBuffer); \
}

using aunit::TestRunner;

char tmpBuffer[32];

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

test(UtilsTest, calculate_number_zero) {
    // expects "0"
    assertEqual(1, HAUtils::calculateNumberSize(0));
}

test(UtilsTest, calculate_number_signed_small) {
    // expects "-8"
    assertEqual(2, HAUtils::calculateNumberSize(-8));
}

test(UtilsTest, calculate_number_unsigned_small) {
    // expects "8"
    assertEqual(1, HAUtils::calculateNumberSize(8));
}

test(UtilsTest, calculate_number_signed_large) {
    // expects "-864564"
    assertEqual(7, HAUtils::calculateNumberSize(-864564));
}

test(UtilsTest, calculate_number_unsigned_large) {
    // expects "864564"
    assertEqual(6, HAUtils::calculateNumberSize(864564));
}

test(UtilsTest, number_to_str_zero) {
    numberToStrAssert(0, "0");
}

test(UtilsTest, number_to_str_signed_small) {
    numberToStrAssert(-8, "-8");
}

test(UtilsTest, number_to_str_unsigned_small) {
    numberToStrAssert(8, "8");
}

test(UtilsTest, number_to_str_signed_large) {
    numberToStrAssert(-864564, "-864564");
}

test(UtilsTest, number_to_str_unsigned_large) {
    numberToStrAssert(864564, "864564");
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
