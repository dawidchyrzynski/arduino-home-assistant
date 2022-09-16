#include <AUnit.h>
#include <ArduinoHA.h>

#define numberToStrAssert(value, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    HAUtils::numberToStr(tmpBuffer, value); \
    assertEqual(F(expectedStr), tmpBuffer); \
}

#define calculateFloatWithPrecisionAssert(value, precision, expectedSize) \
{ \
    assertEqual( \
        expectedSize, \
        HAUtils::calculateNumberSize( \
            HAUtils::processFloatValue(value, precision), \
            precision \
        ) \
    ); \
}

#define floatToStrAssert(value, precision, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    HAUtils::numberToStr( \
        tmpBuffer, \
        HAUtils::processFloatValue(value, precision), \
        precision \
    ); \
    assertEqual(F(expectedStr), tmpBuffer); \
}

using aunit::TestRunner;

char tmpBuffer[32];

AHA_TEST(UtilsTest, ends_with_null_str) {
    assertFalse(HAUtils::endsWith(nullptr, "test"));
}

AHA_TEST(UtilsTest, ends_with_null_suffix) {
    assertFalse(HAUtils::endsWith("test", nullptr));
}

AHA_TEST(UtilsTest, ends_with_valid_1) {
    assertTrue(HAUtils::endsWith("test", "st"));
}

AHA_TEST(UtilsTest, ends_with_valid_2) {
    assertTrue(HAUtils::endsWith("test/abc", "/abc"));
}

AHA_TEST(UtilsTest, ends_with_valid_3) {
    assertTrue(HAUtils::endsWith("test123", "3"));
}

AHA_TEST(UtilsTest, ends_with_invalid_1) {
    assertFalse(HAUtils::endsWith("test", "ST"));
}

AHA_TEST(UtilsTest, ends_with_invalid_2) {
    assertFalse(HAUtils::endsWith("test", ""));
}

AHA_TEST(UtilsTest, ends_with_invalid_3) {
    assertFalse(HAUtils::endsWith("test123", "2"));
}

AHA_TEST(UtilsTest, ends_with_invalid_4) {
    assertFalse(HAUtils::endsWith("test", "testtest"));
}

AHA_TEST(UtilsTest, calculate_number_zero) {
    assertEqual(1, HAUtils::calculateNumberSize(0));
}

AHA_TEST(UtilsTest, calculate_number_signed_small) {
    assertEqual(2, HAUtils::calculateNumberSize(-8));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_small) {
    assertEqual(1, HAUtils::calculateNumberSize(8));
}

AHA_TEST(UtilsTest, calculate_number_signed_large) {
    assertEqual(7, HAUtils::calculateNumberSize(-864564));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_large) {
    assertEqual(6, HAUtils::calculateNumberSize(864564));
}

AHA_TEST(UtilsTest, calculate_float_p1_zero) {
    calculateFloatWithPrecisionAssert(
        0.0,
        HABaseDeviceType::PrecisionP1,
        1
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_zero_signed) {
    calculateFloatWithPrecisionAssert(
        -0.123,
        HABaseDeviceType::PrecisionP1,
        4
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_zero_unsigned) {
    calculateFloatWithPrecisionAssert(
        0.123,
        HABaseDeviceType::PrecisionP1,
        3
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_small) {
    calculateFloatWithPrecisionAssert(
        1.0,
        HABaseDeviceType::PrecisionP1,
        3
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_medium) {
    calculateFloatWithPrecisionAssert(
        50.5,
        HABaseDeviceType::PrecisionP1,
        4
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_large) {
    calculateFloatWithPrecisionAssert(
        5526.02,
        HABaseDeviceType::PrecisionP1,
        6
    )
}

AHA_TEST(UtilsTest, calculate_float_p1_unsigned) {
    calculateFloatWithPrecisionAssert(
        -5526.02,
        HABaseDeviceType::PrecisionP1,
        7
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_zero) {
    calculateFloatWithPrecisionAssert(
        0.0,
        HABaseDeviceType::PrecisionP2,
        1
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_zero_signed) {
    calculateFloatWithPrecisionAssert(
        -0.123,
        HABaseDeviceType::PrecisionP2,
        5
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_zero_with_padding) {
    calculateFloatWithPrecisionAssert(
        0.012,
        HABaseDeviceType::PrecisionP2,
        4
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_zero_unsigned) {
    calculateFloatWithPrecisionAssert(
        0.123,
        HABaseDeviceType::PrecisionP2,
        4
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_small) {
    calculateFloatWithPrecisionAssert(
        1.0,
        HABaseDeviceType::PrecisionP2,
        4
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_medium) {
    calculateFloatWithPrecisionAssert(
        50.5,
        HABaseDeviceType::PrecisionP2,
        5
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_large) {
    calculateFloatWithPrecisionAssert(
        5526.5,
        HABaseDeviceType::PrecisionP2,
        7
    )
}

AHA_TEST(UtilsTest, calculate_float_p2_unsigned) {
    calculateFloatWithPrecisionAssert(
        -5526.12,
        HABaseDeviceType::PrecisionP2,
        8
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_zero) {
    calculateFloatWithPrecisionAssert(
        0.0,
        HABaseDeviceType::PrecisionP3,
        1
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_zero_signed) {
    calculateFloatWithPrecisionAssert(
        -0.123,
        HABaseDeviceType::PrecisionP3,
        6
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_zero_with_padding) {
    calculateFloatWithPrecisionAssert(
        0.012,
        HABaseDeviceType::PrecisionP3,
        5
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_zero_unsigned) {
    calculateFloatWithPrecisionAssert(
        0.123,
        HABaseDeviceType::PrecisionP3,
        5
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_small) {
    calculateFloatWithPrecisionAssert(
        1.0,
        HABaseDeviceType::PrecisionP3,
        5
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_medium) {
    calculateFloatWithPrecisionAssert(
        50.5,
        HABaseDeviceType::PrecisionP3,
        6
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_large) {
    calculateFloatWithPrecisionAssert(
        5526.5,
        HABaseDeviceType::PrecisionP3,
        8
    )
}

AHA_TEST(UtilsTest, calculate_float_p3_unsigned) {
    calculateFloatWithPrecisionAssert(
        -5526.12,
        HABaseDeviceType::PrecisionP3,
        9
    )
}

AHA_TEST(UtilsTest, number_to_str_zero) {
    numberToStrAssert(0, "0");
}

AHA_TEST(UtilsTest, number_to_str_signed_small) {
    numberToStrAssert(-8, "-8");
}

AHA_TEST(UtilsTest, number_to_str_unsigned_small) {
    numberToStrAssert(8, "8");
}

AHA_TEST(UtilsTest, number_to_str_signed_large) {
    numberToStrAssert(-864564, "-864564");
}

AHA_TEST(UtilsTest, number_to_str_unsigned_large) {
    numberToStrAssert(864564, "864564");
}

AHA_TEST(UtilsTest, float_to_str_p0_zero) {
    floatToStrAssert(0.0, HABaseDeviceType::PrecisionP0, "0");
}

AHA_TEST(UtilsTest, float_to_str_p0_small) {
    floatToStrAssert(1.0, HABaseDeviceType::PrecisionP0, "1");
}

AHA_TEST(UtilsTest, float_to_str_p0_large) {
    floatToStrAssert(5526.5, HABaseDeviceType::PrecisionP0, "5526");
}

AHA_TEST(UtilsTest, float_to_str_p0_signed) {
    floatToStrAssert(-5526.12, HABaseDeviceType::PrecisionP0, "-5526");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero) {
    floatToStrAssert(0.0, HABaseDeviceType::PrecisionP1, "0");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero_decimal) {
    floatToStrAssert(0.123, HABaseDeviceType::PrecisionP1, "0.1");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero_decimal_signed) {
    floatToStrAssert(-0.123, HABaseDeviceType::PrecisionP1, "-0.1");
}

AHA_TEST(UtilsTest, float_to_str_p1_small) {
    floatToStrAssert(1.0, HABaseDeviceType::PrecisionP1, "1.0");
}

AHA_TEST(UtilsTest, float_to_str_p1_medium) {
    floatToStrAssert(50.5, HABaseDeviceType::PrecisionP1, "50.5");
}

AHA_TEST(UtilsTest, float_to_str_p1_large) {
    floatToStrAssert(5526.5, HABaseDeviceType::PrecisionP1, "5526.5");
}

AHA_TEST(UtilsTest, float_to_str_p1_signed) {
    floatToStrAssert(-5526.12, HABaseDeviceType::PrecisionP1, "-5526.1");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero) {
    floatToStrAssert(0.0, HABaseDeviceType::PrecisionP2, "0");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_decimal) {
    floatToStrAssert(0.123, HABaseDeviceType::PrecisionP2, "0.12");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_with_padding) {
    floatToStrAssert(0.01, HABaseDeviceType::PrecisionP2, "0.01");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_decimal_signed) {
    floatToStrAssert(-0.123, HABaseDeviceType::PrecisionP2, "-0.12");
}

AHA_TEST(UtilsTest, float_to_str_p2_small) {
    floatToStrAssert(1.0, HABaseDeviceType::PrecisionP2, "1.00");
}

AHA_TEST(UtilsTest, float_to_str_p2_medium) {
    floatToStrAssert(50.50, HABaseDeviceType::PrecisionP2, "50.50");
}

AHA_TEST(UtilsTest, float_to_str_p2_large) {
    floatToStrAssert(5526.5, HABaseDeviceType::PrecisionP2, "5526.50");
}

AHA_TEST(UtilsTest, float_to_str_p2_signed) {
    floatToStrAssert(-5526.12, HABaseDeviceType::PrecisionP2, "-5526.12");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero) {
    floatToStrAssert(0.0, HABaseDeviceType::PrecisionP3, "0");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_decimal) {
    floatToStrAssert(0.123, HABaseDeviceType::PrecisionP3, "0.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_with_padding) {
    floatToStrAssert(0.001, HABaseDeviceType::PrecisionP3, "0.001");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_decimal_signed) {
    floatToStrAssert(-0.123, HABaseDeviceType::PrecisionP3, "-0.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_small) {
    floatToStrAssert(1.123, HABaseDeviceType::PrecisionP3, "1.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_medium) {
    floatToStrAssert(50.555, HABaseDeviceType::PrecisionP3, "50.555");
}

AHA_TEST(UtilsTest, float_to_str_p3_large) {
    floatToStrAssert(5526.5, HABaseDeviceType::PrecisionP3, "5526.500");
}

AHA_TEST(UtilsTest, float_to_str_p3_signed) {
    floatToStrAssert(-5526.12456456, HABaseDeviceType::PrecisionP3, "-5526.124");
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
