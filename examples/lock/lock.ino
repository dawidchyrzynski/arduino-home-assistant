#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myLock" is unique ID of the lock. You should define your own ID.
HALock lock("myLock");

void onLockCommand(HALock::LockCommand cmd, HALock* sender) {
    if (cmd == HALock::CommandLock) {
        Serial.println("Command: Lock");
        sender->setState(HALock::StateLocked); // report state back to the HA
    } else if (cmd == HALock::CommandUnlock) {
        Serial.println("Command: Unlock");
        sender->setState(HALock::StateUnlocked); // report state back to the HA
    } else if (cmd == HALock::CommandOpen) {
        if (sender->getCurrentState() != HALock::StateUnlocked) {
            return; // optionally you can verify if the lock is unlocked before opening
        }

        Serial.println("Command: Open");
    }
}

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);

    lock.onCommand(onLockCommand);
    lock.setName("My door lock"); // optional
    lock.setIcon("mdi:home"); // optional

    // Optionally you can set retain flag for the HA commands
    // lock.setRetain(true);

    // Optionally you can enable optimistic mode for the HALock.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // lock.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your lock using a button connected to the device.
    // lock.setState(HALock::StateLocked); // use any state you want
}
