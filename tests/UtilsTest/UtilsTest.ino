#include <AUnit.h>
#include <ArduinoHA.h>

#define assertNumberToStr(value, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    const uint16_t expectedLength = expectedStr ? strlen(expectedStr) : 0; \
    const uint16_t writtenLength = HAUtils::numberToStr(tmpBuffer, value); \
    assertEqual(F(expectedStr), tmpBuffer); \
    assertEqual(expectedLength, writtenLength); \
}

#define assertFloatToStr(value, precision, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    const uint16_t expectedLength = expectedStr ? strlen(expectedStr) : 0; \
    const uint16_t writtenLength = HAUtils::numberToStr( \
        tmpBuffer, \
        HAUtils::processFloatValue(value, precision), \
        precision \
    ); \
    assertEqual(F(expectedStr), tmpBuffer); \
    assertEqual(expectedLength, writtenLength); \
}

#define assertStrToNumber(expected, str) \
    assertEqual( \
        (HAUtils::Number)expected, \
        HAUtils::strToNumber( \
            reinterpret_cast<const uint8_t*>(str), \
            str ? strlen(str) : 0 \
        ) \
    );

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

AHA_TEST(UtilsTest, calculate_number_signed_one_digit) {
    assertEqual(2, HAUtils::calculateNumberSize(-8));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_one_digit) {
    assertEqual(1, HAUtils::calculateNumberSize(8));
}

AHA_TEST(UtilsTest, calculate_number_signed_two_digits) {
    assertEqual(3, HAUtils::calculateNumberSize(-81));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_two_digits) {
    assertEqual(2, HAUtils::calculateNumberSize(86));
}

AHA_TEST(UtilsTest, calculate_number_signed_three_digits) {
    assertEqual(4, HAUtils::calculateNumberSize(-100));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_three_digits) {
    assertEqual(3, HAUtils::calculateNumberSize(100));
}

AHA_TEST(UtilsTest, calculate_number_signed_four_digits) {
    assertEqual(5, HAUtils::calculateNumberSize(-1234));
}

AHA_TEST(UtilsTest, calculate_number_unsigned_four_digits) {
    assertEqual(4, HAUtils::calculateNumberSize(1234));
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
    assertNumberToStr(0, "0");
}

AHA_TEST(UtilsTest, number_to_str_signed_small) {
    assertNumberToStr(-8, "-8");
}

AHA_TEST(UtilsTest, number_to_str_unsigned_small) {
    assertNumberToStr(8, "8");
}

AHA_TEST(UtilsTest, number_to_str_signed_large) {
    assertNumberToStr(-864564, "-864564");
}

AHA_TEST(UtilsTest, number_to_str_unsigned_large) {
    assertNumberToStr(864564, "864564");
}

AHA_TEST(UtilsTest, float_to_str_p0_zero) {
    assertFloatToStr(0.0, HABaseDeviceType::PrecisionP0, "0");
}

AHA_TEST(UtilsTest, float_to_str_p0_small) {
    assertFloatToStr(1.0, HABaseDeviceType::PrecisionP0, "1");
}

AHA_TEST(UtilsTest, float_to_str_p0_large) {
    assertFloatToStr(5526.5, HABaseDeviceType::PrecisionP0, "5526");
}

AHA_TEST(UtilsTest, float_to_str_p0_signed) {
    assertFloatToStr(-5526.12, HABaseDeviceType::PrecisionP0, "-5526");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero) {
    assertFloatToStr(0.0, HABaseDeviceType::PrecisionP1, "0");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero_decimal) {
    assertFloatToStr(0.123, HABaseDeviceType::PrecisionP1, "0.1");
}

AHA_TEST(UtilsTest, float_to_str_p1_zero_decimal_signed) {
    assertFloatToStr(-0.123, HABaseDeviceType::PrecisionP1, "-0.1");
}

AHA_TEST(UtilsTest, float_to_str_p1_small) {
    assertFloatToStr(1.0, HABaseDeviceType::PrecisionP1, "1.0");
}

AHA_TEST(UtilsTest, float_to_str_p1_medium) {
    assertFloatToStr(50.5, HABaseDeviceType::PrecisionP1, "50.5");
}

AHA_TEST(UtilsTest, float_to_str_p1_large) {
    assertFloatToStr(5526.5, HABaseDeviceType::PrecisionP1, "5526.5");
}

AHA_TEST(UtilsTest, float_to_str_p1_signed) {
    assertFloatToStr(-5526.12, HABaseDeviceType::PrecisionP1, "-5526.1");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero) {
    assertFloatToStr(0.0, HABaseDeviceType::PrecisionP2, "0");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_decimal) {
    assertFloatToStr(0.123, HABaseDeviceType::PrecisionP2, "0.12");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_with_padding) {
    assertFloatToStr(0.01, HABaseDeviceType::PrecisionP2, "0.01");
}

AHA_TEST(UtilsTest, float_to_str_p2_zero_decimal_signed) {
    assertFloatToStr(-0.123, HABaseDeviceType::PrecisionP2, "-0.12");
}

AHA_TEST(UtilsTest, float_to_str_p2_small) {
    assertFloatToStr(1.0, HABaseDeviceType::PrecisionP2, "1.00");
}

AHA_TEST(UtilsTest, float_to_str_p2_medium) {
    assertFloatToStr(50.50, HABaseDeviceType::PrecisionP2, "50.50");
}

AHA_TEST(UtilsTest, float_to_str_p2_large) {
    assertFloatToStr(5526.5, HABaseDeviceType::PrecisionP2, "5526.50");
}

AHA_TEST(UtilsTest, float_to_str_p2_signed) {
    assertFloatToStr(-5526.12, HABaseDeviceType::PrecisionP2, "-5526.12");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero) {
    assertFloatToStr(0.0, HABaseDeviceType::PrecisionP3, "0");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_decimal) {
    assertFloatToStr(0.123, HABaseDeviceType::PrecisionP3, "0.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_with_padding) {
    assertFloatToStr(0.001, HABaseDeviceType::PrecisionP3, "0.001");
}

AHA_TEST(UtilsTest, float_to_str_p3_zero_decimal_signed) {
    assertFloatToStr(-0.123, HABaseDeviceType::PrecisionP3, "-0.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_small) {
    assertFloatToStr(1.123, HABaseDeviceType::PrecisionP3, "1.123");
}

AHA_TEST(UtilsTest, float_to_str_p3_medium) {
    assertFloatToStr(50.555, HABaseDeviceType::PrecisionP3, "50.555");
}

AHA_TEST(UtilsTest, float_to_str_p3_large) {
    assertFloatToStr(5526.5, HABaseDeviceType::PrecisionP3, "5526.500");
}

AHA_TEST(UtilsTest, float_to_str_p3_signed) {
    assertFloatToStr(-5526.12456456, HABaseDeviceType::PrecisionP3, "-5526.124");
}

AHA_TEST(UtilsTest, str_to_number_null) {
    const char* num = nullptr;
    assertStrToNumber(HAUtils::NumberMax, num);
}

AHA_TEST(UtilsTest, str_to_number_unsigned_overflow) {
    assertStrToNumber(HAUtils::NumberMax, "92233720368547758078");
}

AHA_TEST(UtilsTest, str_to_number_signed_overflow) {
    assertStrToNumber(HAUtils::NumberMax, "-92233720368547758078");
}

AHA_TEST(UtilsTest, str_to_number_max) {
    assertStrToNumber((int64_t)9223372036854775807, "9223372036854775807");
}

AHA_TEST(UtilsTest, str_to_number_min) {
    assertStrToNumber((int64_t)-9223372036854775807, "-9223372036854775807");
}

AHA_TEST(UtilsTest, str_to_number_zero) {
    assertStrToNumber(0, "0");
}

AHA_TEST(UtilsTest, str_to_number_signed_0) {
    assertStrToNumber(-1, "-1");
}

AHA_TEST(UtilsTest, str_to_number_signed_1) {
    assertStrToNumber(-12, "-12");
}

AHA_TEST(UtilsTest, str_to_number_signed_2) {
    assertStrToNumber(-123, "-123");
}

AHA_TEST(UtilsTest, str_to_number_signed_3) {
    assertStrToNumber(-1234, "-1234");
}

AHA_TEST(UtilsTest, str_to_number_signed_4) {
    assertStrToNumber(-1234, "-1234");
}

AHA_TEST(UtilsTest, str_to_number_signed_5) {
    assertStrToNumber(-12345, "-12345");
}

AHA_TEST(UtilsTest, str_to_number_signed_6) {
    assertStrToNumber(-123456, "-123456");
}

AHA_TEST(UtilsTest, str_to_number_signed_7) {
    assertStrToNumber(-1234567, "-1234567");
}

AHA_TEST(UtilsTest, str_to_number_signed_8) {
    assertStrToNumber(-12345678, "-12345678");
}

AHA_TEST(UtilsTest, str_to_number_signed_9) {
    assertStrToNumber(-123456789, "-123456789");
}

AHA_TEST(UtilsTest, str_to_number_signed_10) {
    assertStrToNumber(-1234567890, "-1234567890");
}

AHA_TEST(UtilsTest, str_to_number_signed_11) {
    assertStrToNumber(-12345678901, "-12345678901");
}

AHA_TEST(UtilsTest, str_to_number_signed_12) {
    assertStrToNumber(-123456789012, "-123456789012");
}

AHA_TEST(UtilsTest, str_to_number_signed_13) {
    assertStrToNumber(-1234567890123, "-1234567890123");
}

AHA_TEST(UtilsTest, str_to_number_signed_14) {
    assertStrToNumber(-123456789012345, "-123456789012345");
}

AHA_TEST(UtilsTest, str_to_number_signed_15) {
    assertStrToNumber(-1234567890123456, "-1234567890123456");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_0) {
    assertStrToNumber(1, "1");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_1) {
    assertStrToNumber(12, "12");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_2) {
    assertStrToNumber(123, "123");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_3) {
    assertStrToNumber(1234, "1234");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_4) {
    assertStrToNumber(1234, "1234");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_5) {
    assertStrToNumber(12345, "12345");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_6) { 
    assertStrToNumber(123456, "123456");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_7) {
    assertStrToNumber(1234567, "1234567");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_8) {
    assertStrToNumber(12345678, "12345678");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_9) {
    assertStrToNumber(123456789, "123456789");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_10) {
    assertStrToNumber(1234567890, "1234567890");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_11) {
    assertStrToNumber(12345678901, "12345678901");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_12) {
    assertStrToNumber(123456789012, "123456789012");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_13) {
    assertStrToNumber(1234567890123, "1234567890123");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_14) {
    assertStrToNumber(123456789012345, "123456789012345");
}

AHA_TEST(UtilsTest, str_to_number_unsigned_15) {
    assertStrToNumber(1234567890123456, "1234567890123456");
}

AHA_TEST(UtilsTest, str_to_number_invalid_0) {
    assertStrToNumber(HAUtils::NumberMax, "--12");
}

AHA_TEST(UtilsTest, str_to_number_invalid_1) {
    assertStrToNumber(HAUtils::NumberMax, "a1");
}

AHA_TEST(UtilsTest, str_to_number_invalid_2) {
    assertStrToNumber(HAUtils::NumberMax, "567a32");
}

AHA_TEST(UtilsTest, str_to_number_invalid_3) {
    assertStrToNumber(HAUtils::NumberMax, "15.334");
}

AHA_TEST(UtilsTest, number_to_float_1) {
    assertNear(HAUtils::getFloatValue(500, 0), 500.0, 0.01);
}

AHA_TEST(UtilsTest, number_to_float_2) {
    assertNear(HAUtils::getFloatValue(500, 1), 50.0, 0.01);
}

AHA_TEST(UtilsTest, number_to_float_3) {
    assertNear(HAUtils::getFloatValue(500, 2), 5.0, 0.01);
}

AHA_TEST(UtilsTest, number_to_float_4) {
    assertNear(HAUtils::getFloatValue(500, 3), 0.5, 0.01);
}

AHA_TEST(UtilsTest, number_to_float_5) {
    assertNear(HAUtils::getFloatValue(-265544, 3), -265.544, 0.0001);
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
