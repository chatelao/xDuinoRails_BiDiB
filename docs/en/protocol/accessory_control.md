# BiDiB - Accessory Control

This document describes the control of accessory devices in the BiDiB protocol.

## 1. General

BiDiB distinguishes between `Accessory` functions (for safe train operation, e.g., turnouts, signals) and `Switching Applications` (for secondary effects, e.g., lighting). This document covers `Accessory` functions.

Accessory objects have a state, the so-called "aspect", e.g., "straight" or "diverging" for a turnout. The state is controlled by the host. A switching operation can take time; the node reports the start and end of the process. Unforeseen state changes (e.g., due to manual adjustment) are reported spontaneously (`MSG_ACCESSory_NOTIFY`).

## 2. Features for Accessory Functions

-   **FEATURE_ACCESSORY_COUNT (40):** Number of controllable objects (turnouts, signals, etc.) on the node.
-   **FEATURE_ACCESSORY_SURVEILLED (41):** Indicates whether the node reports spontaneous state changes (e.g., due to manual adjustment).
-   **FEATURE_ACCESSORY_MACROMAPPED (42):** Indicates whether aspects can be mapped to macros.

## 3. Messages for Accessory Functions

### 3.1. Downlink: Messages from Host to Node

-   **MSG_ACCESSORY_SET:** Sets an accessory object to a specific aspect.
    -   Parameters: `ANUM` (object number), `ASPECT`.
-   **MSG_ACCESSORY_GET:** Queries the current state of an object.
    -   Parameter: `ANUM`.
-   **MSG_ACCESSORY_GETALL:** Queries the state of all objects on the node.
-   **MSG_ACCESSORY_PARA_SET:** Sets configuration parameters of an object (optional).
    -   Parameters: `ANUM`, `PARA_NUM`, `DATA`.
-   **MSG_ACCESSORY_PARA_GET:** Queries configuration parameters of an object (optional).
    -   Parameters: `ANUM`, `PARA_NUM`.

### 3.2. Uplink: Messages from Node to Host

-   **MSG_ACCESSORY_STATE:** Reports the state of an object. This is sent in response to a command or after the completion of a switching operation.
    -   Parameters: `ANUM`, `ASPECT`, `TOTAL` (number of aspects), `EXECUTE` (execution status), `WAIT` (remaining time or error code).
-   **MSG_ACCESSORY_NOTIFY:** Reports a spontaneous state change (e.g., due to manual adjustment or error). The parameters are identical to `MSG_ACCESSORY_STATE`.
-   **MSG_ACCESSORY_PARA:** Response to `MSG_ACCESSORY_PARA_GET` or `MSG_ACCESSORY_PARA_SET` (optional).
