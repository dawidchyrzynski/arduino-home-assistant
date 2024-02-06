#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastActivateCallbackCall.reset();

#define assertActivateCallbackCalled(callerPtr) \
    assertTrue(lastActivateCallbackCall.called); \
    assertEqual(callerPtr, lastActivateCallbackCall.caller);

#define assertActivateCallbackNotCalled() \
    assertFalse(lastActivateCallbackCall.called);

using aunit::TestRunner;

struct ActivateCallback {
    bool called = false;
    HAScene* caller = nullptr;

    void reset() {
        called = false;
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueScene";
static ActivateCallback lastActivateCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/scene/testDevice/uniqueScene/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueScene/cmd_t"};
const char CommandMessage[] PROGMEM = {"on"};

void onCommandReceived(HAScene* caller)
{
    lastActivateCallbackCall.called = true;
    lastActivateCallbackCall.caller = caller;
}

AHA_TEST(SceneTest, invalid_unique_id) {
    prepareTest

    HAScene scene(nullptr);
    scene.buildSerializerTest();
    HASerializer* serializer = scene.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(SceneTest, default_params) {
    prepareTest

    HAScene scene(testUniqueId);
    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"uniq_id\":\"uniqueScene\","
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, extended_unique_id) {
    prepareTest

    device.enableExtendedUniqueIds();
    HAScene scene(testUniqueId);
    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"uniq_id\":\"testDevice_uniqueScene\","
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, command_subscription) {
    prepareTest

    HAScene scene(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(SceneTest, availability) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAScene
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueScene/avty_t"),
        "online",
        true
    )
}

AHA_TEST(SceneTest, name_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setName("testName");

    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueScene\","
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, object_id_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setObjectId("testId");

    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"obj_id\":\"testId\","
            "\"uniq_id\":\"uniqueScene\","
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, icon_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setIcon("testIcon");

    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"uniq_id\":\"uniqueScene\","
            "\"ic\":\"testIcon\","
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, retain_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setRetain(true);

    assertEntityConfig(
        mock,
        scene,
        (
            "{"
            "\"uniq_id\":\"uniqueScene\","
            "\"ret\":true,"
            "\"pl_on\":\"ON\","
            "\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(SceneTest, command_callback) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertActivateCallbackCalled(&scene)
}

AHA_TEST(SceneTest, no_command_callback) {
    prepareTest

    HAScene scene(testUniqueId);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertActivateCallbackNotCalled()
}

AHA_TEST(SceneTest, different_scene_command) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.onCommand(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueSceneDifferent/cmd_t"),
        AHATOFSTR(CommandMessage)
    );

    assertActivateCallbackNotCalled()
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
