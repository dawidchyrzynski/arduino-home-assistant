#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    commandCallbackCalled = false; \
    commandCallbackScenePtr = nullptr;

#define assertCallback(shouldBeCalled, callerPtr) \
    assertTrue(commandCallbackCalled == shouldBeCalled); \
    assertEqual(callerPtr, commandCallbackScenePtr);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueScene";
static bool commandCallbackCalled = false;
static HAScene* commandCallbackScenePtr = nullptr;

const char ConfigTopic[] PROGMEM = {"homeassistant/scene/testDevice/uniqueScene/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueScene/cmd_t"};
const char CommandMessage[] PROGMEM = {"on"};

void onCommandReceived(HAScene* scene)
{
    commandCallbackCalled = true;
    commandCallbackScenePtr = scene;
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
        "{\"uniq_id\":\"uniqueScene\",\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\"}"
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
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueScene\",\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\"}"
    )
}

AHA_TEST(SceneTest, icon_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setIcon("testIcon");

    assertEntityConfig(
        mock,
        scene,
        "{\"uniq_id\":\"uniqueScene\",\"ic\":\"testIcon\",\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\"}"
    )
}

AHA_TEST(SceneTest, retain_setter) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.setRetain(true);

    assertEntityConfig(
        mock,
        scene,
        "{\"uniq_id\":\"uniqueScene\",\"ret\":true,\"cmd_t\":\"testData/testDevice/uniqueScene/cmd_t\"}"
    )
}

AHA_TEST(SceneTest, command_callback) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.onActivate(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertCallback(true, &scene)
}

AHA_TEST(SceneTest, no_command_callback) {
    prepareTest

    HAScene scene(testUniqueId);
    mock->fakeMessage(AHATOFSTR(CommandTopic), AHATOFSTR(CommandMessage));

    assertCallback(false, nullptr)
}

AHA_TEST(SceneTest, different_scene_command) {
    prepareTest

    HAScene scene(testUniqueId);
    scene.onActivate(onCommandReceived);
    mock->fakeMessage(
        F("testData/testDevice/uniqueSceneDifferent/cmd_t"),
        AHATOFSTR(CommandMessage)
    );

    assertCallback(false, nullptr)
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
