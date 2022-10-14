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
    assertNumberSize(0.0f, 1, 1)
}

AHA_TEST(NumericTest, calculate_float_p1_zero_signed) {
    assertNumberSize(-0.123f, 1, 4)
}

AHA_TEST(NumericTest, calculate_float_p1_zero_unsigned) {
    assertNumberSize(0.123f, 1, 3)
}

AHA_TEST(NumericTest, calculate_float_p1_small) {
    assertNumberSize(1.0f, 1, 3)
}

AHA_TEST(NumericTest, calculate_float_p1_medium) {
    assertNumberSize(50.5f, 1, 4)
}

AHA_TEST(NumericTest, calculate_float_p1_large) {
    assertNumberSize(5526.02f, 1, 6)
}

AHA_TEST(NumericTest, calculate_float_p1_unsigned) {
    assertNumberSize(-5526.02f, 1, 7)
}

AHA_TEST(NumericTest, calculate_float_p2_zero) {
    assertNumberSize(0.0f, 2, 1)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_signed) {
    assertNumberSize(-0.123f, 2, 5)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_with_padding) {
    assertNumberSize(0.012f, 2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_zero_unsigned) {
    assertNumberSize(0.123f, 2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_small) {
    assertNumberSize(1.0f, 2, 4)
}

AHA_TEST(NumericTest, calculate_float_p2_medium) {
    assertNumberSize(50.5f, 2, 5)
}

AHA_TEST(NumericTest, calculate_float_p2_large) {
    assertNumberSize(5526.5f, 2, 7)
}

AHA_TEST(NumericTest, calculate_float_p2_unsigned) {
    assertNumberSize(-5526.12f, 2, 8)
}

AHA_TEST(NumericTest, calculate_float_p3_zero) {
    assertNumberSize(0.0f, 3, 1)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_signed) {
    assertNumberSize(-0.123f, 3, 6)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_with_padding) {
    assertNumberSize(0.012f, 3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_zero_unsigned) {
    assertNumberSize(0.123f, 3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_small) {
    assertNumberSize(1.0f, 3, 5)
}

AHA_TEST(NumericTest, calculate_float_p3_medium) {
    assertNumberSize(50.5f, 3, 6)
}

AHA_TEST(NumericTest, calculate_float_p3_large) {
    assertNumberSize(5526.5f, 3, 8)
}

AHA_TEST(NumericTest, calculate_float_p3_unsigned) {
    assertNumberSize(-5526.12f, 3, 9)
}

AHA_TEST(NumericTest, number_to_str_zero_p0) {
    assertNumberToStr(0, 0, "0");
}

AHA_TEST(NumericTest, number_to_str_zero_p1) {
    assertNumberToStr(0, 1, "0");
}

AHA_TEST(NumericTest, number_to_str_zero_p2) {
    assertNumberToStr(0, 2, "0");
}

AHA_TEST(NumericTest, number_to_str_zero_p3) {
    assertNumberToStr(0, 3, "0");
}

AHA_TEST(NumericTest, number_to_str_int8_p0) {
    int8_t value = -8;
    assertNumberToStr(value, 0, "-8");
}

AHA_TEST(NumericTest, number_to_str_int8_p1) {
    int8_t value = -66;
    assertNumberToStr(value, 1, "-66.0");
}

AHA_TEST(NumericTest, number_to_str_int8_p2) {
    int8_t value = 123;
    assertNumberToStr(value, 2, "123.00");
}

AHA_TEST(NumericTest, number_to_str_int8_p3) {
    int8_t value = -123;
    assertNumberToStr(value, 3, "-123.000");
}

AHA_TEST(NumericTest, number_to_str_int16_p0) {
    int16_t value = -12345;
    assertNumberToStr(value, 0, "-12345");
}

AHA_TEST(NumericTest, number_to_str_int16_p1) {
    int16_t value = 543;
    assertNumberToStr(value, 1, "543.0");
}

AHA_TEST(NumericTest, number_to_str_int16_p2) {
    int16_t value = 1234;
    assertNumberToStr(value, 2, "1234.00");
}

AHA_TEST(NumericTest, number_to_str_int16_p3) {
    int16_t value = -80;
    assertNumberToStr(value, 3, "-80.000");
}

AHA_TEST(NumericTest, number_to_str_int32_p0) {
    int32_t value = -123456;
    assertNumberToStr(value, 0, "-123456");
}

AHA_TEST(NumericTest, number_to_str_int32_p1) {
    int32_t value = 54345;
    assertNumberToStr(value, 1, "54345.0");
}

AHA_TEST(NumericTest, number_to_str_int32_p2) {
    int32_t value = 1234;
    assertNumberToStr(value, 2, "1234.00");
}

AHA_TEST(NumericTest, number_to_str_int32_p3) {
    int32_t value = -801;
    assertNumberToStr(value, 3, "-801.000");
}

AHA_TEST(NumericTest, number_to_str_uint8_p0) {
    uint8_t value = 8;
    assertNumberToStr(value, 0, "8");
}

AHA_TEST(NumericTest, number_to_str_uint8_p1) {
    uint8_t value = 66;
    assertNumberToStr(value, 1, "66.0");
}

AHA_TEST(NumericTest, number_to_str_uint8_p2) {
    uint8_t value = 123;
    assertNumberToStr(value, 2, "123.00");
}

AHA_TEST(NumericTest, number_to_str_uint8_p3) {
    uint8_t value = 255;
    assertNumberToStr(value, 3, "255.000");
}

AHA_TEST(NumericTest, number_to_str_uint16_p0) {
    uint16_t value = 12345;
    assertNumberToStr(value, 0, "12345");
}

AHA_TEST(NumericTest, number_to_str_uint16_p1) {
    uint16_t value = 543;
    assertNumberToStr(value, 1, "543.0");
}

AHA_TEST(NumericTest, number_to_str_uint16_p2) {
    uint16_t value = 1234;
    assertNumberToStr(value, 2, "1234.00");
}

AHA_TEST(NumericTest, number_to_str_uint16_p3) {
    uint16_t value = 80;
    assertNumberToStr(value, 3, "80.000");
}

AHA_TEST(NumericTest, number_to_str_uint32_p0) {
    uint32_t value = 123456;
    assertNumberToStr(value, 0, "123456");
}

AHA_TEST(NumericTest, number_to_str_uint32_p1) {
    uint32_t value = 54345;
    assertNumberToStr(value, 1, "54345.0");
}

AHA_TEST(NumericTest, number_to_str_uint32_p2) {
    uint32_t value = 1234;
    assertNumberToStr(value, 2, "1234.00");
}

AHA_TEST(NumericTest, number_to_str_uint32_p3) {
    uint32_t value = 801;
    assertNumberToStr(value, 3, "801.000");
}

AHA_TEST(NumericTest, number_to_str_float_p0_zero) {
    assertNumberToStr(0.0f, 0, "0");
}

AHA_TEST(NumericTest, number_to_str_float_p0_small) {
    assertNumberToStr(1.0f, 0, "1");
}

AHA_TEST(NumericTest, number_to_str_float_p0_large) {
    assertNumberToStr(5526.5f, 0, "5526");
}

AHA_TEST(NumericTest, number_to_str_float_p0_signed) {
    assertNumberToStr(-5526.12f, 0, "-5526");
}

AHA_TEST(NumericTest, number_to_str_float_p1_zero) {
    assertNumberToStr(0.0f, 1, "0");
}

AHA_TEST(NumericTest, number_to_str_float_p1_zero_decimal) {
    assertNumberToStr(0.123f, 1, "0.1");
}

AHA_TEST(NumericTest, number_to_str_float_p1_zero_decimal_signed) {
    assertNumberToStr(-0.123f, 1, "-0.1");
}

AHA_TEST(NumericTest, number_to_str_float_p1_small) {
    assertNumberToStr(1.0f, 1, "1.0");
}

AHA_TEST(NumericTest, number_to_str_float_p1_medium) {
    assertNumberToStr(50.5f, 1, "50.5");
}

AHA_TEST(NumericTest, number_to_str_float_p1_large) {
    assertNumberToStr(5526.5f, 1, "5526.5");
}

AHA_TEST(NumericTest, number_to_str_float_p1_signed) {
    assertNumberToStr(-5526.12f, 1, "-5526.1");
}

AHA_TEST(NumericTest, number_to_str_float_p2_zero) {
    assertNumberToStr(0.0f, 2, "0");
}

AHA_TEST(NumericTest, number_to_str_float_p2_zero_decimal) {
    assertNumberToStr(0.123f, 2, "0.12");
}

AHA_TEST(NumericTest, number_to_str_float_p2_zero_with_padding) {
    assertNumberToStr(0.01f, 2, "0.01");
}

AHA_TEST(NumericTest, number_to_str_float_p2_zero_decimal_signed) {
    assertNumberToStr(-0.123f, 2, "-0.12");
}

AHA_TEST(NumericTest, number_to_str_float_p2_small) {
    assertNumberToStr(1.0f, 2, "1.00");
}

AHA_TEST(NumericTest, number_to_str_float_p2_medium) {
    assertNumberToStr(50.50f, 2, "50.50");
}

AHA_TEST(NumericTest, number_to_str_float_p2_large) {
    assertNumberToStr(5526.5f, 2, "5526.50");
}

AHA_TEST(NumericTest, number_to_str_float_p2_signed) {
    assertNumberToStr(-5526.12f, 2, "-5526.12");
}

AHA_TEST(NumericTest, number_to_str_float_p3_zero) {
    assertNumberToStr(0.0f, 3, "0");
}

AHA_TEST(NumericTest, number_to_str_float_p3_zero_decimal) {
    assertNumberToStr(0.123f, 3, "0.123");
}

AHA_TEST(NumericTest, number_to_str_float_p3_zero_with_padding) {
    assertNumberToStr(0.001f, 3, "0.001");
}

AHA_TEST(NumericTest, number_to_str_float_p3_zero_decimal_signed) {
    assertNumberToStr(-0.123f, 3, "-0.123");
}

AHA_TEST(NumericTest, number_to_str_float_p3_small) {
    assertNumberToStr(1.123f, 3, "1.123");
}

AHA_TEST(NumericTest, number_to_str_float_p3_medium) {
    assertNumberToStr(50.555f, 3, "50.555");
}

AHA_TEST(NumericTest, number_to_str_float_p3_large) {
    assertNumberToStr(5526.5f, 3, "5526.500");
}

AHA_TEST(NumericTest, number_to_str_float_p3_signed) {
    assertNumberToStr(-5526.12456456f, 3, "-5526.124");
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
