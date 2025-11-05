# BiDiB - Serial Connection

This document describes the transport of BiDiB messages over a serial interface (RS232 or virtual COM port via USB).

## 1. Transmission Parameters

-   **Baud Rate:** 19200, 115200, or 1 Mbaud (preferred). The correct rate is determined by the host during connection setup by querying the system ID (`MSG_SYS_GET_MAGIC`).
-   **Parameters:** 8 data bits, 1 stop bit, no parity (8N1).
-   **Flow Control:** CTS and RTS are used.

## 2. Framing and Data Integrity

A serial packet is structured as follows:
`[MAGIC] MESSAGE_SEQ CRC MAGIC`

-   **`MAGIC` (0xFE):** A special byte that marks the beginning and end of a packet. The `MAGIC` byte at the end of one packet can also serve as the start of the next one.
-   **`MESSAGE_SEQ`:** A sequence of one or more complete BiDiB messages.
-   **`CRC` (CRC8):** A checksum calculated over the `MESSAGE_SEQ` to ensure data integrity.

### Escaping

To prevent data bytes within the `MESSAGE_SEQ` or the `CRC` from being misinterpreted as `MAGIC` (0xFE) or `ESCAPE` (0xFD), an escape mechanism is used:
-   A `MAGIC` byte in the data stream is replaced by the sequence `0xFD 0xDE` (`0xDE = 0xFE ^ 0x20`).
-   An `ESCAPE` byte in the data stream is replaced by the sequence `0xFD 0xDD` (`0xDD = 0xFD ^ 0x20`).

## 3. Connection Setup and Control

The connection between the host and the interface is a point-to-point connection.

-   **Connection Setup:** The host can initiate the connection directly with `MSG_SYS_GET_MAGIC`.
-   **Connection Loss:** If a node does not receive a message from the host for more than 5 seconds, it assumes a connection loss. The host should therefore send a message (e.g., `MSG_LOCAL_PING`) at least every 4 seconds.
-   **Logon/Logoff:** The local messages `MSG_LOCAL_LOGON`, `MSG_LOCAL_LOGON_ACK`, `MSG_LOCAL_LOGON_REJECTED`, and `MSG_LOCAL_LOGOFF` are used to manage the control relationship between the host (interface) and the node.

## 4. Local Messages

-   **`MSG_LOCAL_PING` / `MSG_LOCAL_PONG`:** Used to maintain the connection when no other data is being sent.
-   **`MSG_LOCAL_SYNC`:** Used to transmit the system time from the host to the node to synchronize time-based measurements.
