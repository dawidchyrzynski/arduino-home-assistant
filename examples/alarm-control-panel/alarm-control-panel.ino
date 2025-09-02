#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myAlarmPanel" is the unique ID of the panel. Here we also specify what capabilities we want HA to present to the user.
HAAlarmControlPanel panel("myAlarmPanel", HAAlarmControlPanel::ArmAway | HAAlarmControlPanel::ArmHome | HAAlarmControlPanel::Trigger);

// The command callback that gets called when the user interacts with the panel in HA. Here's where we modify the real alarm 
// panel state and then report back to HA.
void onPanelCommand(const uint8_t* cmd, const uint16_t length, HAAlarmControlPanel* sender)
{
    if (sender == &panel) {
      // We know which instance was commanded. Here we would inspect the payload and take action
      if (cmd && cmd[0] == 'A') {
        // Arm the panel here

        // Then tell HA we've armed
        panel.setPanelState(HAAlarmControlPanel::ArmedAway);
      }
      else if (cmd && cmd[0] == 'H') {
        // Arm the panel here

        // Then tell HA we've armed
        panel.setPanelState(HAAlarmControlPanel::ArmedHome);
      }
      else if (cmd && cmd[0] == 'T') {
        // Trigger the alarm here

        // Then tell HA we've triggered the alarm
        panel.setPanelState(HAAlarmControlPanel::Triggered);
      }
      else if (cmd && cmd[0] == 'D') {
        // Disarm the panel here

        // Then tell HA we've disarmed the alarm
        panel.setPanelState(HAAlarmControlPanel::Disarmed);
      }
    }
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // optional device details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // optional properties
    panel.setIcon("mdi:fire");
    panel.setName("Security System");
    panel.setPayloadArmAway("A");
    panel.setPayloadArmHome("H");
    panel.setPayloadTrigger("T");
    panel.setPayloadDisarm("D");
    panel.setCodeArmRequired(false);
    panel.setCodeDisarmRequired(false);
    panel.setCodeTriggerRequired(true);
    panel.setCode("1234");
    panel.setRetain(true);
    
    // command callback. called when the user interacts with the panel in HA.
    panel.onCommand(onPanelCommand);

    // Set the initial state
    panel.setPanelState(HAAlarmControlPanel::Disarmed, true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
