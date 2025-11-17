#include <BiDiB.h>

// Define the LED pin
#ifdef LED_BUILTIN
  const int ledPin = LED_BUILTIN;
#else
  const int ledPin = 13; // Fallback for boards without LED_BUILTIN
#endif

// Create a BiDiB object
BiDiB bidib;

// Callback function to handle accessory messages
void accessoryCallback(uint8_t accessoryNum, uint8_t aspect) {
  // Check if the command is for our address (13) and port (2)
  if (accessoryNum == 13) {
    // Port 2 corresponds to aspect 1 (or output 1)
    if (aspect == 1) {
      // Toggle the LED
      digitalWrite(ledPin, !digitalRead(ledPin));

      // Print the received command to the serial monitor
      Serial.print("Received accessory command for address ");
      Serial.print(accessoryNum);
      Serial.print(", aspect ");
      Serial.println(aspect);
    }
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Initialize the BiDiB library with the serial port
  bidib.begin(Serial);

  // Register the accessory callback function
  bidib.onAccessoryState(accessoryCallback);
}

void loop() {
  // Process incoming BiDiB messages
  bidib.update();

  // Handle any complete messages
  bidib.handleMessages();
}
