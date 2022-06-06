#ifndef AHA_HADICTIONARY_H
#define AHA_HADICTIONARY_H

// decorators
extern const char HASerializerSlash[];
extern const char HASerializerJsonDataPrefix[];
extern const char HASerializerJsonDataSuffix[];
extern const char HASerializerJsonPropertyPrefix[];
extern const char HASerializerJsonPropertySuffix[];
extern const char HASerializerJsonEscapeChar[];
extern const char HASerializerJsonPropertiesSeparator[];
extern const char HASerializerJsonArrayPrefix[];
extern const char HASerializerJsonArraySuffix[];

// properties
extern const char HADeviceIdentifiersProperty[];
extern const char HADeviceManufacturerProperty[];
extern const char HADeviceModelProperty[];
extern const char HADeviceSoftwareVersionProperty[];
extern const char HANameProperty[];
extern const char HAUniqueIdProperty[];
extern const char HADeviceProperty[];
extern const char HADeviceClassProperty[];
extern const char HAIconProperty[];
extern const char HARetainProperty[];
extern const char HASourceTypeProperty[];
extern const char HAEncodingProperty[];

// topics
extern const char HAConfigTopic[];
extern const char HAAvailabilityTopic[];
extern const char HATopic[];
extern const char HAStateTopic[];
extern const char HACommandTopic[];
extern const char HAPositionTopic[];

// misc
extern const char HAOnline[];
extern const char HAOffline[];
extern const char HAStateOn[];
extern const char HAStateOff[];
extern const char HAStateLocked[];
extern const char HAStateUnlocked[];
extern const char HATrue[];
extern const char HAFalse[];
extern const char HAHome[];
extern const char HANotHome[];

// covers
extern const char HAClosedState[];
extern const char HAClosingState[];
extern const char HAOpenState[];
extern const char HAOpeningState[];
extern const char HAStoppedState[];

// commands
extern const char HAOpenCommand[];
extern const char HACloseCommand[];
extern const char HAStopCommand[];
extern const char HALockCommand[];
extern const char HAUnlockCommand[];

// device tracker
extern const char HAGPSType[];
extern const char HARouterType[];
extern const char HABluetoothType[];
extern const char HABluetoothLEType[];

// camera
extern const char HAEncodingBase64[];

#endif
