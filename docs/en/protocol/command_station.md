# BiDiB - Command Station

This document describes the control of command station devices in the BiDiB protocol.

> **Note on Library Implementation:**
> This Arduino library implements a subset of the features described below.
>
> **Implemented Features:**
> - `MSG_CS_SET_STATE`: Control of the track power (`setTrackState()`).
> - `MSG_CS_DRIVE`: Sending drive and function commands (`drive()`).
> - `MSG_CS_STATE`: The library processes this message to know the current track state.
>
> **Features Not Yet Implemented:**
> - Acknowledgment messages (`MSG_CS_DRIVE_ACK`, `MSG_CS_ACCESSORY_ACK`).
> - Accessory control (`MSG_CS_ACCESSORY`).
> - Programming on Main (`MSG_CS_POM`).

## 1. General

Command stations are nodes that can generate a DCC signal. BiDiB allows the parallel operation of multiple DCC systems, e.g., for a separate programming track.

### Watchdog

To prevent the uncontrolled operation of trains in the event of a connection loss to the host, a watchdog function is available. The host must periodically renew the `GO` state (`MSG_CS_SET_STATE`). If this fails to happen, all locomotives are stopped. The interval is defined by `FEATURE_GEN_WATCHDOG`.

## 2. Features for Command Station

-   **FEATURE_GEN_SPYMODE (100):** Allows "eavesdropping" on local manual controller commands.
-   **FEATURE_GEN_WATCHDOG (101):** Defines the interval for host monitoring.
-   **FEATURE_GEN_DRIVE_ACK (102):** Activates multi-level acknowledgments for drive commands.
-   **FEATURE_GEN_SWITCH_ACK (103):** Activates multi-level acknowledgments for accessory commands.
-   **FEATURE_GEN_POM_REPEAT (106):** Number of repetitions for POM commands.
-   **FEATURE_GEN_DRIVE_BUS (107):** Controls whether the node drives or receives the DCC bus.
-   **FEATURE_GEN_LOK_LOST_DETECT (108):** Activates the detection of "lost" locomotives.
-   **FEATURE_GEN_NOTIFY_DRIVE_MANUAL (109):** Reports locally operated locomotives/accessories.
-   **FEATURE_GEN_START_STATE (110):** State of the command station after power-on (On/Off).
-   **FEATURE_GEN_EXT_AVAILABLE (111):** Bitfield for additionally supported protocols (RailcomPlus, M4, DCCA, etc.).

## 3. Messages for Command Station

### 3.1. Downlink: Messages from Host to Node

-   **MSG_CS_ALLOCATE:** Locks the node for commands from other local addresses (valid for 2s).
-   **MSG_CS_SET_STATE:** Sets the state of the command station (OFF, STOP, GO, PROG, etc.).
-   **MSG_CS_DRIVE:** Sends drive commands (address, format, speed, functions F0-F28).
    -   **Speed Encoding:** The speed is always transmitted as a 7-bit value (0-127) plus a direction bit, analogous to the DCC128 format. The conversion to other formats (e.g., DCC28) is done by the node.
        -   `0`: Stop
        -   `1`: Emergency Stop
        -   `2-127`: Speed steps
-   **MSG_CS_ACCESSORY:** Controls accessory decoders via the DCC track signal.
-   **MSG_CS_POM:** Sends programming commands for the main track (Program on Main).
-   **MSG_CS_BIN_STATE:** Triggers individual actions on a vehicle decoder (e.g., coupler).
-   **MSG_CS_QUERY:** Queries the state of active vehicles.
-   **MSG_CS_PROG:** Sends programming commands for the programming track.
-   **MSG_CS_RCPLUS:** Controls the Railcom-Plus output (optional).

### 3.2. Uplink: Messages from Node to Host

A multi-level acknowledgment system exists:
-   **Level 0:** Message received by the node.
-   **Level 1:** Command has been sent to the track.
-   **Level 2:** Decoder has confirmed the command via ACK (through BiDi).

-   **MSG_CS_STATE:** Reports the current state of the command station.
-   **MSG_CS_DRIVE_ACK:** Acknowledges a drive command.
-   **MSG_CS_ACCESSORY_ACK:** Acknowledges an accessory command.
-   **MSG_CS_POM_ACK:** Acknowledges a POM command.
-   **MSG_CS_DRIVE_MANUAL:** Reports a manual operation of a locomotive.
-   **MSG_CS_DRIVE_EVENT:** Reports events for a locomotive (e.g., `LOST`).
-   **MSG_CS_ACCESSORY_MANUAL:** Reports a manual adjustment of a DCC accessory.
-   **MSG_CS_DRIVE_STATE:** Response to `MSG_CS_QUERY` with the state of a vehicle.
-   **MSG_CS_PROG_STATE:** Reports the result of a programming operation on the programming track.
-   **MSG_CS_RCPLUS_ACK:** Acknowledges a Railcom-Plus command (optional).
