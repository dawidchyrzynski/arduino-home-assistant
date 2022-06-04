#ifndef AHA_AUNITHELPERS_H
#define AHA_AUNITHELPERS_H

#define initMqttTest(testDeviceId) \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    mqtt.setDataPrefix("testData"); \
    mock->connectDummy();

#define assertNoMqttMessage() \
    assertTrue(mock->getFlushedMessagesNb() == 0);

#define assertSingleMqttMessage(eTopic, eMessage, eRetained) { \
    const __FlashStringHelper* messageP = F(eMessage); \
    size_t messageLen = strlen_P(reinterpret_cast<const char *>(messageP)) + 1; \
    assertTrue(mock->getFlushedMessagesNb() == 1); \
    MqttMessage* publishedMessage = mock->getFirstFlushedMessage(); \
    assertStringCaseEqual(publishedMessage->topic, eTopic); \
    assertStringCaseEqual(publishedMessage->buffer, messageP); \
    assertEqual(publishedMessage->bufferSize, messageLen); \
    assertEqual(publishedMessage->retained, eRetained); \
}

#endif
