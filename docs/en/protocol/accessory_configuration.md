# BiDiB - Accessory Configuration

This document describes the control and configuration of general switching functions (ports) and macros in the BiDiB protocol.

## 1. Control of Accessories / Switching Functions

Nodes with switching functions (e.g., for light, animations, sound) have the 'Switching' Class ID flag set. They have inputs and outputs that are addressed as 'ports'.

There are two addressing models for ports:
-   **Type-oriented Port Model:** Ports are permanently assigned to a type (e.g., switch, light, servo) and are numbered within that type.
-   **Flat Port Model:** All ports share a common address space. The type of a port can be changed via configuration.

### Port Types
- `SWITCH`: Switch output
- `LIGHT`: Light output (dimmable, flashing)
- `SERVO`: Servo output
- `SOUND`: Sound output
- `MOTOR`: Motor output
- `ANALOGOUT`: Analog output
- `BACKLIGHT`: Backlight output
- `SWITCHPAIR`: Two exclusively switched outputs
- `INPUT`: Contact input

## 2. Features for Switching Functions

-   **FEATURE_CTRL_INPUT_COUNT (50):** Number of inputs.
-   **FEATURE_CTRL_INPUT_NOTIFY (51):** Allows spontaneous reporting from inputs.
-   **FEATURE_CTRL_..._COUNT (52-57, 59):** Number of outputs of the respective type (SWITCH, LIGHT, SERVO, SOUND, MOTOR, ANALOGOUT, BACKLIGHT).
-   **FEATURE_CTRL_PORT_FLAT_MODEL (70):** Activates the flat port model and specifies the number of ports.
-   **FEATURE_CTRL_PORT_QUERY_AVAILABLE (66):** Indicates whether port states can be queried.

## 3. Messages for Switching Functions

### 3.1. Downlink: Messages from Host to Node

-   **MSG_LC_OUTPUT:** Directly controls an output port with a specific state.
-   **MSG_LC_PORT_QUERY:** Queries the state of a single port.
-   **MSG_LC_PORT_QUERY_ALL:** Queries the state of all or multiple ports.
-   **MSG_LC_CONFIGX_SET:** Configures a port using key-value pairs.
    -   **Example:** To configure the brightness of a light output (`LIGHT`), the host sends `MSG_LC_CONFIGX_SET` with the parameters for `BIDIB_PCFG_LEVEL_PORT_ON` and `BIDIB_PCFG_LEVEL_PORT_OFF`.
-   **MSG_LC_CONFIGX_GET:** Queries the configuration of a port.
-   **MSG_LC_CONFIGX_GET_ALL:** Queries the configuration of all or multiple ports.

### 3.2. Uplink: Messages from Node to Host

-   **MSG_LC_NA:** Reports an exception or error on a port.
-   **MSG_LC_STAT:** Transmits the state of a port (in response to a query or spontaneously for inputs).
-   **MSG_LC_WAIT:** Sent when an output operation takes longer than 100ms, and provides the estimated remaining time.
-   **MSG_LC_CONFIGX:** Response to `MSG_LC_CONFIGX_GET` or `MSG_LC_CONFIGX_SET`, contains the port's configuration parameters.

## 4. Local Macros

Macros combine switching functions into local sequences. A macro is a chain of actions executed on a node.

### 4.1. Features for Local Macros

-   **FEATURE_CTRL_MAC_LEVEL (60):** Supported macro level (0: none, 1: simple lists, 2: with queries, start/stop).
-   **FEATURE_CTRL_MAC_SAVE (61):** Number of permanent storage locations for macros.
-   **FEATURE_CTRL_MAC_COUNT (62):** Number of possible macros.
-   **FEATURE_CTRL_MAC_SIZE (63):** Maximum number of entries per macro list.

### 4.2. Messages for Local Macros

#### Downlink
-   **MSG_LC_MACRO_HANDLE:** Controls a macro (Start, Stop, Save, Delete).
-   **MSG_LC_MACRO_SET:** Defines a single step (action point) in a macro.
-   **MSG_LC_MACRO_GET:** Queries a macro step.
-   **MSG_LC_MACRO_PARA_SET:** Sets general parameters for a macro (e.g., speed, repetitions).
-   **MSG_LC_MACRO_PARA_GET:** Queries macro parameters.

#### Uplink
-   **MSG_LC_MACRO_STATE:** Response to `MSG_LC_MACRO_HANDLE`, reports the macro's state.
-   **MSG_LC_MACRO:** Response to `MSG_LC_MACRO_GET`, contains the data of the macro step.
-   **MSG_LC_MACRO_PARA:** Response to `MSG_LC_MACRO_PARA_GET`, contains the macro parameters.
