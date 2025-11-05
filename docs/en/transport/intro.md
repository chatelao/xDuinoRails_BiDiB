# BiDiB - Transport

This document describes the various physical transmission media for the BiDiB protocol.

## Overview

BiDiB is designed to be transported over various media. The respective transport layer is responsible for the correct framing of messages and securing the data transmission (e.g., using CRC).

The main transport protocols are:

-   **[Serial Connection](./serial.md):** A point-to-point connection, typically via RS232 or a virtual COM port (USB). Suitable for the direct connection between a computer and the BiDiB interface.
-   **[BiDiBus](./bidibus.md):** A robust fieldbus based on RS485, specially developed for wiring within the model railway layout. It allows for high speeds and the layout-wide distribution of the DCC signal.
-   **[netBiDiB (Network)](./netbidib.md):** A packet-oriented network connection based on TCP/IP. It is ideal for connecting computers to each other or for integrating mobile devices (smartphones, tablets) into the control system.

These transmission media can be combined as desired. The routing of messages is ensured by the unique identifier of each module (Unique ID).
