#ifndef AHA_AUNITHELPERS_H
#define AHA_AUNITHELPERS_H

#define initMqttTest(testDeviceId) \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    mqtt.setDataPrefix("testData"); \
    mqtt.begin("testHost", "testUser", "testPass");

#define assertNoMqttMessage() \
    assertTrue(mock->getFlushedMessagesNb() == 0);

#define assertMqttMessage(index, eTopic, eMessage, eRetained) { \
    const __FlashStringHelper* messageP = F(eMessage); \
    size_t messageLen = strlen_P(reinterpret_cast<const char *>(messageP)); \
    assertTrue(mock->getFlushedMessagesNb() > 0); \
    assertTrue(mock->getFlushedMessagesNb() > index); \
    MqttMessage* publishedMessage = mock->getFlushedMessages()[index]; \
    assertEqual(eTopic, publishedMessage->topic); \
    assertEqual(messageP, publishedMessage->buffer); \
    assertEqual(messageLen, publishedMessage->bufferSize - 1); \
    assertEqual(eRetained, publishedMessage->retained); \
}

#define assertSingleMqttMessage(eTopic, eMessage, eRetained) { \
    assertEqual(1, mock->getFlushedMessagesNb()); \
    assertMqttMessage(0, eTopic, eMessage, eRetained) \
}

#define assertEntityConfig(mock, entity, expectedJson) \
{ \
    mqtt.loop(); \
    assertMqttMessage(0, configTopic, expectedJson, true) \
    assertTrue(entity.getSerializer() == nullptr); \
}

#ifdef AUNITER
#include <MemoryUsage.h>

#define AHA_LEAKTRACKSTART \
    int freeRam = mu_freeRam();

#define AHA_LEAKTRACKEND \
    int diff = freeRam - mu_freeRam(); \
    if (diff < 0) { diff *= -1; } \
    if (diff != 0) { \
        Serial.print(Test::getName().getFString()); \
        Serial.print(F(" memory leak: ")); \
        Serial.print(diff); \
        Serial.println(F("b")); \
        Test::fail(); \
    }
#else
// EpoxyDuino doesn't support memory tracking
#define AHA_LEAKTRACKSTART
#define AHA_LEAKTRACKEND
#endif

#define AHA_TEST(suiteName, name) \
class suiteName##_##name : public aunit::TestOnce { \
public: \
    suiteName##_##name(); \
    void once() override; \
    void loop() override { \
        AHA_LEAKTRACKSTART \
        once(); \
        if (isNotDone()) { pass(); } \
        AHA_LEAKTRACKEND \
    } \
} suiteName##_##name##_instance; \
suiteName##_##name :: suiteName##_##name() { \
  init(AUNIT_F(#suiteName "_" #name)); \
} \
void suiteName##_##name :: once()

#endif
