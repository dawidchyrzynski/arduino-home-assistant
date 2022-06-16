#include <AUnit.h>
#include <ArduinoHA.h>

#define floatToStrAssert(value, precision, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    HAUtils::floatToStr(tmpBuffer, value, precision); \
    assertStringCaseEqual(F(expectedStr), tmpBuffer); \
}

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

test(UtilsTest, calculate_float_size_zero_p0) {
    // expects "0"
    assertEqual(1, HAUtils::calculateFloatSize(0, 0));
}

test(UtilsTest, calculate_float_size_zero_p1) {
    // expects "0.0"
    assertEqual(3, HAUtils::calculateFloatSize(0, 1));
}

test(UtilsTest, calculate_float_size_zero_p2) {
    // expects "0.00"
    assertEqual(4, HAUtils::calculateFloatSize(0, 2));
}

test(UtilsTest, calculate_float_size_zero_signed_p0) {
    // expects "0"
    assertEqual(1, HAUtils::calculateFloatSize(-0.323, 0));
}

test(UtilsTest, calculate_float_size_zero_signed_p1) {
    // expects "-0.3"
    assertEqual(4, HAUtils::calculateFloatSize(-0.323, 1));
}

test(UtilsTest, calculate_float_size_zero_signed_p2) {
    // expects "-0.32"
    assertEqual(5, HAUtils::calculateFloatSize(-0.323, 2));
}

test(UtilsTest, calculate_float_size_large_signed_p0) {
    // expects "-156"
    assertEqual(4, HAUtils::calculateFloatSize(-156.12312, 0));
}

test(UtilsTest, calculate_float_size_large_signed_p1) {
    // expects "-23445.9"
    assertEqual(8, HAUtils::calculateFloatSize(-23445.90999, 1));
}

test(UtilsTest, calculate_float_size_large_signed_p2) {
    // expects "-16345.33"
    assertEqual(9, HAUtils::calculateFloatSize(-16345.333, 2));
}

test(UtilsTest, calculate_float_size_zero_unsigned_p0) {
    // expects "0"
    assertEqual(1, HAUtils::calculateFloatSize(0.323, 0));
}

test(UtilsTest, calculate_float_size_zero_unsigned_p1) {
    // expects "0.3"
    assertEqual(3, HAUtils::calculateFloatSize(0.323, 1));
}

test(UtilsTest, calculate_float_size_zero_unsigned_p2) {
    // expects "0.32"
    assertEqual(4, HAUtils::calculateFloatSize(0.323, 2));
}

test(UtilsTest, calculate_float_size_large_unsigned_p0) {
    // expects "156"
    assertEqual(3, HAUtils::calculateFloatSize(156.12312, 0));
}

test(UtilsTest, calculate_float_size_large_unsigned_p1) {
    // expects "23445.9"
    assertEqual(7, HAUtils::calculateFloatSize(23445.90999, 1));
}

test(UtilsTest, calculate_float_size_large_unsigned_p2) {
    // expects "16345.33"
    assertEqual(8, HAUtils::calculateFloatSize(16345.333, 2));
}

test(UtilsTest, calculate_float_p2_leading_zero) {
    // expects "165.03"
    assertEqual(6, HAUtils::calculateFloatSize(165.0333, 2));
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

test(UtilsTest, float_to_str_) {
    // expects "864564"
    assertEqual(6, HAUtils::calculateNumberSize(864564));
}

test(UtilsTest, float_to_str_zero_p0) {
    floatToStrAssert(0, 0, "0");
}

test(UtilsTest, float_to_str_zero_p1) {
    floatToStrAssert(0, 1, "0.0");
}

test(UtilsTest, float_to_str_zero_p2) {
    floatToStrAssert(0, 2, "0.00");
}

test(UtilsTest, float_to_str_zero_signed_p0) {
    floatToStrAssert(-0.323, 0, "0");
}

test(UtilsTest, float_to_str_zero_signed_p1) {
    floatToStrAssert(-0.323, 1, "-0.3");
}

test(UtilsTest, float_to_str_zero_signed_p2) {
    floatToStrAssert(-0.323, 2, "-0.32");
}

test(UtilsTest, float_to_str_large_signed_p0) {
    floatToStrAssert(-156.12312, 0, "-156");
}

test(UtilsTest, float_to_str_large_signed_p1) {
    floatToStrAssert(-23445.90999, 1, "-23445.9");
}

test(UtilsTest, float_to_str_large_signed_p2) {
    floatToStrAssert(-16345.333, 2, "-16345.33");
}

test(UtilsTest, float_to_str_zero_unsigned_p0) {
    floatToStrAssert(0.323, 0, "0");
}

test(UtilsTest, float_to_str_zero_unsigned_p1) {
    floatToStrAssert(0.323, 1, "0.3");
}

test(UtilsTest, float_to_str_zero_unsigned_p2) {
    floatToStrAssert(0.323, 2, "0.32");
}

test(UtilsTest, float_to_str_large_unsigned_p0) {
    floatToStrAssert(156.12312, 0, "156");
}

test(UtilsTest, float_to_str_large_unsigned_p1) {
    floatToStrAssert(23445.90999, 1, "23445.9");
}

test(UtilsTest, float_to_str_large_unsigned_p2) {
    floatToStrAssert(16345.333, 2, "16345.33");
}

test(UtilsTest, float_to_str_large_unsigned_p3) {
    floatToStrAssert(16345.333, 3, "16345.333");
}

test(UtilsTest, float_to_str_unsigned_p2_leading_zero) {
    floatToStrAssert(163.0333, 2, "163.03");
}

test(UtilsTest, float_to_str_unsigned_p1_leading_zero) {
    floatToStrAssert(163.0333, 1, "163.0");
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
