# BiDiB - Distributed Control

This document describes the protocol for distributed control, where multiple control devices (guests) communicate with a central host (host).

## 1. General

The concept of distributed control allows various devices such as handheld controllers, control panels, or other software applications to access the BiDiB system without undermining the central control of the host.

-   **Host:** The central host retains full control and responsibility. It can provide guest access and accept or reject requests from guests.
-   **Guest:** A control device or application that connects to the host and sends requests to access the system.

## 2. Control and Addressing

A guest communicates exclusively with the host. To access a specific node in the host's system, the guest encapsulates a normal BiDiB message in a `REQ_SEND` message. The host forwards this to the target node after checking permissions. Responses from the target node are packaged by the host into `RESP_NOTIFY` messages and sent to the guest.

The guest addresses a target node using a `TARGET_MODE`:
-   **`BIDIB_TARGET_MODE_UID` (0x00):** Addressing via the Unique ID (without class bits).
-   **`BIDIB_TARGET_MODE_ALL` (0x01):** Addresses all nodes (mainly for subscriptions).
-   **`BIDIB_TARGET_MODE_TOP` (0x02):** Addresses the root node of the system from the host's perspective.
-   **`BIDIB_TARGET_MODE_...` (0x08-0x0F):** Addresses nodes of a specific class (e.g., `_DCCGEN` for command stations).

## 3. Messages from Guests to Host (Requests)

-   **MSG_GUEST_REQ_SUBSCRIBE:** The guest requests to be informed about specific messages from/to a target node (subscription).
-   **MSG_GUEST_REQ_UNSUBSCRIBE:** The guest cancels a subscription.
-   **MSG_GUEST_REQ_SEND:** The guest sends an encapsulated BiDiB message that the host should forward to a target node.

## 4. Messages from Host to Guests (Responses)

-   **MSG_GUEST_RESP_NOTIFY:** The host notifies the guest about a subscribed message that was received from or sent to a target node.
-   **MSG_GUEST_RESP_SENT:** The host acknowledges receipt of a `REQ_SEND` message and informs the guest whether the request was executed, rejected, or if the response was generated directly (from cache).
-   **MSG_GUEST_RESP_SUBSCRIPTION_COUNT:** Response to a (un)subscription request, indicating the number of affected nodes.
-   **MSG_GUEST_RESP_SUBSCRIPTION:** Confirms or changes the status of a single subscription for a node.
