# BiDiB Arduino Library

This is a comprehensive Arduino library for the BiDiB protocol. BiDiB is a versatile, bidirectional communication protocol primarily used for model railroad control systems. It allows for robust communication between a central controller (like a PC) and various nodes on the bus, such as feedback encoders, accessory decoders, and boosters.

This library provides a high-level interface to the BiDiB protocol, simplifying the development of BiDiB-compatible Arduino-based devices.

## Features

This library supports a wide range of BiDiB features, including:

- **Core Protocol:** Full implementation of the BiDiB message framing, escaping, and CRC8 checksum validation.
- **System & Node Management:**
  - Automatic node discovery and logon.
  - Feature management for querying and setting node capabilities.
  - System-level control (enable/disable).
- **Command Station (DCC Control):**
  - Control of track power (GO, STOP, OFF).
  - Driving locomotives (`MSG_CS_DRIVE`).
  - Switching accessories (`MSG_CS_ACCESSORY`).
  - Programming on the Main (PoM).
- **Feedback & Occupancy:**
  - Receiving and processing occupancy messages (`MSG_BM_OCC`, `MSG_BM_FREE`, `MSG_BM_MULTIPLE`).
  - Support for Secure-ACK for guaranteed message delivery.
  - Address feedback for Railcom-like functionality.
- **Native BiDiB Accessories:**
  - Direct control of BiDiB-native accessory nodes.
- **Booster Management:**
  - Control and monitoring of BiDiB-compatible boosters.
  - Reading diagnostic information (current, voltage, temperature).
- **Firmware Updates:**
  - A complete implementation of the BiDiB firmware update protocol.
- **Vendor-Specific Messages:**
  - Support for custom, vendor-defined messages.

## Getting Started

### Prerequisites

- An Arduino-compatible board.
- A BiDiB-compatible interface (e.g., a serial connection to a BiDiB master).
- The Arduino IDE or PlatformIO.

### Installation

1.  **Download:** Download the latest release of this library from the [GitHub repository](https://github.com/your-repo/bidib-arduino-library).
2.  **Install:** In the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...` and select the downloaded file.

### Basic Usage

Here is a simple example of how to use the library to create a basic BiDiB node that reports occupancy feedback.

```cpp
#include <Arduino.h>
#include <BiDiB.h>

// Create a BiDiB instance
BiDiB bidib;

// --- Callback function for occupancy events ---
// This function will be called automatically when the master sends an occupancy query
// or when another node reports its state.
void handleOccupancy(uint8_t detectorNum, bool occupied) {
  Serial.print("Occupancy event: Detector ");
  Serial.print(detectorNum);
  Serial.print(" is now ");
  Serial.println(occupied ? "occupied" : "free");
}

void setup() {
  // Start the serial communication with your PC for debugging
  Serial.begin(115200);
  Serial.println("BiDiB Occupancy Feedback Node");

  // --- Initialize BiDiB ---
  // Use Serial1 for the BiDiB bus communication.
  // Make sure to connect your BiDiB interface to the RX1/TX1 pins.
  Serial1.begin(115200);
  bidib.begin(Serial1);

  // --- Register Callbacks ---
  // Tell the library which function to call when an occupancy message is received.
  bidib.onOccupancy(handleOccupancy);

  // --- Set Node Features ---
  // This node supports occupancy feedback, so we need to set the relevant feature.
  // For this example, we'll imagine our node has 8 occupancy detectors.
  bidib.setFeature(FEATURE_BM_NUMBER, 8); // Number of occupancy detectors

  // --- Logon to the Bus ---
  // This will start the communication with the BiDiB master.
  bidib.logon();
}

void loop() {
  // The update() function must be called in the main loop.
  // It handles all incoming communication and processes messages.
  bidib.update();

  // If a complete message has been received, handle it.
  // This will trigger the registered callback functions.
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }

  // --- Example: Sending Feedback ---
  // In a real application, you would read your hardware sensors here.
  // For this example, we'll just send a simulated event every 5 seconds.
  static unsigned long lastSendTime = 0;
  static bool isOccupied = false;
  if (millis() - lastSendTime > 5000) {
    lastSendTime = millis();
    isOccupied = !isOccupied; // Toggle the state

    Serial.print("Sending feedback: Detector 1 is ");
    Serial.println(isOccupied ? "occupied" : "free");

    // Send the occupancy state for detector number 1
    bidib.sendOccupancySingle(1, isOccupied);
  }
}
```

## Contributing

Contributions are welcome! If you find a bug or want to add a new feature, please open an issue or submit a pull request. Before contributing, please read the `implementation_guide.md` and `AGENTS.md` files for guidelines on the development process.
