// Turns on debug information of the ArduinoHA core.
// Please note that you need to initialize serial interface manually
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG

// You can reduce Flash size of the compiled library by commenting unused components below
#define ARDUINOHA_BINARY_SENSOR
//#define ARDUINOHA_COVER
//#define ARDUINOHA_FAN
//#define ARDUINOHA_HVAC
//#define ARDUINOHA_SENSOR
//#define ARDUINOHA_SWITCH
//#define ARDUINOHA_TAG_SCANNER
//#define ARDUINOHA_TRIGGERS

#if defined(ARDUINOHA_DEBUG)
    #include <Arduino.h>

    #define ARDUINOHA_DEBUG_INIT() Serial.begin(115200);
    #define ARDUINOHA_DEBUG_PRINTLN(x) Serial.println(F(x));
    #define ARDUINOHA_DEBUG_PRINT(x) Serial.print(F(x));
    #define ARDUINOHA_DEBUG_PRINTF(x, ...) Serial.printf(x, __VA_ARGS__);
#else
    #define ARDUINOHA_DEBUG_INIT()
    #define ARDUINOHA_DEBUG_PRINTLN(x)
    #define ARDUINOHA_DEBUG_PRINT(x)
    #define ARDUINOHA_DEBUG_PRINTF(...)
#endif