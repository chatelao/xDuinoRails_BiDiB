#include <Arduino.h>
#include <BiDiB_RS485.h>

// --- Configuration ---
const int RS485_DE_PIN = 4; // Direction control pin for the RS485 transceiver

// Create a BiDiB_RS485 instance
BiDiB_RS485 bidib(RS485_DE_PIN);

void setup() {
  // Start the serial communication with your PC for debugging
  Serial.begin(115200);
  Serial.println("BiDiB RS485 Node");

  // --- Initialize BiDiB ---
  // Use Serial1 for the BiDiB bus communication.
  // Make sure to connect your RS485 transceiver to the RX1/TX1 pins.
  Serial1.begin(500000); // BiDiBus runs at 500 kbaud
  bidib.begin(Serial1);

  // --- Set Node Features ---
  // This node supports occupancy feedback.
  // Let's say it has 8 detectors.
  // bidib.setFeature(FEATURE_BM_NUMBER, 8); // This needs to be a valid feature number from BiDiB.h

  // --- Logon to the Bus ---
  // This will queue a logon message to be sent when the master polls.
  bidib.logon();
}

void loop() {
  // The update() function must be called in the main loop.
  // It handles all incoming communication.
  bidib.update();

  // If a full message has been received by update(), handleMessages()
  // will process its content (e.g., trigger callbacks).
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }


  // --- Example: Sending Feedback ---
  // In a real application, you would read your hardware sensors here.
  // For this example, we'll just queue a simulated event every 5 seconds.
  static unsigned long lastSendTime = 0;
  static bool isOccupied = false;
  if (millis() - lastSendTime > 5000) {
    lastSendTime = millis();
    isOccupied = !isOccupied; // Toggle the state

    Serial.print("Queueing feedback: Detector 1 is ");
    Serial.println(isOccupied ? "occupied" : "free");

    // Queue the occupancy state for detector number 1
    bidib.sendOccupancySingle(1, isOccupied);
  }
}
