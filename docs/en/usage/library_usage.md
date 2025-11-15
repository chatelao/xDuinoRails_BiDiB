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

## Controlling DCC Accessories

You can also control DCC accessories like turnouts or signals connected to an accessory decoder on the track.

```cpp
// This function will be called when an accessory ACK is received
void handleAccessoryAck(uint16_t address, uint8_t status) {
  Serial.print("Accessory ACK for address ");
  Serial.println(address);
}

void setup() {
  // ... (setup code as above) ...
  bidib.onAccessoryAck(handleAccessoryAck);
}

void loop() {
  // ... (update loop as above) ...

  if (bidib.isLoggedIn()) {
    // Switch turnout with address 10 to the 'on' state (e.g., thrown)
    bidib.accessory(10, 0, 1);
    delay(2000);

    // Switch it back to the 'off' state (e.g., straight)
    bidib.accessory(10, 0, 0);
    delay(2000);
  }
}
```

## Receiving Occupancy Feedback

The library can receive feedback from occupancy detectors and other sensors on the bus using callback functions.

```cpp
// Callback for single occupancy events
void handleOccupancy(uint8_t detectorNum, bool occupied) {
  Serial.print("Detector ");
  Serial.print(detectorNum);
  Serial.println(occupied ? " is occupied." : " is free.");
}

// Callback for locomotive address reports (e.g., from a Railcom detector)
void handleAddress(uint8_t detectorNum, uint16_t address) {
  Serial.print("Loco with address ");
  Serial.print(address);
  Serial.print(" detected at detector ");
  Serial.println(detectorNum);
}

void setup() {
  // ... (setup code as above) ...

  // Register the callback functions
  bidib.onOccupancy(handleOccupancy);
  bidib.onAddress(handleAddress);
  bidib.onSpeedUpdate(handleSpeed);
  bidib.onCvUpdate(handleCv);
}

// Callback for speed messages
void handleSpeed(uint16_t address, uint16_t speed) {
  Serial.print("Loco with address ");
  Serial.print(address);
  Serial.print(" reports speed: ");
  Serial.println(speed);
}

// Callback for CV reports
void handleCv(uint16_t address, uint16_t cv, uint8_t value) {
  Serial.print("Loco with address ");
  Serial.print(address);
  Serial.print(" reports CV ");
  Serial.print(cv);
  Serial.print(" = ");
  Serial.println(value);
}

void loop() {
  // The update loop will automatically trigger the callbacks when messages are received
  bidib.update();
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
}
```

## Managing Boosters

You can control and monitor BiDiB-enabled boosters.

```cpp
// Callback for booster status messages
void handleBoosterStatus(uint8_t status) {
  Serial.print("Booster status changed: ");
  Serial.println(status);
}

void setup() {
  // ... (setup code as above) ...
  bidib.onBoosterStatus(handleBoosterStatus);
}

void loop() {
  // ... (update loop as above) ...

  if (bidib.isLoggedIn()) {
    // Turn all boosters on (broadcast to address 0)
    bidib.setBoosterState(true, 0);
    delay(10000);

    // Turn all boosters off
    bidib.setBoosterState(false, 0);
    delay(10000);
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
-   `accessory(uint16_t address, uint8_t output, uint8_t state)`: Sends a command to a DCC accessory.
-   `pomWriteByte(uint16_t address, uint16_t cv, uint8_t value)`: Writes a CV value on the main track (PoM).
-   `setBoosterState(bool on, uint8_t node_addr)`: Turns a booster on or off.
-   `queryBooster(uint8_t node_addr)`: Requests the status of a booster.
-   `setAccessory(uint8_t accessoryNum, uint8_t aspect)`: Sets the state of a native BiDiB accessory.
-   `getAccessory(uint8_t accessoryNum)`: Requests the state of a native BiDiB accessory.

### Callback Registration Functions

-   `onDriveAck(callback)`: Registers a function to handle drive command acknowledgements.
-   `onAccessoryAck(callback)`: Registers a function to handle DCC accessory acknowledgements.
-   `onPomAck(callback)`: Registers a function to handle PoM write acknowledgements.
-   `onOccupancy(callback)`: Registers a function to handle occupancy reports (`occupied`/`free`).
-   `onAddress(callback)`: Registers a function to handle address reports from detectors.
-   `onAccessoryState(callback)`: Registers a function to handle state reports from native BiDiB accessories.
-   `onBoosterStatus(callback)`: Registers a function to handle booster status reports.
-   `onBoosterDiagnostic(callback)`: Registers a function to handle booster diagnostic reports.
-   `onSpeedUpdate(callback)`: Registers a function to handle speed reports from detectors.
-   `onCvUpdate(callback)`: Registers a function to handle CV reports from detectors.
-   `onFirmwareUpdateStatus(callback)`: Registers a function to handle firmware update status reports.
-   `onVendorAck(callback)`: Registers a function to handle vendor acknowledgements.
-   `onVendorData(callback)`: Registers a function to handle vendor data reports.

## Performing a Firmware Update

The library supports performing a firmware update for a node on the bus. This is an advanced use case.

```cpp
// This function is called to monitor the update status
void handleFwUpdateStatus(uint8_t status_code, uint8_t detail) {
  Serial.print("Firmware Update Status: ");
  Serial.print(status_code);
  Serial.print(" - Detail: ");
  Serial.println(detail);
}

void setup() {
  // ... (setup code as above) ...
  bidib.onFirmwareUpdateStatus(handleFwUpdateStatus);
}

void performFirmwareUpdate(uint8_t targetNodeAddress) {
  // Step 1: Start update mode for the target node
  bidib.enterFirmwareUpdateMode(targetNodeAddress);
  delay(100); // Wait for the node to be ready

  // Step 2: Send firmware data in chunks
  // (This is a simplified representation)
  const uint8_t firmware_data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
  bidib.sendFirmwareUpdateData(targetNodeAddress, firmware_data, sizeof(firmware_data));

  // Step 3: Finish the update
  bidib.signalFirmwareUpdateDone(targetNodeAddress);
  delay(100);
  bidib.exitFirmwareUpdateMode(targetNodeAddress);
}
```

## Vendor-Specific Configuration

BiDiB allows communication with vendor-specific configuration or command messages.

```cpp
// Callback for incoming vendor responses
void handleVendorData(uint8_t node_addr, const char* name, const char* value) {
  Serial.print("Vendor data from node ");
  Serial.print(node_addr);
  Serial.print(": ");
  Serial.print(name);
  Serial.print(" = ");
  Serial.println(value);
}

void setup() {
  // ... (setup code as above) ...
  bidib.onVendorData(handleVendorData);
}

void queryVendorSetting(uint8_t targetNode) {
  // Enable vendor mode
  bidib.vendorEnable(targetNode);
  delay(50);

  // Get a value
  bidib.vendorGet(targetNode, "brightness");
  delay(50);

  // Set a value
  bidib.vendorSet(targetNode, "mode", "auto");
  delay(50);

  // Disable vendor mode
  bidib.vendorDisable(targetNode);
}
```
