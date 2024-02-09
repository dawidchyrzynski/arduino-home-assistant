#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastCommandCallbackCall.reset();

#define assertCommandCallbackCalled(expectedCommand, callerPtr) \
    assertTrue(lastCommandCallbackCall.called); \
    assertEqual(expectedCommand, lastCommandCallbackCall.command); \
    assertEqual(callerPtr, lastCommandCallbackCall.caller);

#define assertCommandCallbackNotCalled() \
    assertFalse(lastCommandCallbackCall.called);

using aunit::TestRunner;

struct CommandCallback {
    bool called = false;
    HALock::LockCommand command = static_cast<HALock::LockCommand>(0);
    HALock* caller = nullptr;

    void reset() {
        called = false;
        command = static_cast<HALock::LockCommand>(0);
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueLock";
static CommandCallback lastCommandCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/lock/testDevice/uniqueLock/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueLock/cmd_t"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueLock/stat_t"};

void onCommandReceived(HALock::LockCommand command, HALock* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.command = command;
    lastCommandCallbackCall.caller = caller;
}

AHA_TEST(LockTest, invalid_unique_id) {
    prepareTest

    HALock lock(nullptr);
    lock.buildSerializerTest();
    HASerializer* serializer = lock.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(LockTest, default_params) {
    prepareTest

    HALock lock(testUniqueId);
    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"uniq_id\":\"uniqueLock\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(LockTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HALock lock(testUniqueId);
    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueLock\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(LockTest, command_subscription) {
    prepareTest

    HALock lock(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(LockTest, availability) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HALock
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueLock/avty_t"),
        "online",
        true
    )
}

AHA_TEST(LockTest, publish_last_known_state) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setCurrentState(HALock::StateUnlocked);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "UNLOCKED", true)
}

AHA_TEST(LockTest, publish_nothing_if_retained) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setRetain(true);
    lock.setCurrentState(HALock::StateUnlocked);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(LockTest, name_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setName("testName");

    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueLock\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LockTest, object_id_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setObjectId("testId");

    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueLock\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LockTest, icon_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setIcon("testIcon");

    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"uniq_id\":\"uniqueLock\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LockTest, retain_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setRetain(true);

    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"uniq_id\":\"uniqueLock\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LockTest, optimistic_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setOptimistic(true);

    assertEntityConfig(
        mock,
        lock,
        (
            "{"
            "\"uniq_id\":\"uniqueLock\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueLock/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueLock/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(LockTest, current_state_setter) {
    prepareTest

    HALock lock(testUniqueId);
    lock.setCurrentState(HALock::StateLocked);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(HALock::StateLocked, lock.getCurrentState());
}

AHA_TEST(LockTest, publish_state_locked) {
    prepareTest

    mock->connectDummy();
    HALock lock(testUniqueId);

    assertTrue(lock.setState(HALock::StateLocked));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "LOCKED", true)
}

AHA_TEST(LockTest, publish_state_unlocked) {
    prepareTest

    mock->connectDummy();
    HALock lock(testUniqueId);

    assertTrue(lock.setState(HALock::StateUnlocked));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "UNLOCKED", true)
}

AHA_TEST(LockTest, command_lock) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("LOCK"));

    assertCommandCallbackCalled(HALock::CommandLock, &lock)
}

AHA_TEST(LockTest, command_unlock) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("UNLOCK"));

    assertCommandCallbackCalled(HALock::CommandUnlock, &lock)
}

AHA_TEST(LockTest, command_open) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("OPEN"));

    assertCommandCallbackCalled(HALock::CommandOpen, &lock)
}

AHA_TEST(LockTest, command_invalid) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("NOT_SUPPORTED"));

    assertCommandCallbackNotCalled()
}

AHA_TEST(LockTest, different_lock_command) {
    prepareTest

    HALock lock(testUniqueId);
    lock.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueLockDifferent/cmd_t"),
        F("CLOSE")
    );

    assertCommandCallbackNotCalled()
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
