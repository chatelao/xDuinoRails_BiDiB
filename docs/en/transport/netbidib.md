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

## 5. Example: Typical Startup Sequence (Paired)

1.  **Discovery:** Client finds server via DNS-SD.
2.  **TCP Connection:** Client establishes a TCP connection to the server.
3.  **Identification:** Both sides send their descriptors (UID, name, etc.) via `MSG_LOCAL_LINK`.
    -   Host (Client) → Node (Server): `MSG_LOCAL_LINK DESCRIPTOR_UID ...`
    -   Node (Server) → Host (Client): `MSG_LOCAL_LINK DESCRIPTOR_UID ...`
4.  **Pairing Confirmation:** Since both sides already trust each other, they confirm the `paired` status.
    -   Host → Node: `MSG_LOCAL_LINK STATUS_PAIRED ...`
    -   Node → Host: `MSG_LOCAL_LINK STATUS_PAIRED ...`
5.  **Logon:** The node logs on to the host.
    -   Node → Host: `MSG_LOCAL_LOGON`
6.  **Logon Acknowledgment:** The host accepts the logon.
    -   Host → Node: `MSG_LOCAL_LOGON_ACK`
7.  **Protocol Start:** The host begins the normal BiDiB protocol startup.
    -   Host → Node: `MSG_SYS_GET_MAGIC`
