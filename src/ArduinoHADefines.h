// Turns on debug information of the ArduinoHA core.
// Please note that you need to initialize serial interface manually
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG

// You can reduce Flash size of the compiled library by commenting unused components below
#define ARDUINOHA_BINARY_SENSOR
#define ARDUINOHA_COVER
#define ARDUINOHA_FAN
#define ARDUINOHA_HVAC
#define ARDUINOHA_SENSOR
#define ARDUINOHA_SWITCH
#define ARDUINOHA_TAG_SCANNER
#define ARDUINOHA_TRIGGERS
