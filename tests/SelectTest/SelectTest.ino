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

const char ConfigTopic[] PROGMEM = {"homeassistant/select/testDevice/uniqueSelect/config"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueSelect/stat_t"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueSelect/cmd_t"};

static bool commandCallbackCalled = false;
static int8_t commandCallbackIndex = -1;
static HASelect* commandCallbackSelectPtr = nullptr;

void onCommandReceived(int8_t index, HASelect* select)
{
    commandCallbackCalled = true;
    commandCallbackIndex = index;
    commandCallbackSelectPtr = select;
}

AHA_TEST(SelectTest, invalid_unique_id) {
    prepareTest

    HASelect select(nullptr);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(SelectTest, no_options) {
    prepareTest

    HASelect select(testUniqueId);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(select.getOptions() == nullptr);
}

AHA_TEST(SelectTest, invalid_options_nullptr) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions(nullptr);
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(select.getOptions() == nullptr);
}

AHA_TEST(SelectTest, invalid_options_empty) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("");
    select.buildSerializerTest();
    HASerializer* serializer = select.getSerializer();

    assertTrue(serializer == nullptr);
    assertTrue(select.getOptions() == nullptr);
}

AHA_TEST(SelectTest, single_option) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A");

    assertEqual(1, select.getOptions()->getItemsNb());
    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(SelectTest, multiple_options) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");

    assertEqual(3, select.getOptions()->getItemsNb());
    assertEntityConfig(
        mock,
        select,
        "{\"uniq_id\":\"uniqueSelect\",\"options\":[\"Option A\",\"B\",\"C\"],\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSelect/stat_t\",\"cmd_t\":\"testData/testDevice/uniqueSelect/cmd_t\"}"
    )
    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(SelectTest, command_subscription) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(SelectTest, availability) {
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

AHA_TEST(SelectTest, publish_last_known_state) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "B", true)
}

AHA_TEST(SelectTest, publish_nothing_if_retained) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);
    select.setRetain(true);
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(SelectTest, name_setter) {
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

AHA_TEST(SelectTest, icon_setter) {
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

AHA_TEST(SelectTest, retain_setter) {
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

AHA_TEST(SelectTest, optimistic_setter) {
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

AHA_TEST(SelectTest, current_state_getter) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.setCurrentState(1);

    assertEqual(0, mock->getFlushedMessagesNb());
    assertEqual(1, select.getCurrentState());
}

AHA_TEST(SelectTest, publish_state_first) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    bool result = select.setState(0);

    assertTrue(select.getOptions() != nullptr);
    assertEqual(3, select.getOptions()->getItemsNb());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "Option A", true)
    assertTrue(result);
}

AHA_TEST(SelectTest, publish_state_last) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    bool result = select.setState(2);

    assertTrue(select.getOptions() != nullptr);
    assertEqual(3, select.getOptions()->getItemsNb());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "C", true)
    assertTrue(result);
}

AHA_TEST(SelectTest, publish_state_only) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A");
    bool result = select.setState(0);

    assertTrue(select.getOptions() != nullptr);
    assertEqual(1, select.getOptions()->getItemsNb());
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "Option A", true)
    assertTrue(result);
}

AHA_TEST(SelectTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A");
    select.setCurrentState(0);
    bool result = select.setState(0);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
    assertTrue(result);
}

AHA_TEST(SelectTest, publish_state_debounce_skip) {
    prepareTest

    mock->connectDummy();
    HASelect select(testUniqueId);
    select.setOptions("Option A");
    select.setCurrentState(0);
    bool result = select.setState(0, true);

    assertSingleMqttMessage(AHATOFSTR(StateTopic), "Option A", true)
    assertTrue(result);
}

AHA_TEST(SelectTest, command_option_first) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("Option A"));

    assertCallback(true, 0, &select)
}

AHA_TEST(SelectTest, command_option_middle) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("B"));

    assertCallback(true, 1, &select)
}

AHA_TEST(SelectTest, command_option_last) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("C"));

    assertCallback(true, 2, &select)
}

AHA_TEST(SelectTest, command_option_non_existing) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), F("DoesNotExist"));

    assertCallback(false, -1, nullptr)
}

AHA_TEST(SelectTest, different_select_command) {
    prepareTest

    HASelect select(testUniqueId);
    select.setOptions("Option A;B;C");
    select.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueDifferentSelect/cmd_t"),
        F("Option A")
    );

    assertCallback(false, -1, nullptr)
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
