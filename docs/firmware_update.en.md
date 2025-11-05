# BiDiB - Firmware Update

This document describes the firmware update process in the BiDiB protocol.

## 1. General

Nodes can have the ability to update their firmware. This is indicated by the `FEATURE_FW_UPDATE_MODE` feature.

## 2. Firmware Update Process

If `FEATURE_FW_UPDATE_MODE = 1`, an update typically proceeds as follows:

1.  **Enter Update Mode:** The host sends `MSG_FW_UPDATE_OP` with the parameter `BIDIB_MSG_FW_UPDATE_OP_ENTER` to the node. The message includes the node's Unique ID for security. The node enters a restricted mode where only firmware update commands are accepted.
2.  **Signal Readiness:** The node responds with a `Ready` message (`MSG_FW_UPDATE_STAT`) to indicate that it is ready to receive the firmware.
3.  **Set Destination:** The host sends a message to specify the destination memory (e.g., Flash) for the following data.
4.  **Acknowledge Reception:** The node acknowledges the command and signals that it is expecting the data.
5.  **Transmit Data:** The host sends the firmware file (typically in Intel Hex format) line by line to the node.
6.  **Acknowledge Each Line:** The node acknowledges the receipt of each line. The host must wait for the acknowledgment before sending the next line.
7.  **Complete Transmission:** The host sends a message to signal the end of the transmission.
8.  **Ready for More Packets:** The node sends a `Ready` message and is ready for another firmware packet.
9.  **Exit Update Mode:** At the end of the update, the host sends a command to restart (`BIDIB_MSG_FW_UPDATE_OP_EXIT`). The node acknowledges, waits for one second, restarts, and re-registers on the bus.

## 3. Features for FW Update

-   **FEATURE_FW_UPDATE_MODE (254):**
    -   `0`: No firmware update possible.
    -   `1`: Firmware update using the Intel Hex format is possible.

## 4. Messages for FW Update

### 4.1. Downlink: Messages from Host to Node

-   **MSG_FW_UPDATE_OP:** Initiates a firmware update operation.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_ENTER` (0x00):** Instructs the node to enter update mode. Requires the 7-byte Unique ID of the node.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_EXIT` (0x01):** Instructs the node to exit update mode and restart.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_SETDEST` (0x02):** Sets the destination memory area (e.g., 0 for Flash, 1 for EEPROM).
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_DATA` (0x03):** Transmits one line of the firmware file.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_DONE` (0x04):** Signals the end of the data transmission for the current destination memory.

### 4.2. Uplink: Messages from Node to Host

-   **MSG_FW_UPDATE_STAT:** Status message from the node in response to `MSG_FW_UPDATE_OP`.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_READY` (0x00):** Node is in update mode and ready.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_EXIT` (0x01):** Node is exiting update mode.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_DATA` (0x02):** Node is expecting (more) data.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_ERROR` (255):** An error has occurred. The second parameter contains a specific error code.
