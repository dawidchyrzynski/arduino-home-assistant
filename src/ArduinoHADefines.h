// Turns on debug information of the ArduinoHA core.
// Please note that you need to initialize serial interface manually
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG

// #define EX_ARDUINOHA_BINARY_SENSOR
// #define EX_ARDUINOHA_BUTTON
// #define EX_ARDUINOHA_CAMERA
// #define EX_ARDUINOHA_COVER
// #define EX_ARDUINOHA_DEVICE_TRACKER
// #define EX_ARDUINOHA_DEVICE_TRIGGER
// #define EX_ARDUINOHA_LOCK
// #define EX_ARDUINOHA_SENSOR
// #define EX_ARDUINOHA_SWITCH
// #define EX_ARDUINOHA_TAG_SCANNER

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