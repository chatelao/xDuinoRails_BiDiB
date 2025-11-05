# BiDiB - netBiDiB

Dieses Dokument beschreibt netBiDiB, die Einbettung des BiDiB-Protokolls in den TCP/IP-Stack zur Kommunikation über Standard-Netzwerke (Ethernet, WLAN).

## 1. Architektur

-   **Client-Server-Modell:** Die Kommunikation ist verbindungsorientiert (TCP).
    -   **Server:** Akzeptiert eingehende Verbindungen (typischerweise ein BiDiB-Knoten/Hardware-Interface).
    -   **Client:** Baut aktiv Verbindungen zu Servern auf (typischerweise ein Host-Programm).
-   **Port:** Der Standard-Port ist 62875.

## 2. Verbindungsablauf

Eine netBiDiB-Verbindung wird in vier Phasen aufgebaut:

1.  **Discovery (Optional):** Ein Client findet verfügbare netBiDiB-Server im lokalen Netzwerk. Dies geschieht bevorzugt über Standard-Mechanismen wie DNS-SD (Bonjour, mDNS) mit dem Service-Namen `_bidib._tcp`.
2.  **Linkaufbau:** Nach dem Öffnen der TCP-Verbindung tauschen beide Teilnehmer ihre "Deskriptoren" aus (Unique-ID, Produktname, Username), um sich gegenseitig zu identifizieren.
3.  **Pairing:** Ein einmaliger Prozess, bei dem der Benutzer an beiden Geräten bestätigt, dass die Verbindung vertrauenswürdig ist. Gepaarte Geräte verbinden sich zukünftig automatisch. Das Pairing wird durch einen Austausch von `PAIRING_REQUEST`-Nachrichten initiiert und bestätigt.
4.  **Kontrolle (Logon):** Auf einem gepaarten Link kann sich ein Knoten beim Interface anmelden (`MSG_LOCAL_LOGON`), um eine Kontrollbeziehung herzustellen. Erst danach werden reguläre BiDiB-Nachrichten ausgetauscht.

## 3. Protokoll auf TCP

-   **Datenstrom:** Der TCP-Datenstrom besteht aus einer Sequenz von BiDiB-Nachrichten.
-   **Signatur:** Die allererste Nachricht in einem Stream muss eine `MSG_LOCAL_PROTOCOL_SIGNATURE` mit dem Präfix "BiDiB" sein, um die Verbindung als netBiDiB-Stream zu validieren.
-   **Keepalive:** TCP-Keepalive oder regelmäßige `MSG_LOCAL_PROTOCOL_SIGNATURE`-Nachrichten werden verwendet, um Verbindungsabbrüche zu erkennen.

## 4. Lokale Nachrichten für netBiDiB

Eine Reihe von `MSG_LOCAL_...`-Nachrichten wird verwendet, um den Verbindungsstatus zu steuern:

-   **`MSG_LOCAL_LINK`:** Wird für den Austausch von Deskriptoren, die Initiierung und Bestätigung des Pairings und die Statusabfrage verwendet.
-   **`MSG_LOCAL_LOGON`:** Ein Knoten bittet um die Aufnahme einer Kontrollbeziehung.
-   **`MSG_LOCAL_LOGON_ACK`:** Das Interface akzeptiert die Anmeldung.
-   **`MSG_LOCAL_LOGON_REJECTED`:** Das Interface lehnt die Anmeldung ab oder beendet eine bestehende Kontrollbeziehung.
-   **`MSG_LOCAL_LOGOFF`:** Ein Knoten meldet sich aktiv ab.
-   **`MSG_LOCAL_SYNC`:** Überträgt die Systemzeit, synchronisiert über NTP und einen UTC-Zeitstempel.
