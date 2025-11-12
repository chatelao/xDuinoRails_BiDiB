# BiDiB Arduino Library - User Guide

This guide explains how to use the BiDiB Arduino Library to create your own BiDiB-enabled devices.

## Installation

1.  **Download the Library:** Download the latest version of the library from the [GitHub repository](https://github.com/your-repo-link).
2.  **Install in Arduino IDE:**
    *   Open the Arduino IDE.
    *   Go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
    *   Select the downloaded ZIP file.
    *   The library will be installed and available under the name "BiDiB".

## "Hello World": A Minimal BiDiB Node

This example shows how to create a minimal BiDiB node that can connect to a BiDiB bus and be recognized by a master.

```cpp
#include <Arduino.h>
#include <BiDiB.h>

// Create a BiDiB instance
BiDiB bidib;

void setup() {
  // Open the serial port for BiDiB communication
  Serial.begin(115200);

  // Initialize the BiDiB library with the serial port
  bidib.begin(Serial);

  // Set a unique ID for this node (replace with your own)
  uint8_t my_unique_id[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  memcpy(bidib.unique_id, my_unique_id, 7);

  // Set some features (optional)
  bidib.setFeature(BIDIB_FEATURE_FW_UPDATE_SUPPORT, 0); // Firmware update not supported
  bidib.setFeature(BIDIB_FEATURE_STRING_SIZE, 32);      // Max string size
}

void loop() {
  // The update() function must be called regularly to process incoming messages
  bidib.update();

  // If a message is available, handle it
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
}
```

## Controlling a Locomotive

This example demonstrates how to use the library as a command station to control a locomotive on the track.

```cpp
#include <Arduino.h>
#include <BiDiB.h>

BiDiB bidib;

void setup() {
  Serial.begin(115200);
  bidib.begin(Serial);

  // ... (unique ID and feature setup as above) ...
}

void loop() {
  bidib.update();

  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }

  // Example: Once logged in, turn on the track power and drive a loco
  if (bidib.isLoggedIn()) {
    // Turn on the track power
    bidib.setTrackState(BIDIB_CS_STATE_GO);

    // Drive locomotive with address 3 at half speed
    bidib.drive(3, 64, 0); // Address, speed (0-127), functions

    // Wait for 5 seconds
    delay(5000);

    // Stop the locomotive
    bidib.drive(3, 0, 0);

    // Wait for another 5 seconds
    delay(5000);
  }
}
```

## Key Functions

-   `begin(Stream &serial)`: Initializes the library with a serial interface.
-   `update()`: Reads and processes incoming data from the serial port. Call this in your main `loop()`.
-   `handleMessages()`: Interprets a complete, received message.
-   `isLoggedIn()`: Returns `true` if the node has successfully logged on to the BiDiB bus.
-   `setTrackState(uint8_t state)`: Sets the track power state (`BIDIB_CS_STATE_OFF`, `BIDIB_CS_STATE_STOP`, `BIDIB_CS_STATE_GO`).
-   `drive(uint16_t address, int8_t speed, uint8_t functions)`: Sends a drive command to a locomotive.
