/**
 * This example uses MFRC522 library: https://github.com/miguelbalboa/rfid
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <Ethernet.h>
#include <ArduinoHA.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN          53 // Arduino Mega
#define RST_PIN         5 // Arduino Mega
#define BROKER_ADDR     IPAddress(192,168,0,17)
#define SCAN_INTERVAL   5000

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

MFRC522 rfid(SS_PIN, RST_PIN);
EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HATagScanner scanner("myScanner");
unsigned long lastTagScannedAt = 0;

void scan() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    if (millis() - lastTagScannedAt < SCAN_INTERVAL) {
        return; // prevents spamming the HA. You can implement your own logic for debouncing the scans.
    }

    // the tag UID needs to be converted into string
    char tag[rfid.uid.size*2+1] = {0};
    HAUtils::byteArrayToStr(tag, rfid.uid.uidByte, rfid.uid.size);

    Serial.print("Card scanned: ");
    Serial.println(tag);

    scanner.tagScanned(tag); // let the HA know about the scanned tag
    lastTagScannedAt = millis();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");

    // you don't need to verify return status
    Ethernet.begin(mac);

    SPI.begin(); // Init SPI bus
    rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    scan();
}