#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackCommand = unknownCommand; \
    commandCallbackLockPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedCommand, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedCommand, commandCallbackCommand); \
    assertEqual(callerPtr, commandCallbackLockPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueLock";
static const char* configTopic = "homeassistant/lock/testDevice/uniqueLock/config";
static const char* commandTopic = "testData/testDevice/uniqueLock/cmd_t";
static const HALock::LockCommand unknownCommand = static_cast<HALock::LockCommand>(0);

static bool commandCallbackCalled = false;
static HALock::LockCommand commandCallbackCommand = unknownCommand;
static HALock* commandCallbackLockPtr = nullptr;

void onCommandReceived(HALock::LockCommand command, HALock* lock)
{
    commandCallbackCalled = true;
    commandCallbackCommand = command;
    commandCallbackLockPtr = lock;
}

test(LockTest, invalid_unique_id) {
    prepareTest

    HALock lock(nullptr);
    lock.buildSerializerTest();
    HASerializer* serializer = lock.getSerializer();

    assertTrue(serializer == nullptr);
}

test(LockTest, default_params) {
    prepareTest

    HALock lock(testUniqueId);
    assertEntityConfig(
        mock,
        lock,
        "{\"uniq_id\":\"uniqueLock\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(LockTest, command_subscription) {
    prepareTest

    HALock lock(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertStringCaseEqual(commandTopic, mock->getSubscriptions()[0].topic);
}

test(LockTest, availability) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HALock
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueLock/avty_t",
        "online",
        true
    )
}

test(LockTest, publish_last_known_state) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setCurrentState(HALock::StateUnlocked);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueLock/stat_t",
        "unlocked",
        true
    )
}

test(LockTest, publish_nothing_if_retained) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setRetain(true);
    lock.setCurrentState(HALock::StateUnlocked);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(LockTest, name_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setName("testName");

    assertEntityConfig(
        mock,
        lock,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueLock\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\"}"
    )
}

test(LockTest, icon_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setIcon("testIcon");

    assertEntityConfig(
        mock,
        lock,
        "{\"uniq_id\":\"uniqueLock\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\"}"
    )
}

test(LockTest, retain_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setRetain(true);

    assertEntityConfig(
        mock,
        lock,
        "{\"uniq_id\":\"uniqueLock\",\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\"}"
    )
}

test(LockTest, current_state_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setCurrentState(HALock::StateLocked);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(HALock::StateLocked, lock.getCurrentState());
}

test(LockTest, publish_state_locked) {
    prepareTest

    mock->connectDummy();
    HALock lock(testUniqueId);
    bool result = lock.setState(HALock::StateLocked);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueLock/stat_t",
        "locked",
        true
    )
    assertTrue(result);
}

test(LockTest, publish_state_unlocked) {
    prepareTest

    mock->connectDummy();
    HALock lock(testUniqueId);
    bool result = lock.setState(HALock::StateUnlocked);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueLock/stat_t",
        "unlocked",
        true
    )
    assertTrue(result);
}

test(LockTest, command_lock) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "LOCK");

    assertCallback(true, HALock::CommandLock, &lock)
}

test(LockTest, command_unlock) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "UNLOCK");

    assertCallback(true, HALock::CommandUnlock, &lock)
}

test(LockTest, command_open) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "OPEN");

    assertCallback(true, HALock::CommandOpen, &lock)
}

test(LockTest, command_invalid) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "NOT_SUPPORTED");

    assertCallback(false, unknownCommand, nullptr)
}

test(LockTest, different_lock_command) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(
        "testData/testDevice/uniqueLockDifferent/cmd_t",
        "CLOSE"
    );

    assertCallback(false, unknownCommand, nullptr)
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    TestRunner::run();
    delay(1);
}
