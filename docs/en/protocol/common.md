# BiDiB - Common Protocol Parts

This document describes the common parts of the BiDiB protocol.

## 1. General

### 1.1. Objective, Special Properties

The BiDiB protocol is used to control a model railway. It allows the control of locomotives, turnouts, and accessories, as well as the reliable transmission of feedback information from the model railway layout to the controlling computer. BiDiB stands for BiDirectional Bus and offers the following advantages:
- Automatic hardware assignment, no programming required (Plug&Play).
- Largely free arrangement of modules.
- Easy scalability and expandability through hub modules.
- Extremely fast data transmission, secured by CRC.
- Monitoring of the entire transmission chain with Secure-ACK technology.
- Full support for BiDi-capable feedback detectors.
- Reporting of multiple locomotives in one section.
- Mixed operation of normal and BiDi-capable feedback detectors.
- Flexible, open protocol, future-proof.

### 1.2. Design Principles

- BiDiB is designed as a stateless protocol.
- Messages are idempotent.
- Communication errors are reported only once.
- Spontaneous events are not repeated.
- Backward compatibility is maintained in new revisions.

### 1.3. Glossary

- **BiDiB:** The protocol standard.
- **BiDiBus:** A possible physical implementation (RS485).
- **Bus system:** The entire setup.
- **Class:** Classification of nodes according to their properties.
- **Feature:** A specific property of a node.
- **Host:** The controlling computer.
- **Hub:** A node that serves as an interface for a subordinate level.
- **Interface:** Interface to the host or a higher-level node.
- **Node:** A participant in the bus system.
- **Logon:** Attempt by a participant to establish a logical connection.
- **Detector:** A node that detects track occupancy.
- **Node Address:** Number assigned to a node by the interface.
- **Magic:** System ID for detecting the baud rate.
- **Unique ID:** Globally unique identifier of a node.

### 1.4. Principles of Node Assignment, Addressing

- Tree-like organization.
- Automatic address assignment by the interface.
- Assignment table is created at startup.
- Changes in the structure are reported to the host.
- Addressing can be hierarchical across multiple levels (hubs). The address of a node is then represented as an address stack that describes the path from the interface to the target node.
  - `00`: The interface itself.
  - `04 00`: A node (hub) with the local address `04` on the first level.
  - `04 02 00`: A node with the local address `02` behind the hub `04`.

## 2. Protocol Description

### 2.1. Basic Message Structure

A message (MESSAGE) has the following structure:
`MESSAGE ::= MSG_LENGTH MSG_ADDR MSG_NUM MSG_TYPE DATA`

- **MSG_LENGTH:** Message size in bytes (starting from MSG_ADDR).
- **MSG_ADDR:** Destination or source address (stack of NODE_ADDR, ending with 0x00).
- **MSG_NUM:** Sequential message index (1-255). 0 for no sequence check.
- **MSG_TYPE:** Message type, encoding type and direction.
- **DATA:** Optional parameters.

### 2.2. Data Integrity and Framing

- **CRC Check:** Each message is secured by a CRC8 checksum. It is calculated over the entire message (from `MSG_LENGTH` to the end of `DATA`). The polynomial used is `x^8 + x^5 + x^4 + 1` (0x31).
- **Framing (Serial):** In serial transmission, each message is framed by `MAGIC` bytes (0xFE). To prevent data bytes from being misinterpreted as a `MAGIC` byte, an escape mechanism is used:
  - If a data byte has the value `0xFE` (MAGIC) or `0xFD` (ESCAPE), it is preceded by an `ESCAPE` byte (0xFD) and the original byte is XORed with `0x20`.

#### Example of a serial message (`MSG_SYS_GET_MAGIC`):
`0xFE 0x03 0x00 0x00 0x01 0xD6 0xFE`
- `0xFE`: Start-MAGIC
- `0x03`: MSG_LENGTH
- `0x00`: MSG_ADDR (Interface)
- `0x00`: MSG_NUM
- `0x01`: MSG_TYPE
- `0xD6`: CRC8
- `0xFE`: End-MAGIC

### 2.3. Unique ID

The Unique ID consists of 7 bytes:
- **Byte 1-2: ClassID:** Bitfield of the class membership.
- **Byte 3: Vendor ID:** Manufacturer ID (NMRA).
- **Byte 4-7: Product ID:** Product type and serial number.

### 2.3. Typical Protocol Start

1.  Establish a connection with the interface.
2.  Stop the BiDiB system with `MSG_SYS_DISABLE`.
3.  Read the properties of the first node (Magic, Version, Unique ID, Features, Node Table).
4.  Repeat step 3 for all other nodes.
5.  Enable the system for spontaneous messages with `MSG_SYS_ENABLE`.

#### Example of a Startup Sequence
A typical sequence of messages at startup looks like this:
1.  Host sends `MSG_SYS_GET_MAGIC` (to check the connection and baud rate).
2.  Node responds with `MSG_SYS_MAGIC`.
3.  Host sends `MSG_SYS_RESET`.
4.  Host queries basic information: `MSG_SYS_GET_P_VERSION`, `MSG_SYS_GET_UNIQUE_ID`, `MSG_SYS_GET_SW_VERSION`.
5.  Host queries the node table (`MSG_NODETAB_GETALL`) and then all features (`MSG_FEATURE_GETALL`).

## 3. System Messages

### 3.1. Downlink: System Messages

- **MSG_SYS_GET_MAGIC:** Request the system identifier.
- **MSG_SYS_GET_P_VERSION:** Request the protocol version.
- **MSG_SYS_ENABLE:** Allow spontaneous messages for specific classes.
- **MSG_SYS_DISABLE:** Prohibit spontaneous messages for specific classes.
- **MSG_SYS_GET_UNIQUE_ID:** Request the Unique ID and configuration fingerprint.
- **MSG_SYS_GET_SW_VERSION:** Request the software version.
- **MSG_SYS_PING:** Check if a node is alive.
- **MSG_SYS_IDENTIFY:** Switch the identification indicator (e.g., blinking LED) on/off.
- **MSG_SYS_GET_ERROR:** Read the last error.
- **MSG_SYS_RESET:** Reset the system and re-run the node assignment.
- **MSG_NODETAB_GETALL:** Request the complete node table.
- **MSG_NODETAB_GETNEXT:** Request the next entry in the node table.
- **MSG_NODE_CHANGED_ACK:** Acknowledge `MSG_NODE_NEW` or `MSG_NODE_LOST`.
- **MSG_SYS_CLOCK:** Transmit the layout model time.

### 3.2. Uplink: System Messages

- **MSG_SYS_MAGIC:** Response to `MSG_SYS_GET_MAGIC`. Contains the system identifier.
- **MSG_SYS_PONG:** Response to `MSG_SYS_PING`.
- **MSG_SYS_P_VERSION:** Response to `MSG_SYS_GET_P_VERSION`. Contains the protocol version.
- **MSG_SYS_UNIQUE_ID:** Response to `MSG_SYS_GET_UNIQUE_ID`. Contains the 7-byte Unique ID.
- **MSG_SYS_SW_VERSION:** Response to `MSG_SYS_GET_SW_VERSION`. Contains the software version.
- **MSG_SYS_IDENTIFY_STATE:** Reports the state of the identification indicator.
- **MSG_SYS_ERROR:** Error message from a node.
- **MSG_NODETAB_COUNT:** Response to `MSG_NODETAB_GETALL`. Contains the number of entries in the node table.
- **MSG_NODETAB:** Response to `MSG_NODETAB_GETNEXT`. Contains one entry of the node table.
- **MSG_NODE_NA:** Sent when a requested node does not exist.
- **MSG_NODE_LOST:** Reports that a node is no longer reachable.
- **MSG_NODE_NEW:** Reports that a new node has been detected.
- **MSG_STALL:** Reports that a node's output buffer is full.

## 4. Feature Messages

Features are used to query and configure node properties.

### 4.1. Downlink: Feature Messages

- **MSG_FEATURE_GETALL:** Start querying all feature values.
- **MSG_FEATURE_GETNEXT:** Query the next feature value.
- **MSG_FEATURE_GET:** Query a single feature.
- **MSG_FEATURE_SET:** Set a single feature.

### 4.2. Uplink: Feature Messages

- **MSG_FEATURE:** Response to a feature query. Contains the feature number and value.
- **MSG_FEATURE_NA:** Sent when a requested feature is not available.
- **MSG_FEATURE_COUNT:** Response to `MSG_FEATURE_GETALL`. Contains the number of features.

#### Example of Querying All Features
1.  **Host → Node:** `MSG_FEATURE_GETALL`
2.  **Node → Host:** `MSG_FEATURE_COUNT` (e.g., with value 3)
3.  **Host → Node:** `MSG_FEATURE_GETNEXT`
4.  **Node → Host:** `MSG_FEATURE` (for the first feature)
5.  **Host → Node:** `MSG_FEATURE_GETNEXT`
6.  **Node → Host:** `MSG_FEATURE` (for the second feature)
7.  **Host → Node:** `MSG_FEATURE_GETNEXT`
8.  **Node → Host:** `MSG_FEATURE` (for the third feature)
9.  **Host → Node:** `MSG_FEATURE_GETNEXT`
10. **Node → Host:** `MSG_FEATURE_NA` (with a special value of 255 to signal the end)

## 5. User Configuration Messages (Vendor-specific)

These messages are used to transmit manufacturer-specific parameters.

### 5.1. Downlink: User Configuration

- **MSG_VENDOR_ENABLE:** Activates user configuration mode for a node.
- **MSG_VENDOR_DISABLE:** Deactivates user configuration mode.
- **MSG_VENDOR_SET:** Sets a manufacturer-specific parameter.
- **MSG_VENDOR_GET:** Reads a manufacturer-specific parameter.
- **MSG_STRING_SET:** Sends a string to the node.
- **MSG_STRING_GET:** Requests a string from the node.

### 5.2. Uplink: User Configuration

- **MSG_VENDOR:** Response to `MSG_VENDOR_GET` or `MSG_VENDOR_SET`.
- **MSG_VENDOR_ACK:** Acknowledgment for `MSG_VENDOR_ENABLE` or `MSG_VENDOR_DISABLE`.
- **MSG_STRING:** Response to `MSG_STRING_GET` or `MSG_STRING_SET`.
