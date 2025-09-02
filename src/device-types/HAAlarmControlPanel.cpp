#include "HAAlarmControlPanel.h"
#ifndef EX_ARDUINOHA_ALARMPANEL

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HAAlarmControlPanel::HAAlarmControlPanel(
    const char* uniqueId,
    const uint16_t features
) :
    HABaseDeviceType(AHATOFSTR(HAComponentAlarmControlPanel), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _panelState(UnknownState),
    _code(nullptr),
    _codeArmRequired(true),
    _codeDisarmRequired(true),
    _codeTriggerRequired(true),
    _enableByDefault(true),
    _entityCategory(nullptr),
    _commandCallback(nullptr),
    _payloadArmAway(nullptr),
    _payloadArmHome(nullptr),
    _payloadArmNight(nullptr),
    _payloadArmVacation(nullptr),
    _payloadDisarm(nullptr),
    _payloadTrigger(nullptr)
{
    _supportedFeaturesSerializer = new HASerializerArray(4);
}

HAAlarmControlPanel::~HAAlarmControlPanel()
{
    if (_supportedFeaturesSerializer) {
        delete _supportedFeaturesSerializer;
    }
}

bool HAAlarmControlPanel::setPanelState(const PanelState state, const bool force)
{
    if (!force && state == _panelState) {
        return true;
    }

    if (publishPanelState(state)) {
        _panelState = state;
        return true;
    }

    return false;
}

void HAAlarmControlPanel::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 21); // 21 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAObjectIdProperty), _objectId);
    _serializer->set(HASerializer::WithUniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (!_codeArmRequired) {
        _serializer->set(
            AHATOFSTR(HACodeArmRequiredProperty),
            &_codeArmRequired,
            HASerializer::BoolPropertyType
        );
    }

    if (!_codeDisarmRequired) {
        _serializer->set(
            AHATOFSTR(HACodeDisarmRequiredProperty),
            &_codeDisarmRequired,
            HASerializer::BoolPropertyType
        );
    }

    if (!_codeTriggerRequired) {
        _serializer->set(
            AHATOFSTR(HACodeTriggerRequiredProperty),
            &_codeTriggerRequired,
            HASerializer::BoolPropertyType
        );
    }

    if (_code) {
        _serializer->set(AHATOFSTR(HACodeProperty), _code);
    }


    // Supported features
    if (_features != DefaultFeatures) {
        _supportedFeaturesSerializer->clear();

        if (_features & ArmAway) {
            _supportedFeaturesSerializer->add(HAArmAway);
        }

        if (_features & ArmHome) {
            _supportedFeaturesSerializer->add(HAArmHome);
        }

        if (_features & ArmNight) {
            _supportedFeaturesSerializer->add(HAArmNight);
        }

        if (_features & ArmVacation) {
            _supportedFeaturesSerializer->add(HAArmVacation);
        }

        if (_features & ArmCustomBypass) {
            _supportedFeaturesSerializer->add(HAArmCustomBypass);
        }

        if (_features & Trigger) {
            _supportedFeaturesSerializer->add(HATrigger);
        }

        _serializer->set(
            AHATOFSTR(HASupportedFeaturesProperty),
            _supportedFeaturesSerializer,
            HASerializer::ArrayPropertyType
        );
    }

    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
    _serializer->set(AHATOFSTR(HAPayloadArmAwayTopic), _payloadArmAway);
    _serializer->set(AHATOFSTR(HAPayloadArmHomeTopic), _payloadArmHome);
    _serializer->set(AHATOFSTR(HAPayloadArmNightTopic), _payloadArmNight);
    _serializer->set(AHATOFSTR(HAPayloadArmVacationTopic), _payloadArmVacation);
    _serializer->set(AHATOFSTR(HAPayloadArmCustomBypassTopic), _payloadArmCustomBypass);
    _serializer->set(AHATOFSTR(HAPayloadDisarmTopic), _payloadDisarm);
    _serializer->set(AHATOFSTR(HAPayloadTriggerTopic), _payloadTrigger);
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
}

void HAAlarmControlPanel::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (!_retain) {
        publishPanelState(_panelState);


    }
}

void HAAlarmControlPanel::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        handleCommand(payload, length);
    }
}

bool HAAlarmControlPanel::publishPanelState(const PanelState state)
{
    const __FlashStringHelper* str = nullptr;
    switch (state)
    {
        case ArmedAway: str = AHATOFSTR(HAStateArmedAway); break;
        case ArmedCustomBypass: str = AHATOFSTR(HAStateArmedCustomBypass); break;
        case ArmedHome: str = AHATOFSTR(HAStateArmedHome); break;
        case ArmedNight: str = AHATOFSTR(HAStateArmedNight); break;
        case ArmedVacation: str = AHATOFSTR(HAStateArmedVacation); break;
        case Arming: str = AHATOFSTR(HAStateArming); break;
        case Disarmed: str = AHATOFSTR(HAStateDisarmed); break;
        case Disarming: str = AHATOFSTR(HAStateDisarming); break;
        case UnknownState: // fallthrough
        case Pending: str = AHATOFSTR(HAStatePending); break;
        case Triggered: str = AHATOFSTR(HAStateTriggered); break;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        str,
        true
    );
}

void HAAlarmControlPanel::handleCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_commandCallback) {
        return;
    }

    _commandCallback(cmd, length, this);
}

#endif