#include <AUnit.h>
#include <ArduinoHA.h>

#define assertNumberSize(value, precision, expectedSize) \
{ \
    HANumeric number(value, precision); \
    assertTrue(number.isSet()); \
    assertEqual(expectedSize, number.calculateSize()); \
}

#define assertNumberToStr(value, precision, expectedStr) \
{ \
    memset(tmpBuffer, 0, sizeof(tmpBuffer)); \
    HANumeric number(value, precision); \
    const uint16_t expectedLength = expectedStr ? strlen(expectedStr) : 0; \
    const uint16_t writtenLength = number.toStr(tmpBuffer); \
    assertEqual(F(expectedStr), tmpBuffer); \
    assertEqual(expectedLength, writtenLength); \
}

#define assertStrToNumber(expected, str) \
{ \
    HANumeric number = HANumeric::fromStr( \
        reinterpret_cast<const uint8_t*>(str), \
        str ? strlen(str) : 0 \
    ); \
    assertTrue(number.isSet()); \
    assertEqual((int64_t)expected, number.getBaseValue()); \
}

#define assertStrToNumberInvalid(str) \
{ \
    HANumeric number = HANumeric::fromStr( \
        reinterpret_cast<const uint8_t*>(str), \
        str ? strlen(str) : 0 \
    ); \
    assertFalse(number.isSet()); \
}

using aunit::TestRunner;

char tmpBuffer[32];

AHA_TEST(NumericTest, calculate_number_zero) {
    assertNumberSize(0, 0, 1)
}

AHA_TEST(NumericTest, calculate_number_signed_one_digit) {
    assertNumberSize(-8, 0, 2)
}

AHA_TEST(NumericTest, calculate_number_unsigned_one_digit) {
    assertNumberSize(8, 0, 1)
}

AHA_TEST(NumericTest, calculate_number_signed_two_digits) {
    assertNumberSize(-81, 0, 3)
}

AHA_TEST(NumericTest, calculate_number_unsigned_two_digits) {
    assertNumberSize(86, 0, 2)
}

AHA_TEST(NumericTest, calculate_number_signed_three_digits) {
    assertNumberSize(-100, 0, 4)
}

AHA_TEST(NumericTest, calculate_number_unsigned_three_digits) {
    assertNumberSize(100, 0, 3)
}

AHA_TEST(NumericTest, calculate_number_signed_four_digits) {
    assertNumberSize(-1234, 0, 5)
}

AHA_TEST(NumericTest, calculate_number_unsigned_four_digits) {
    assertNumberSize(1234, 0, 4)
}

AHA_TEST(NumericTest, calculate_float_p1_zero) {
    assertNumberSize(0.0f, HABaseDeviceType::PrecisionP1, 1)
}

AHA_TEST(NumericTest, calculate_float_p1_zero_signed) {
    assertNumberSize(-0.123f, HABaseDeviceType::PrecisionP1, 4)
}

AHA_TEST(NumericTest, calculate_float_p1_zero_unsigned) {
    assertNumberSize(0.123f, HABaseDeviceType::PrecisionP1, 3)
}

AHA_TEST(NumericTest, calculate_float_p1_small) {
    assertNumberSize(1.0f, HABaseDeviceType::PrecisionP1, 3)
}

AHA_TEST(NumericTest, calculate_float_p1_medium) {
    assertNumberSize(50.5f, HABaseDeviceType::PrecisionP1, 4)
}

AHA_TEST(NumericTest, calculate_float_p1_large) {
    assertNumberSize(5526.02f, HABaseDeviceType::PrecisionP1, 6)
}

AHA_TEST(NumericTest, calculate_float_p1_unsigned) {
    assertNumberSize(-5526.02f, HABaseDeviceType::PrecisionP1, 7)
}

AHA_TEST(NumericTest, calculate_float_p2_zero) {
    assertNumberSize(0.0f, HABaseDeviceType::PrecisionP2, 1)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_signed) {
    assertNumberSize(-0.123f, HABaseDeviceType::PrecisionP2, 5)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_with_padding) {
    assertNumberSize(0.012f, HABaseDeviceType::PrecisionP2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_unsigned) {
    assertNumberSize(0.123f, HABaseDeviceType::PrecisionP2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_small) {
    assertNumberSize(1.0f, HABaseDeviceType::PrecisionP2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_medium) {
    assertNumberSize(50.5f, HABaseDeviceType::PrecisionP2, 5)
}

AHA_TEST(NumericTest, calculate_float_p2_large) {
    assertNumberSize(5526.5f, HABaseDeviceType::PrecisionP2, 7)
}

AHA_TEST(NumericTest, calculate_float_p2_unsigned) {
    assertNumberSize(-5526.12f, HABaseDeviceType::PrecisionP2, 8)
}

AHA_TEST(NumericTest, calculate_float_p3_zero) {
    assertNumberSize(0.0f, HABaseDeviceType::PrecisionP3, 1)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_signed) {
    assertNumberSize(-0.123f, HABaseDeviceType::PrecisionP3, 6)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_with_padding) {
    assertNumberSize(0.012f, HABaseDeviceType::PrecisionP3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_unsigned) {
    assertNumberSize(0.123f, HABaseDeviceType::PrecisionP3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_small) {
    assertNumberSize(1.0f, HABaseDeviceType::PrecisionP3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_medium) {
    assertNumberSize(50.5f, HABaseDeviceType::PrecisionP3, 6)
}

AHA_TEST(NumericTest, calculate_float_p3_large) {
    assertNumberSize(5526.5f, HABaseDeviceType::PrecisionP3, 8)
}

AHA_TEST(NumericTest, calculate_float_p3_unsigned) {
    assertNumberSize(-5526.12f, HABaseDeviceType::PrecisionP3, 9)
}

AHA_TEST(NumericTest, number_to_str_zero) {
    assertNumberToStr(0, 0, "0");
}

AHA_TEST(NumericTest, number_to_str_signed_small) {
    assertNumberToStr(-8, 0, "-8");
}

AHA_TEST(NumericTest, number_to_str_unsigned_small) {
    assertNumberToStr(8, 0, "8");
}

AHA_TEST(NumericTest, number_to_str_signed_large) {
    assertNumberToStr(-864564, 0, "-864564");
}

AHA_TEST(NumericTest, number_to_str_unsigned_large) {
    assertNumberToStr(864564, 0, "864564");
}

AHA_TEST(NumericTest, float_to_str_p0_zero) {
    assertNumberToStr(0.0f, HABaseDeviceType::PrecisionP0, "0");
}

AHA_TEST(NumericTest, float_to_str_p0_small) {
    assertNumberToStr(1.0f, HABaseDeviceType::PrecisionP0, "1");
}

AHA_TEST(NumericTest, float_to_str_p0_large) {
    assertNumberToStr(5526.5f, HABaseDeviceType::PrecisionP0, "5526");
}

AHA_TEST(NumericTest, float_to_str_p0_signed) {
    assertNumberToStr(-5526.12f, HABaseDeviceType::PrecisionP0, "-5526");
}

AHA_TEST(NumericTest, float_to_str_p1_zero) {
    assertNumberToStr(0.0f, HABaseDeviceType::PrecisionP1, "0");
}

AHA_TEST(NumericTest, float_to_str_p1_zero_decimal) {
    assertNumberToStr(0.123f, HABaseDeviceType::PrecisionP1, "0.1");
}

AHA_TEST(NumericTest, float_to_str_p1_zero_decimal_signed) {
    assertNumberToStr(-0.123f, HABaseDeviceType::PrecisionP1, "-0.1");
}

AHA_TEST(NumericTest, float_to_str_p1_small) {
    assertNumberToStr(1.0f, HABaseDeviceType::PrecisionP1, "1.0");
}

AHA_TEST(NumericTest, float_to_str_p1_medium) {
    assertNumberToStr(50.5f, HABaseDeviceType::PrecisionP1, "50.5");
}

AHA_TEST(NumericTest, float_to_str_p1_large) {
    assertNumberToStr(5526.5f, HABaseDeviceType::PrecisionP1, "5526.5");
}

AHA_TEST(NumericTest, float_to_str_p1_signed) {
    assertNumberToStr(-5526.12f, HABaseDeviceType::PrecisionP1, "-5526.1");
}

AHA_TEST(NumericTest, float_to_str_p2_zero) {
    assertNumberToStr(0.0f, HABaseDeviceType::PrecisionP2, "0");
}

AHA_TEST(NumericTest, float_to_str_p2_zero_decimal) {
    assertNumberToStr(0.123f, HABaseDeviceType::PrecisionP2, "0.12");
}

AHA_TEST(NumericTest, float_to_str_p2_zero_with_padding) {
    assertNumberToStr(0.01f, HABaseDeviceType::PrecisionP2, "0.01");
}

AHA_TEST(NumericTest, float_to_str_p2_zero_decimal_signed) {
    assertNumberToStr(-0.123f, HABaseDeviceType::PrecisionP2, "-0.12");
}

AHA_TEST(NumericTest, float_to_str_p2_small) {
    assertNumberToStr(1.0f, HABaseDeviceType::PrecisionP2, "1.00");
}

AHA_TEST(NumericTest, float_to_str_p2_medium) {
    assertNumberToStr(50.50f, HABaseDeviceType::PrecisionP2, "50.50");
}

AHA_TEST(NumericTest, float_to_str_p2_large) {
    assertNumberToStr(5526.5f, HABaseDeviceType::PrecisionP2, "5526.50");
}

AHA_TEST(NumericTest, float_to_str_p2_signed) {
    assertNumberToStr(-5526.12f, HABaseDeviceType::PrecisionP2, "-5526.12");
}

AHA_TEST(NumericTest, float_to_str_p3_zero) {
    assertNumberToStr(0.0f, HABaseDeviceType::PrecisionP3, "0");
}

AHA_TEST(NumericTest, float_to_str_p3_zero_decimal) {
    assertNumberToStr(0.123f, HABaseDeviceType::PrecisionP3, "0.123");
}

AHA_TEST(NumericTest, float_to_str_p3_zero_with_padding) {
    assertNumberToStr(0.001f, HABaseDeviceType::PrecisionP3, "0.001");
}

AHA_TEST(NumericTest, float_to_str_p3_zero_decimal_signed) {
    assertNumberToStr(-0.123f, HABaseDeviceType::PrecisionP3, "-0.123");
}

AHA_TEST(NumericTest, float_to_str_p3_small) {
    assertNumberToStr(1.123f, HABaseDeviceType::PrecisionP3, "1.123");
}

AHA_TEST(NumericTest, float_to_str_p3_medium) {
    assertNumberToStr(50.555f, HABaseDeviceType::PrecisionP3, "50.555");
}

AHA_TEST(NumericTest, float_to_str_p3_large) {
    assertNumberToStr(5526.5f, HABaseDeviceType::PrecisionP3, "5526.500");
}

AHA_TEST(NumericTest, float_to_str_p3_signed) {
    assertNumberToStr(-5526.12456456f, HABaseDeviceType::PrecisionP3, "-5526.124");
}

AHA_TEST(NumericTest, str_to_number_max) {
    assertStrToNumber(9223372036854775807, "9223372036854775807");
}

AHA_TEST(NumericTest, str_to_number_min) {
    assertStrToNumber(-9223372036854775807, "-9223372036854775807");
}

AHA_TEST(NumericTest, str_to_number_zero) {
    assertStrToNumber(0, "0");
}

AHA_TEST(NumericTest, str_to_number_signed_0) {
    assertStrToNumber(-1, "-1");
}

AHA_TEST(NumericTest, str_to_number_signed_1) {
    assertStrToNumber(-12, "-12");
}

AHA_TEST(NumericTest, str_to_number_signed_2) {
    assertStrToNumber(-123, "-123");
}

AHA_TEST(NumericTest, str_to_number_signed_3) {
    assertStrToNumber(-1234, "-1234");
}

AHA_TEST(NumericTest, str_to_number_signed_4) {
    assertStrToNumber(-1234, "-1234");
}

AHA_TEST(NumericTest, str_to_number_signed_5) {
    assertStrToNumber(-12345, "-12345");
}

AHA_TEST(NumericTest, str_to_number_signed_6) {
    assertStrToNumber(-123456, "-123456");
}

AHA_TEST(NumericTest, str_to_number_signed_7) {
    assertStrToNumber(-1234567, "-1234567");
}

AHA_TEST(NumericTest, str_to_number_signed_8) {
    assertStrToNumber(-12345678, "-12345678");
}

AHA_TEST(NumericTest, str_to_number_signed_9) {
    assertStrToNumber(-123456789, "-123456789");
}

AHA_TEST(NumericTest, str_to_number_signed_10) {
    assertStrToNumber(-1234567890, "-1234567890");
}

AHA_TEST(NumericTest, str_to_number_signed_11) {
    assertStrToNumber(-12345678901, "-12345678901");
}

AHA_TEST(NumericTest, str_to_number_signed_12) {
    assertStrToNumber(-123456789012, "-123456789012");
}

AHA_TEST(NumericTest, str_to_number_signed_13) {
    assertStrToNumber(-1234567890123, "-1234567890123");
}

AHA_TEST(NumericTest, str_to_number_signed_14) {
    assertStrToNumber(-123456789012345, "-123456789012345");
}

AHA_TEST(NumericTest, str_to_number_signed_15) {
    assertStrToNumber(-1234567890123456, "-1234567890123456");
}

AHA_TEST(NumericTest, str_to_number_unsigned_0) {
    assertStrToNumber(1, "1");
}

AHA_TEST(NumericTest, str_to_number_unsigned_1) {
    assertStrToNumber(12, "12");
}

AHA_TEST(NumericTest, str_to_number_unsigned_2) {
    assertStrToNumber(123, "123");
}

AHA_TEST(NumericTest, str_to_number_unsigned_3) {
    assertStrToNumber(1234, "1234");
}

AHA_TEST(NumericTest, str_to_number_unsigned_4) {
    assertStrToNumber(1234, "1234");
}

AHA_TEST(NumericTest, str_to_number_unsigned_5) {
    assertStrToNumber(12345, "12345");
}

AHA_TEST(NumericTest, str_to_number_unsigned_6) {
    assertStrToNumber(123456, "123456");
}

AHA_TEST(NumericTest, str_to_number_unsigned_7) {
    assertStrToNumber(1234567, "1234567");
}

AHA_TEST(NumericTest, str_to_number_unsigned_8) {
    assertStrToNumber(12345678, "12345678");
}

AHA_TEST(NumericTest, str_to_number_unsigned_9) {
    assertStrToNumber(123456789, "123456789");
}

AHA_TEST(NumericTest, str_to_number_unsigned_10) {
    assertStrToNumber(1234567890, "1234567890");
}

AHA_TEST(NumericTest, str_to_number_unsigned_11) {
    assertStrToNumber(12345678901, "12345678901");
}

AHA_TEST(NumericTest, str_to_number_unsigned_12) {
    assertStrToNumber(123456789012, "123456789012");
}

AHA_TEST(NumericTest, str_to_number_unsigned_13) {
    assertStrToNumber(1234567890123, "1234567890123");
}

AHA_TEST(NumericTest, str_to_number_unsigned_14) {
    assertStrToNumber(123456789012345, "123456789012345");
}

AHA_TEST(NumericTest, str_to_number_unsigned_15) {
    assertStrToNumber(1234567890123456, "1234567890123456");
}

AHA_TEST(NumericTest, str_to_number_null) {
    const char* num = nullptr;
    assertStrToNumberInvalid(num);
}

AHA_TEST(NumericTest, str_to_number_unsigned_overflow) {
    assertStrToNumberInvalid("92233720368547758078");
}

AHA_TEST(NumericTest, str_to_number_signed_overflow) {
    assertStrToNumberInvalid("-92233720368547758078");
}

AHA_TEST(NumericTest, str_to_number_invalid_0) {
    assertStrToNumberInvalid("--12");
}

AHA_TEST(NumericTest, str_to_number_invalid_1) {
    assertStrToNumberInvalid("a1");
}

AHA_TEST(NumericTest, str_to_number_invalid_2) {
    assertStrToNumberInvalid("567a32");
}

AHA_TEST(NumericTest, str_to_number_invalid_3) {
    assertStrToNumberInvalid("15.334");
}

AHA_TEST(NumericTest, number_to_float_1) {
    assertNear(HANumeric(500, 0).toFloat(), 500.0, 0.01);
}

AHA_TEST(NumericTest, number_to_float_2) {
    assertNear(HANumeric(50, 1).toFloat(), 50.0, 0.01);
}

AHA_TEST(NumericTest, number_to_float_3) {
    assertNear(HANumeric(5, 2).toFloat(), 5.0, 0.01);
}

AHA_TEST(NumericTest, number_to_float_4) {
    assertNear(HANumeric(0.5f, 3).toFloat(), 0.5, 0.01);
}

AHA_TEST(NumericTest, number_to_float_5) {
    assertNear(HANumeric(-265.544f, 3).toFloat(), -265.544, 0.0001);
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
