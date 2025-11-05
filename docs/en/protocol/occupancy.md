# BiDiB - Occupancy

This document describes occupancy detection and the evaluation of BiDi messages (Railcom) in the BiDiB protocol.

## 1. Occupancy Detection

Occupancy detection is used to determine the location of vehicles, typically through current sensing. Nodes that offer this function have the 'Detector Function' Class ID flag set.

### Security
Due to the importance of occupancy information for safe operation, BiDiB offers several security mechanisms:
-   **CRC and Sequence Numbers:** Secure the transmission.
-   **Secure-ACK:** An extended acknowledgment procedure where the host mirrors received messages back to the detector. In case of discrepancies, the detector re-transmits the correct data.
-   **'Confidence' Control:** The detector can report the "quality" of its detection (e.g., if it is disturbed by a short circuit).
-   **'Alive' Control:** The interface monitors the connection to the detectors.

## 2. Features for Occupancy Detectors

-   **FEATURE_BM_SIZE (0):** Number of detector bits of the node.
-   **FEATURE_BM_ON (1):** Enables/disables the spontaneous transmission of occupancy messages.
-   **FEATURE_BM_SECACK_AVAILABLE (2):** Indicates whether the detector supports the Secure-ACK procedure.
-   **FEATURE_BM_SECACK_ON (3):** Enables the Secure-ACK procedure.
-   **FEATURE_BM_CURMEAS_AVAILABLE (4):** Indicates whether the detector can provide current measurement data.
-   **FEATURE_BM_CURMEAS_INTERVAL (5):** Enables the transmission of current measurement data and sets the interval.

## 3. Messages for Occupancy Detectors

### 3.1. Downlink: Messages from Host to Node

-   **MSG_BM_GET_RANGE:** Queries the states of all detectors in a specific range.
-   **MSG_BM_MIRROR_OCC / MSG_BM_MIRROR_FREE / MSG_BM_MIRROR_MULTIPLE:** Used for the Secure-ACK procedure to mirror received states back to the detector.
-   **MSG_BM_GET_CONFIDENCE:** Queries the "confidence" of the current occupancy detection.

### 3.2. Uplink: Messages from Node to Host

-   **MSG_BM_OCC:** Reports that a single detector is occupied.
-   **MSG_BM_FREE:** Reports that a single detector is free.
-   **MSG_BM_MULTIPLE:** Transmits the states of a whole range of detectors.
-   **MSG_BM_CONFIDENCE:** Transmits the "confidence" of the detection (valid, frozen, no track signal).
-   **MSG_BM_CURRENT:** Transmits the current consumption of a section.

## 4. BiDi Detectors (Railcom)

Nodes can also evaluate bidirectional messages from vehicles (Railcom).

### 4.1. Features for BiDi Detectors

-   **FEATURE_BM_ADDR_DETECT_AVAILABLE (8):** Indicates whether the detector can recognize addresses.
-   **FEATURE_BM_ADDR_DETECT_ON (9):** Enables the transmission of address data.
-   **FEATURE_BM_ADDR_AND_DIR (10):** Indicates whether the detector can recognize the track orientation.
-   **FEATURE_BM_ISTSPEED_AVAILABLE (11):** Indicates whether the detector can forward speed messages (km/h).
-   **FEATURE_BM_CV_AVAILABLE (13):** Indicates whether the detector can recognize CV responses (PoM).

### 4.2. Messages for BiDi Detectors

#### Downlink
-   **MSG_BM_ADDR_GET_RANGE:** Requests the re-transmission of the detected locomotive addresses in a range.
-   **MSG_BM_MIRROR_POSITION:** Used for Secure-ACK with position messages.

#### Uplink
-   **MSG_BM_ADDRESS:** Reports the appearance of one or more locomotive addresses in a section.
-   **MSG_BM_CV / MSG_BM_XPOM:** Forwards CV responses from a decoder (from a PoM operation).
-   **MSG_BM_SPEED:** Forwards the speed (km/h) reported by a decoder.
-   **MSG_BM_DYN_STATE:** Transmits dynamic state data of a decoder (e.g., signal quality, temperature).
-   **MSG_BM_RCPLUS:** Reports feedback from decoders during the RailcomPlus registration process.
-   **MSG_BM_POSITION:** Reports the position transmitted by a decoder (e.g., from a location beacon).
