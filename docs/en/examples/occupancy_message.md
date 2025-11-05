# BiDiB - Example: Occupancy Report from Node to Host

This document describes the detailed path of an occupancy report message (`MSG_BM_OCC`) from a node (e.g., an occupancy detector) via the BiDiBus to the interface (host).

## Scenario

An occupancy detector with the local `NODE_ADDR` **4** on the BiDiBus sends its **12th** message (`MSG_NUM` = 12) to report that its detector with the number **5** (`MNUM` = 5) is now occupied.

---

### 1. Logical Message (Uplink)

The information to be transmitted is an occupancy report.

-   **Message Type:** `MSG_BM_OCC` (decimal 162, hexadecimal `0xA2`)
-   **Parameter:** Detector number `MNUM` = **5**

---

### 2. BiDiB Message (`MESSAGE`)

The node assembles the complete BiDiB message. Since the message originates from the node itself and is addressed to the parent interface, the address stack is empty (`MSG_ADDR` = `0x00`).

-   **`MSG_LENGTH`:** 4 (length starting from `MSG_ADDR`)
-   **`MSG_ADDR`:** `0x00` (message originates from the node itself)
-   **`MSG_NUM`:** 12
-   **`MSG_TYPE`:** `0xA2` (`MSG_BM_OCC`)
-   **`DATA`:** `0x05` (parameter `MNUM`)

**The `MESSAGE` sequence in bytes:**
`0x04 0x00 0x0C 0xA2 0x05`

---

### 3. BiDiBus Packet (`BIDIBUS_PKT`)

For transmission on the BiDiBus, the `MESSAGE` is wrapped in a `BIDIBUS_PKT`.

1.  **`P_LENGTH` (Packet Length):** The length of the `MESSAGE_SEQ`. Since only one message is being sent, this corresponds to the length of the `MESSAGE`, which is **5** bytes.
2.  **`MESSAGE_SEQ`:** The byte sequence created above.
3.  **`CRC8` (Checksum):** The CRC8 checksum is calculated over `P_LENGTH` and the `MESSAGE_SEQ`.
    -   Bytes to be calculated: `0x05 0x04 0x00 0x0C 0xA2 0x05`
    -   The result of the CRC8 calculation (polynomial `0x31`) is **`0x1B`**.

**The complete `BIDIBUS_PKT` in bytes:**
`0x05 0x04 0x00 0x0C 0xA2 0x05 0x1B`

---

### 4. Bit Level (Transmission on RS485)

The node waits until it is polled by the interface with its address (`NODE_ADDR` = 4). As soon as it receives permission to transmit, it sends the `BIDIBUS_PKT` as serial data (500 kbaud, 9N1) on the RS485 bus.

**The final byte sequence transmitted on the bus:**
`0x05 0x04 0x00 0x0C 0xA2 0x05 0x1B`

The interface receives this packet, verifies the CRC, adds the `NODE_ADDR` of the sending node (4) to the address stack, and forwards the message to the host.
