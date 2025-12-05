#include <Arduino.h>
#include <BiDiB.h>

// This example implements a simple BiDiB Booster Node.
// It receives switching commands (ON/OFF) and reports its status.

BiDiB bidib;

// Pin to control the booster hardware (e.g. Relay or H-Bridge enable)
// On Arduino Uno, Pin 13 has the built-in LED.
const int BOOSTER_ENABLE_PIN = 13;

// Current state
bool isBoosterOn = false;

// Callback function to handle incoming booster commands (MSG_BOOST_ON / MSG_BOOST_OFF)
void handleBoosterCommand(bool on) {
    isBoosterOn = on;
    digitalWrite(BOOSTER_ENABLE_PIN, on ? HIGH : LOW);

    // Report the new state back to the host
    if (on) {
        bidib.sendBoosterStatus(BIDIB_BST_STATE_ON);
    } else {
        bidib.sendBoosterStatus(BIDIB_BST_STATE_OFF);
    }
}

void setup() {
    Serial.begin(115200);
    // Initialize BiDiB with the Serial port
    bidib.begin(Serial);

    // Setup hardware pin
    pinMode(BOOSTER_ENABLE_PIN, OUTPUT);
    digitalWrite(BOOSTER_ENABLE_PIN, LOW);

    // Set a unique ID for this node.
    // In a real device, this should be unique per device.
    uint8_t my_uid[] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
    memcpy(bidib.unique_id, my_uid, 7);

    // Register the callback for booster commands
    bidib.onBoosterCommand(handleBoosterCommand);

    // Logon to the bus
    bidib.logon();
}

void loop() {
    // Regular update call
    bidib.update();

    // Process messages
    if (bidib.messageAvailable()) {
        bidib.handleMessages();
    }
}
