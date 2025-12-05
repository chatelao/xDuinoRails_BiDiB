#include <Arduino.h>
#include <BiDiB.h>

// This example implements a simple Occupancy Detector Node.
// It reports occupancy state of a sensor (e.g., a button or current sensor).

BiDiB bidib;

const int SENSOR_PIN = 2; // Pin where the sensor is connected (active low)

bool lastState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(115200);
    bidib.begin(Serial);

    pinMode(SENSOR_PIN, INPUT_PULLUP);

    // Set unique ID
    uint8_t my_uid[] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
    memcpy(bidib.unique_id, my_uid, 7);

    // Enable Secure-ACK feature support
    bidib.setFeature(FEATURE_BM_SECACK_AVAILABLE, 1);

    bidib.logon();
}

void loop() {
    bidib.update();
    if (bidib.messageAvailable()) {
        bidib.handleMessages();
    }

    // Check sensor
    int reading = digitalRead(SENSOR_PIN);
    bool currentState = (reading == LOW); // Active Low means Occupied

    if (currentState != lastState) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastState = currentState;
            lastDebounceTime = millis();

            // Send Occupancy Report
            // Detector 0
            bidib.sendOccupancySingle(0, currentState);
        }
    }
}
