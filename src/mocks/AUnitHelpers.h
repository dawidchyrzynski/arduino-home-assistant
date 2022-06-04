#ifndef AHA_AUNITHELPERS_H
#define AHA_AUNITHELPERS_H

#define initMqttTest(testDeviceId) \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    mqtt.setDataPrefix("testData");

#define assertNoMqttMessage() \
    assertTrue(mock->getFlushedMessagesNb() == 0);

#define assertSingleMqttMessage(eTopic, eMessage, eRetained) { \
    const __FlashStringHelper* messageP = F(eMessage); \
    size_t messageLen = strlen_P(reinterpret_cast<const char *>(messageP)); \
    assertEqual(1, mock->getFlushedMessagesNb()); \
    MqttMessage* publishedMessage = mock->getFirstFlushedMessage(); \
    assertStringCaseEqual(publishedMessage->topic, eTopic); \
    assertStringCaseEqual(publishedMessage->buffer, messageP); \
    assertEqual(publishedMessage->bufferSize - 1, messageLen); \
    assertEqual(publishedMessage->retained, eRetained); \
}

#define assertEntityConfig(mock, entity, expectedJson) \
{ \
    mock->connectDummy(); \
    entity.publishConfigTest(); \
    assertSingleMqttMessage(configTopic, expectedJson, true) \
    assertTrue(entity.getSerializer() == nullptr); \
}

#endif
