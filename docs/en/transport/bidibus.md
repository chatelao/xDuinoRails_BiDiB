# BiDiB - BiDiBus

This document describes BiDiBus, a robust high-speed bus for model railway control based on RS485.

## 1. Features

-   **High Speed:** 500 kbaud.
-   **Secure Transmission:** Resistant to interference due to differential signal transmission (RS485) and CRC checking.
-   **Flexible Wiring:** Linear topology with short stubs, automatic address assignment (Plug & Play).
-   **Integrated Signals:** Transmits the DCC track signal and an ACK signal in addition to BiDiB data.
-   **Power Supply:** Can supply power to bus participants with low power consumption.

## 2. Electrical and Mechanical Parameters

-   **Cabling:** Standard network cable (CAT5) with RJ45 connectors.
-   **Bus Termination:** A 120 Ohm terminating resistor is required at both ends of the linear bus.
-   **Participants:** Up to 32 participants per bus segment.
-   **Pinout (RJ45):**
    -   1/2: VCC
    -   3/6: GND
    -   4/5: DATA (RS485 A/B)
    -   7/8: DCC (RS485 A/B)
    -   ACK: Open-collector line for DCC acknowledgments and emergency stop.

## 3. Bus Protocol

-   **Bus Access:** The interface (master) controls bus access using a token-passing procedure. It sends poll commands to the individual nodes to grant them permission to transmit.
-   **Message Format:** A BiDiBus packet (`BIDIBUS_PKT`) consists of:
    `P_LENGTH MESSAGE_SEQ CRC`
    -   `P_LENGTH`: The length of the `MESSAGE_SEQ` in bytes.
    -   `MESSAGE_SEQ`: A sequence of one or more BiDiB messages.
    -   `CRC`: A CRC8 checksum over `P_LENGTH` and `MESSAGE_SEQ`.
-   **Logon Process:**
    1.  The interface periodically sends a logon request (`BIDIBUS_LOGON`).
    2.  Nodes that are not yet logged on respond with their Unique ID. A collision avoidance mechanism (random delay) reduces the probability of simultaneous responses.
    3.  The interface confirms a successful logon with `MSG_LOCAL_LOGON_ACK` and assigns a dynamic `NODE_ADDR` to the node.

## 4. System Time

For system time synchronization, the interface regularly sends `MSG_LOCAL_SYNC` messages as a broadcast.

## 5. Example: Logon Message

A typical logon message from a node on the BiDiBus looks like this:

`0x0B 0x0A 0x00 0x00 0x8E 0x40 0x00 0x0D 0x67 0x00 0x01 0x00 0x4C`

-   `0x0B`: P_LENGTH (Packet size = 11 bytes)
-   `0x0A`: MSG_LENGTH (Message size = 10 bytes)
-   `0x00`: Local address (ignored by the interface)
-   `0x00`: MSG_NUM (0 for local messages)
-   `0x8E`: MSG_TYPE (`MSG_LOCAL_LOGON`)
-   `0x40 ... 0x00`: The 7 bytes of the node's Unique ID
-   `0x4C`: CRC8
