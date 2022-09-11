#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackIndex = -1; \
    commandCallbackSelectPtr = nullptr;

#define assertCallback(shouldBeCalled, expectedIndex, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(expectedIndex, commandCallbackIndex); \
    assertEqual(callerPtr, commandCallbackSelectPtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSelect";
static const char* configTopic = "homeassistant/select/testDevice/uniqueSelect/config";
static const char* commandTopic = "testData/testDevice/uniqueSelect/cmd_t";

static bool commandCallbackCalled = false;
static int8_t commandCallbackIndex = -1;
static HASelect* commandCallbackSelectPtr = nullptr;

void onCommandReceived(int8_t index, HASelect* select)
{
    commandCallbackCalled = true;
    commandCallbackIndex = index;
    commandCallbackSelectPtr = select;
}

test(SelectTest, invalid_unique_id) {
    prepareTest

    HASelect select(nullptr);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SelectTest, no_options) {
    prepareTest

    HASelect select(testUniqueId);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SelectTest, invalid_options_nullptr) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions(nullptr);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SelectTest, invalid_options_empty) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("");
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SelectTest, single_option) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A");

    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(SelectTest, multiple_options) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");

    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\",\"B\",\"C\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(SelectTest, command_subscription) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertStringCaseEqual(commandTopic, mock->getSubscriptions()[0].topic);
}

test(SelectTest, availability) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HACover
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSelect/avty_t",
        "online",
        true
    )
}

test(SelectTest, publish_last_known_state) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSelect/stat_t",
        "B",
        true
    )
}

test(SelectTest, publish_nothing_if_retained) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);
    select.setRetain(true);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

test(SelectTest, name_setter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setName("testName");

    assertEntityConfig(
        mock,
        select,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\",\"B\",\"C\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
}

test(SelectTest, icon_setter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setIcon("testIcon");

    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"ic\":\"testIcon\",\"options\":[\"Option A\",\"B\",\"C\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
}

test(SelectTest, retain_setter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setRetain(true);

    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\",\"B\",\"C\"],\"ret\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
}

test(SelectTest, optimistic_setter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setOptimistic(true);

    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\",\"B\",\"C\"],\"opt\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
}

test(SelectTest, current_state_getter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(1, select.getCurrentState());
}

test(SelectTest, publish_state_first) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    bool result = select.setState(0);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueSelect/stat_t",
        "Option A",
        true
    )
    assertTrue(result);
}

test(SelectTest, publish_state_last) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    bool result = select.setState(2);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueSelect/stat_t",
        "C",
        true
    )
    assertTrue(result);
}

test(SelectTest, publish_state_only) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A");
    bool result = select.setState(0);

    assertSingleMqttMessage(
        "testData/testDevice/uniqueSelect/stat_t",
        "Option A",
        true
    )
    assertTrue(result);
}

test(SelectTest, command_option_first) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "Option A");

    assertCallback(true, 0, &select)
}

test(SelectTest, command_option_last) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "C");

    assertCallback(true, 2, &select)
}

test(SelectTest, command_option_non_existing) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(commandTopic, "DoesNotExist");

    assertCallback(false, -1, nullptr)
}

test(SelectTest, different_select_command) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(
        "testData/testDevice/uniqueDifferentSelect/cmd_t",
        "Option A"
    );

    assertCallback(false, -1, nullptr)
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
