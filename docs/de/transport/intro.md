# BiDiB - Transport

Dieses Dokument beschreibt die verschiedenen physikalischen Übertragungsmedien für das BiDiB-Protokoll.

## Übersicht

BiDiB ist so konzipiert, dass es über verschiedene Medien transportiert werden kann. Die jeweilige Übertragungsschicht (Transport Layer) ist für das korrekte Framing der Nachrichten und die Sicherung der Datenübertragung (z.B. mittels CRC) verantwortlich.

Die wichtigsten Transportprotokolle sind:

-   **[Serielle Verbindung](./serial.md):** Eine Punkt-zu-Punkt-Verbindung, typischerweise über RS232 oder einen virtuellen COM-Port (USB). Geeignet für die direkte Verbindung zwischen einem Computer und dem BiDiB-Interface.
-   **[BiDiBus](./bidibus.md):** Ein robuster Feldbus auf RS485-Basis, der speziell für die Verkabelung innerhalb der Modellbahnanlage entwickelt wurde. Er ermöglicht hohe Geschwindigkeiten und die anlagenweite Verteilung des DCC-Signals.
-   **[netBiDiB (Netzwerk)](./netbidib.md):** Eine paketorientierte Netzwerkverbindung auf Basis von TCP/IP. Sie eignet sich ideal, um Computer untereinander zu verbinden oder mobile Geräte (Smartphones, Tablets) in die Steuerung einzubinden.

Diese Übertragungsmedien können beliebig kombiniert werden. Das Routing der Nachrichten wird durch die eindeutige Kennung jeder Baugruppe (Unique-ID) sichergestellt.
