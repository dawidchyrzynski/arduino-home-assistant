#include "HASelect.h"
#ifndef EX_ARDUINOHA_SELECT

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HASelect::HASelect(const char* uniqueId) :
    HABaseDeviceType("select", uniqueId),
    _options(nullptr),
    _currentState(-1),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _commandCallback(nullptr)
{

}

HASelect::~HASelect()
{
    if (_options) {
        delete _options;
    }
}

void HASelect::setOptions(const char* options)
{
    if (!options || _options) { // options can be set only once
        return;
    }

    const uint16_t optionsNb = countOptionsInString(options);
    if (optionsNb == 0) {
        return;
    }

    const uint16_t optionsLen = strlen(options) + 1; // include null terminator
    _options = new HASerializerArray(optionsNb, false);

    if (optionsNb == 1) {
        _options->add(options);
        return;
    }

    uint8_t optionLen = 0;
    for (uint16_t i = 0; i < optionsLen; i++) {
        if (options[i] == ';' || options[i] == 0) {
            if (optionLen == 0) {
                break;
            }

            char* option = new char[optionLen];
            option[optionLen] = 0;
            memcpy(option, &options[i - optionLen], optionLen);

            _options->add(option);
            optionLen = 0;
            continue;
        }

        optionLen++;
    }
}

bool HASelect::setState(const int8_t state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HASelect::buildSerializer()
{
    if (_serializer || !uniqueId() || !_options) {
        return;
    }

    _serializer = new HASerializer(this, 10); // 10 - max properties nb
    _serializer->set(HANameProperty, _name);
    _serializer->set(HAUniqueIdProperty, _uniqueId);
    _serializer->set(HAIconProperty, _icon);
    _serializer->set(
        HAOptionsProperty,
        _options,
        HASerializer::ArrayPropertyType
    );

    if (_retain) {
        _serializer->set(
            HARetainProperty,
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            HAOptimisticProperty,
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(HAStateTopic);
    _serializer->topic(HACommandTopic);
}

void HASelect::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
    }

    subscribeTopic(uniqueId(), HACommandTopic);
}

void HASelect::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    (void)payload;
    (void)length;

    if (_commandCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        HACommandTopic
    )) {
        char option[length + 1];
        memset(option, 0, sizeof(option));
        memcpy(option, payload, length);

        const uint8_t optionsNb = _options->getItemsNb();
        const HASerializerArray::ItemType* options = _options->getItems();

        for (uint8_t i = 0; i < optionsNb; i++) {
            if (strcmp(option, options[i]) == 0) {
                _commandCallback(i, this);
                return;
            }
        }
    }
}

bool HASelect::publishState(const int8_t state)
{
    if (
        !uniqueId() ||
        state == -1 ||
        !_options ||
        state >= _options->getItemsNb()
    ) {
        return false;
    }

    const char* item = _options->getItems()[state];
    if (!item) {
        return false;
    }

    return publishOnDataTopic(HAStateTopic, item, true);
}

uint8_t HASelect::countOptionsInString(const char* options) const
{
    // the given string is treated as a single option if there are no semicolons
    uint8_t optionsNb = 1;
    const uint16_t optionsLen = strlen(options);

    if (optionsLen == 0) {
        return 0;
    }

    for (uint8_t i = 0; i < optionsLen; i++) {
        if (options[i] == ';') {
            optionsNb++;
        }
    }

    return optionsNb;
}

#endif
