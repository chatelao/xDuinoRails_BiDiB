# BiDiB - Booster

This document describes the control and monitoring of boosters in the BiDiB protocol.

## 1. General

Boosters are used to amplify the DCC track signal. They can include diagnostic functions (current, voltage, temperature), control options (on/off), and a global Railcom detector.

Booster commands (`MSG_BOOST_ON`/`MSG_BOOST_OFF`) are typically sent by the host as a broadcast to all boosters to ensure synchronous behavior. Each booster reports its status change individually.

## 2. Features for Boosters

-   **FEATURE_BST_VOLT_ADJUSTABLE (15):** Indicates if the output voltage is adjustable.
-   **FEATURE_BST_VOLT (16):** The current value of the output voltage in volts.
-   **FEATURE_BST_CUTOUT_AVAILABLE (17):** Indicates if the booster can generate a Railcom cutout.
-   **FEATURE_BST_TURNOFF_TIME (19):** Shutdown time in case of a short circuit, in ms.
-   **FEATURE_BST_AMPERE_ADJUSTABLE (21):** Indicates if the maximum output current is adjustable.
-   **FEATURE_BST_AMPERE (22):** The maximum set output current.
-   **FEATURE_BST_CURMEAS_INTERVAL (23):** Enables the transmission of diagnostic values and sets the interval.
-   **FEATURE_BST_INHIBIT_AUTOSTART (26):** Prevents the booster from starting automatically when a DCC signal is present.
-   **FEATURE_BST_INHIBIT_LOCAL_ONOFF (27):** Determines whether local buttons (Go/Stop) take effect directly or are only reported to the host.

## 3. Messages for Boosters

### 3.1. Downlink: Messages from Host to Node

-   **MSG_BOOST_ON:** Turns the track voltage on. A parameter (`UNICAST`) controls whether the command is sent to all (broadcast) or only to the addressed booster.
    -   **Example Message (Broadcast):** `[MSG_BOOST_ON, 0x00]`
-   **MSG_BOOST_OFF:** Turns the track voltage off (also with a `UNICAST` parameter).
-   **MSG_BOOST_QUERY:** Queries the status and, if available, diagnostic values of the booster.

### 3.2. Uplink: Messages from Node to Host

-   **MSG_BOOST_STAT:** Reports the current status of the booster. This message is sent after every state change.
    -   `0x00`: Off (general)
    -   `0x01`: Off (due to short circuit)
    -   `0x02`: Off (due to overtemperature)
    -   `0x80`: On
    -   `0x81`: On (in current limit)
-   **MSG_BOOST_DIAGNOSTIC:** Transmits a list of diagnostic values as key-value pairs.
    -   `0x00`: Current value
    -   `0x01`: Voltage value
    -   `0x02`: Temperature value
