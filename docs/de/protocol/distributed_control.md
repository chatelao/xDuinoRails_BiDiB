# BiDiB - Verteilte Steuerung (Distributed Control)

Dieses Dokument beschreibt das Protokoll für die verteilte Steuerung, bei der mehrere Bediengeräte (Gäste) mit einem zentralen Host (Gastgeber) kommunizieren.

## 1. Allgemeines

Das Konzept der verteilten Steuerung erlaubt es verschiedenen Geräten wie Handreglern, Stellpulten oder auch anderen Software-Anwendungen, auf das BiDiB-System zuzugreifen, ohne die zentrale Kontrolle des Hosts zu untergraben.

-   **Gastgeber (Host):** Der zentrale Host behält die volle Kontrolle und Verantwortung. Er kann einen Gastzugang bereitstellen und Anfragen von Gästen annehmen oder ablehnen.
-   **Gast (Guest):** Ein Bediengerät oder eine Anwendung, die sich mit dem Gastgeber verbindet und Anfragen (Requests) sendet, um auf das System zuzugreifen.

## 2. Kontrolle und Adressierung

Ein Gast kommuniziert ausschließlich mit dem Gastgeber. Um auf einen bestimmten Knoten im System des Gastgebers zuzugreifen, kapselt der Gast eine normale BiDiB-Nachricht in eine `REQ_SEND`-Nachricht. Der Gastgeber leitet diese nach Prüfung der Berechtigungen an den Zielknoten weiter. Antworten vom Zielknoten werden vom Gastgeber in `RESP_NOTIFY`-Nachrichten verpackt und an den Gast gesendet.

Die Adressierung eines Zielknotens durch den Gast erfolgt über einen `TARGET_MODE`:
-   **`BIDIB_TARGET_MODE_UID` (0x00):** Adressierung über die Unique-ID (ohne Klassenbits).
-   **`BIDIB_TARGET_MODE_ALL` (0x01):** Adressiert alle Knoten (hauptsächlich für Abonnements).
-   **`BIDIB_TARGET_MODE_TOP` (0x02):** Adressiert den Wurzelknoten des Systems aus Sicht des Gastgebers.
-   **`BIDIB_TARGET_MODE_...` (0x08-0x0F):** Adressiert Knoten einer bestimmten Klasse (z.B. `_DCCGEN` für Gleissignalgeneratoren).

## 3. Nachrichten von Gästen an Gastgeber (Requests)

-   **MSG_GUEST_REQ_SUBSCRIBE:** Der Gast bittet darum, über bestimmte Nachrichten von/an einen Zielknoten informiert zu werden (Abonnement).
-   **MSG_GUEST_REQ_UNSUBSCRIBE:** Der Gast kündigt ein Abonnement.
-   **MSG_GUEST_REQ_SEND:** Der Gast sendet eine gekapselte BiDiB-Nachricht, die der Gastgeber an einen Zielknoten weiterleiten soll.

## 4. Nachrichten von Gastgebern an Gäste (Responses)

-   **MSG_GUEST_RESP_NOTIFY:** Der Gastgeber benachrichtigt den Gast über eine abonnierte Nachricht, die von einem Zielknoten empfangen oder an diesen gesendet wurde.
-   **MSG_GUEST_RESP_SENT:** Der Gastgeber bestätigt dem Gast den Empfang einer `REQ_SEND`-Nachricht und teilt mit, ob die Anfrage ausgeführt, abgelehnt oder die Antwort direkt (aus dem Cache) generiert wurde.
-   **MSG_GUEST_RESP_SUBSCRIPTION_COUNT:** Antwort auf eine (De-)Abonnement-Anfrage, die die Anzahl der betroffenen Knoten mitteilt.
-   **MSG_GUEST_RESP_SUBSCRIPTION:** Bestätigt oder ändert den Status eines einzelnen Abonnements für einen Knoten.
