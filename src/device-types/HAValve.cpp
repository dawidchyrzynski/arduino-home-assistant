#include "HAValve.h"
#ifndef EX_ARDUINOHA_VALVE

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HANumeric.h"
#include "../utils/HASerializer.h"

HAValve::HAValve(const char* uniqueId, const uint16_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentValve), uniqueId),
    _features(features),
    _currentState(StateUnknown),
    _currentPosition(DefaultPosition),
    _positionOpen(HANumeric((uint16_t)100, 0)),
    _positionClosed(HANumeric((uint16_t)0, 0)),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _commandCallback(nullptr)
{
}

bool HAValve::setState(const ValveState state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }
    
    if (_features & PositionFeature) {
      // if position enabled, state topic wants position
      if (publishState(StatePublishJson, state, _currentPosition)) {
          _currentState = state;
          return true;
      }
    } else {
      // normal is standard state strings
      if (publishState(StatePublishString, state, -1)) {
          _currentState = state;
          return true;
      }
    }

    return false;
}

bool HAValve::setPosition(const int16_t position, const bool force)
{
    if (!force && _currentPosition == position) {
        return true;
    }

    if (publishState(StatePublishNumeric, StateUnknown, position)) {
        _currentPosition = position;
        return true;
    }

    return false;
}

bool HAValve::setStateWithPosition(const ValveState state, const int16_t position, const bool force) {
  if (!force && _currentState == state && _currentPosition == position) {
      return true;
  }

  if (publishState(StatePublishJson, state, position)) {
      _currentState = state;
      _currentPosition = position;
      return true;
  }

  return false;
}

void HAValve::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }
    
    _serializer = new HASerializer(this, 16); //  max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAObjectIdProperty), _objectId);
    _serializer->set(HASerializer::WithUniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    
    if (_positionOpen.toInt16() != 100) {
      _serializer->set(
        AHATOFSTR(HAPositionOpenProperty),
        &_positionOpen,
        HASerializer::NumberPropertyType
      );
    }
    
    if (_positionClosed.toInt16() != 0) {
      _serializer->set(
        AHATOFSTR(HAPositionClosedProperty),
        &_positionClosed,
        HASerializer::NumberPropertyType
      );
    }
    
    if (_features & PositionFeature) {
      _serializer->set(
        AHATOFSTR(HAReportsPosition),
        AHATOFSTR(HATrue),
        HASerializer::ProgmemPropertyValue
      );
    }
    
    if (_features & StopFeature) {
      _serializer->set(
        AHATOFSTR(HAPayloadStopProperty),
        AHATOFSTR(HAStopCommand),
        HASerializer::ProgmemPropertyValue
      );
    }

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAValve::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
      if (_features & PositionFeature) {
        publishState(StatePublishJson, _currentState, _currentPosition);
      } else {
        publishState(StatePublishString, _currentState, -1);
      }
      publishState(StatePublishNumeric, StateUnknown, _currentPosition);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HAValve::onMqttMessage(
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


bool HAValve::publishState(StatePublishType type, ValveState state, int16_t position)
{
  const __FlashStringHelper *stateStr = nullptr;
  char positionStr[6 + 1] = {0}; // int16_t digits with null terminator
  
  // for state only or combo
  if (type == StatePublishString  ||  type == StatePublishJson) {
    // ensure we have a valid state
    if (state == StateUnknown) {
      return false;
    } else {
      switch (state) {
      case StateClosed:
          stateStr = AHATOFSTR(HAClosedState);
          break;

      case StateClosing:
          stateStr = AHATOFSTR(HAClosingState);
          break;

      case StateOpen:
          stateStr = AHATOFSTR(HAOpenState);
          break;

      case StateOpening:
          stateStr = AHATOFSTR(HAOpeningState);
          break;

      default:
          return false;
      }
    }
  }
  
  // for position only or combo
  if (type == StatePublishNumeric  ||  type == StatePublishJson) {
    // ensure we have a valid position
    if (position == DefaultPosition || !(_features & PositionFeature)) {
      return false;
    } else {
      HANumeric(position, 0).toStr(positionStr);
    }
  }

  // publish
  if (type == StatePublishString) {
    return publishOnDataTopic(AHATOFSTR(HAStateTopic), stateStr, true);
  }
  
  if (type == StatePublishNumeric) {
    return publishOnDataTopic(AHATOFSTR(HAStateTopic), positionStr, true);
  }
  
  if (type == StatePublishJson) {
    char jsonState[40];
    snprintf(
      jsonState,
      40,
      "{\"%s\":\"%s\",\"%s\":%d}",
      AHAFROMFSTR(HAStateProperty),
      AHAFROMFSTR(stateStr),
      AHAFROMFSTR(HAPositionProperty),
      position
    );
    return publishOnDataTopic(AHATOFSTR(HAStateTopic), jsonState, true);
  }
    
  return false;
}


void HAValve::handleCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_commandCallback) {
        return;
    }

    if (memcmp_P(cmd, HACloseCommand, length) == 0) {
        _commandCallback(CommandClose, this);
    } else if (memcmp_P(cmd, HAOpenCommand, length) == 0) {
        _commandCallback(CommandOpen, this);
    } else if (memcmp_P(cmd, HAStopCommand, length) == 0) {
        _commandCallback(CommandStop, this);
    } else if (_features & PositionFeature) {
      HANumeric number = HANumeric::fromStr(cmd, length);
      if (number.isSet()) {
        _commandCallback(number.toInt16(), this);
      }
    }
}

#endif
