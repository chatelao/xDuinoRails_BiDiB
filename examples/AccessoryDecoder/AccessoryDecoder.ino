#include <Arduino.h>
#include <BiDiB.h>

// Define the LED pin
#ifdef LED_BUILTIN
  const int ledPin = LED_BUILTIN;
#else
  const int ledPin = 13; // Fallback for boards without LED_BUILTIN
#endif

// Create a BiDiB object
BiDiB bidib;

// Callback function to handle accessory commands (MSG_ACCESSORY_SET)
void accessoryCommandCallback(uint8_t accessoryNum, uint8_t aspect) {
  // Check if the command is for our address (e.g., accessory 0)
  // In BiDiB, accessory numbers are 0-based index on the node.
  // We'll control the LED with accessory 0.
  if (accessoryNum == 0) {
    if (aspect == 1) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }

    // Acknowledge by sending the new state back
    bidib.sendAccessoryState(accessoryNum, aspect);
  }
}

void setup() {
  Serial.begin(115200);

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Initialize the BiDiB library with the serial port
  bidib.begin(Serial);

  // Register the accessory command callback function
  bidib.onAccessoryCommand(accessoryCommandCallback);

  // Logon to the bus
  bidib.logon();
}

void loop() {
  bidib.update();
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
}
