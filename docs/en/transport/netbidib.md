# BiDiB - netBiDiB

This document describes netBiDiB, the embedding of the BiDiB protocol into the TCP/IP stack for communication over standard networks (Ethernet, Wi-Fi).

## 1. Architecture

-   **Client-Server Model:** Communication is connection-oriented (TCP).
    -   **Server:** Accepts incoming connections (typically a BiDiB node/hardware interface).
    -   **Client:** Actively establishes connections to servers (typically a host program).
-   **Port:** The default port is 62875.

## 2. Connection Process

A netBiDiB connection is established in four phases:

1.  **Discovery (Optional):** A client finds available netBiDiB servers on the local network. This is preferably done using standard mechanisms like DNS-SD (Bonjour, mDNS) with the service name `_bidib._tcp`.
2.  **Link Setup:** After opening the TCP connection, both participants exchange their "descriptors" (Unique ID, product name, username) to identify each other.
3.  **Pairing:** A one-time process where the user confirms on both devices that the connection is trusted. Paired devices will connect automatically in the future. Pairing is initiated and confirmed by exchanging `PAIRING_REQUEST` messages.
4.  **Control (Logon):** On a paired link, a node can log on to the interface (`MSG_LOCAL_LOGON`) to establish a control relationship. Only then are regular BiDiB messages exchanged.

## 3. Protocol on TCP

-   **Data Stream:** The TCP data stream consists of a sequence of BiDiB messages.
-   **Signature:** The very first message in a stream must be a `MSG_LOCAL_PROTOCOL_SIGNATURE` with the prefix "BiDiB" to validate the connection as a netBiDiB stream.
-   **Keepalive:** TCP keepalive or regular `MSG_LOCAL_PROTOCOL_SIGNATURE` messages are used to detect connection losses.

## 4. Local Messages for netBiDiB

A series of `MSG_LOCAL_...` messages are used to manage the connection state:

-   **`MSG_LOCAL_LINK`:** Used for exchanging descriptors, initiating and confirming pairing, and status queries.
-   **`MSG_LOCAL_LOGON`:** A node requests to establish a control relationship.
-   **`MSG_LOCAL_LOGON_ACK`:** The interface accepts the logon.
-   **`MSG_LOCAL_LOGON_REJECTED`:** The interface rejects the logon or terminates an existing control relationship.
-   **`MSG_LOCAL_LOGOFF`:** A node actively logs off.
-   **`MSG_LOCAL_SYNC`:** Transmits the system time, synchronized via NTP and a UTC timestamp.
