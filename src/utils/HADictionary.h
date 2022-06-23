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
extern const char HASerializerUnderscore[];

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
extern const char HAOptimisticProperty[];
extern const char HAAutomationTypeProperty[];
extern const char HATypeProperty[];
extern const char HASubtypeProperty[];
extern const char HAForceUpdateProperty[];
extern const char HAUnitOfMeasurementProperty[];
extern const char HAValueTemplateProperty[];

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
extern const char HATrigger[];

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

// trigger
extern const char HAButtonShortPressType[];
extern const char HAButtonShortReleaseType[];
extern const char HAButtonLongPressType[];
extern const char HAButtonLongReleaseType[];
extern const char HAButtonDoublePressType[];
extern const char HAButtonTriplePressType[];
extern const char HAButtonQuadruplePressType[];
extern const char HAButtonQuintuplePressType[];
extern const char HATurnOnSubtype[];
extern const char HATurnOffSubtype[];
extern const char HAButton1Subtype[];
extern const char HAButton2Subtype[];
extern const char HAButton3Subtype[];
extern const char HAButton4Subtype[];
extern const char HAButton5Subtype[];
extern const char HAButton6Subtype[];

// value templates
extern const char HAValueTemplateFloatP1[];
extern const char HAValueTemplateFloatP2[];
extern const char HAValueTemplateFloatP3[];
extern const char HAValueTemplateFloatP4[];

#endif
