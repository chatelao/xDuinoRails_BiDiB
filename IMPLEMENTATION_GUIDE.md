# BiDiB Arduino Library - Implementation Guide

This document provides a guide for developers who want to contribute to the BiDiB Arduino Library. It covers the project's architecture, build and test procedures, and the process for adding new features.

## 1. Project Architecture

The library is designed to be modular and extensible. The core components are:

-   **`BiDiB.h` / `BiDiB.cpp`**: This is the main class that encapsulates the entire BiDiB protocol logic. It handles message serialization/deserialization, CRC checks, communication with the `Stream` object, and manages the node's state.
-   **`Stream` Class**: The library uses the standard Arduino `Stream` class for communication, making it compatible with any hardware or software serial port (`HardwareSerial`, `SoftwareSerial`, etc.).
-   **Callback Mechanism**: To handle asynchronous incoming messages (like feedback or acknowledgements), the library uses a callback system. User code can register callback functions (e.g., `onOccupancy()`, `onDriveAck()`) that are automatically invoked by `handleMessages()` when a relevant message is received.
-   **Data Structures**: `BiDiB.h` defines several `struct`s to represent BiDiB concepts like messages (`BiDiBMessage`), nodes (`BiDiBNode`), and features (`BiDiBFeature`).

### Message Flow

1.  **Receiving**: The `update()` method reads incoming bytes from the `Stream` and buffers them. The `receiveMessage()` private method is responsible for parsing the byte stream, finding `MAGIC` bytes, performing CRC validation, and assembling a complete `BiDiBMessage`.
2.  **Handling**: When `messageAvailable()` is true, the user calls `handleMessages()`. This method contains a large `switch` statement that inspects the `msg_type` of the incoming message and executes the appropriate logic, which often involves calling a registered callback function.
3.  **Sending**: Public methods like `drive()`, `setTrackState()`, or `sendOccupancySingle()` assemble a `BiDiBMessage` structure and pass it to the `sendMessage()` method. `sendMessage()` serializes the message into a byte stream, calculates the CRC, handles byte escaping, and writes the final data to the `Stream`.

## 2. Building and Testing

The project uses [PlatformIO](https://platformio.org/) for development and testing.

### Prerequisites

-   [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO IDE extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide).
-   Alternatively, the [PlatformIO Core CLI](https://docs.platformio.org/en/latest/core/index.html).

### Building the Library

You can build the library and its examples from the PlatformIO interface in VS Code or by using the CLI:

```bash
# Build the project for the 'uno' environment
platformio run -e uno
```

### Running Tests

The library includes a comprehensive suite of unit and end-to-end tests. The tests are located in the `test/` directory.

To run all tests in the native environment (recommended), use the following command:

```bash
# Run all test suites
platformio test -e native
```

To run a specific test suite, you can use the `-f` (filter) flag. For example, to run only the occupancy tests:

```bash
# Run only the occupancy tests
platformio test -e native -f test_occupancy
```

The tests use the `ArduinoFake` library to mock Arduino framework functions, allowing the code to be compiled and tested on a native machine, which is significantly faster than testing on hardware.

## 3. How to Add a New Feature

This section provides a step-by-step guide for adding support for a new BiDiB message type. As an example, let's imagine we are adding a new feature: `MSG_SYS_PING` and its reply `MSG_SYS_PONG`.

### Step 1: Define Message Constants

First, add the new message type constants to `src/BiDiB.h` in the "BiDiB Message Types" section.

```cpp
// In src/BiDiB.h

// --- System Messages ---
// ... existing constants ...
const uint8_t MSG_SYS_PING = 15;
const uint8_t MSG_SYS_PONG = 0x8F;
```

### Step 2: Implement the Sending Function

Create a new public function in `BiDiB.h` and implement it in `BiDiB.cpp`. This function will be what the user calls to send the new message.

**In `src/BiDiB.h`:**

```cpp
// In the BiDiB class definition
public:
    // ... other public functions ...

    /// @brief Sends a PING message to a node.
    /// @param node_addr The address of the target node.
    /// @param payload A single byte payload for the ping.
    void ping(uint8_t node_addr, uint8_t payload);
```

**In `src/BiDiB.cpp`:**

```cpp
// ... other function implementations ...

void BiDiB::ping(uint8_t node_addr, uint8_t payload) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.msg_num = 0; // Or manage a sequence number
    msg.msg_type = MSG_SYS_PING;
    msg.data[0] = payload;
    msg.length = 4; // addr + num + type + data

    sendMessage(msg);
}
```

### Step 3: Handle the Incoming Reply

To process the `MSG_SYS_PONG` reply, you need to add a `case` to the `switch` statement in `BiDiB::handleMessages()`. Typically, this involves invoking a callback.

**First, define the callback type in `src/BiDiB.h`:**

```cpp
// In BiDiB.h, with other callback typedefs
/// @brief Callback function type for PONG replies.
/// @param node_addr The address of the node that replied.
/// @param payload The payload from the pong message.
typedef void (*PongCallback)(uint8_t node_addr, uint8_t payload);
```

**Next, add the callback member and registration function to the `BiDiB` class in `BiDiB.h`:**

```cpp
// In the BiDiB class definition
public:
    // ...
    void onPong(PongCallback callback);

protected:
    // ...
    PongCallback _pongCallback;
```

**Implement the registration function in `BiDiB.cpp`:**

```cpp
void BiDiB::onPong(PongCallback callback) {
    _pongCallback = callback;
}
```

**Finally, add the `case` to `BiDiB::handleMessages()` in `BiDiB.cpp`:**

```cpp
void BiDiB::handleMessages() {
    switch (_lastMessage.msg_type) {
        // ... existing cases ...

        case MSG_SYS_PONG: {
            if (_pongCallback) {
                // Assuming the pong message returns the original payload
                uint8_t payload = _lastMessage.data[0];
                _pongCallback(_lastMessage.address[0], payload);
            }
            break;
        }

        // ... other cases ...
    }
}
```

### Step 4: Write Tests

Per the `AGENTS.md` guidelines, every new feature requires tests. You should create a new test suite in the `test/` directory.

1.  Create a new folder: `test/test_ping/`
2.  Create a test file: `test/test_ping/test_main.cpp`
3.  Add a new environment to `platformio.ini`:

    ```ini
    [env:test_ping]
    platform = native
    test_filter = test_ping
    build_src = yes
    lib_deps =
        fabiobatsilva/ArduinoFake@^0.4.0
    ```

4.  Write the tests in `test_main.cpp`. You should have at least two tests:
    -   **TX Test**: A test to verify that calling `bidib.ping()` sends the correctly formatted byte sequence to the stream.
    -   **RX Test**: A test to verify that when the stream receives a `MSG_SYS_PONG` byte sequence, the `onPong` callback is correctly invoked with the right parameters.

Refer to the existing test files (e.g., `test/test_occupancy/test_main.cpp`) for examples of how to use `MockStream` and `ArduinoFake` to write these tests.

By following these steps, you can ensure that new features are added in a way that is consistent with the existing codebase and maintains a high standard of quality.
