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
    MqttMessage* publishedMessage = &mock->getFlushedMessages()[index]; \
    assertStringCaseEqual(F(eTopic), publishedMessage->topic); \
    assertStringCaseEqual(messageP, publishedMessage->buffer); \
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

#endif
