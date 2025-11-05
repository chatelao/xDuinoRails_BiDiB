# BiDiB - BiDiBus

Dieses Dokument beschreibt den BiDiBus, einen robusten Highspeed-Bus für die Modellbahnsteuerung auf Basis von RS485.

## 1. Eigenschaften

-   **Hohe Geschwindigkeit:** 500 kBaud.
-   **Gesicherte Übertragung:** Störunanfällig durch differentielle Signalübertragung (RS485) und CRC-Prüfung.
-   **Freie Verkabelung:** Lineare Topologie mit kurzen Stichleitungen, automatische Adressvergabe (Plug & Play).
-   **Integrierte Signale:** Überträgt neben den BiDiB-Daten auch das DCC-Gleissignal und ein ACK-Signal.
-   **Spannungsversorgung:** Kann Busteilnehmer mit geringem Strombedarf versorgen.

## 2. Elektrische und Mechanische Parameter

-   **Verkabelung:** Standard-Netzwerkkabel (CAT5) mit RJ45-Steckern.
-   **Busabschluss:** An beiden Enden des linearen Busses ist ein Abschlusswiderstand von 120 Ohm erforderlich.
-   **Teilnehmer:** Bis zu 32 Teilnehmer pro Bussegment.
-   **Pinbelegung (RJ45):**
    -   1/2: VCC
    -   3/6: GND
    -   4/5: DATA (RS485 A/B)
    -   7/8: DCC (RS485 A/B)
    -   ACK: Open-Kollektor-Leitung für DCC-Bestätigungen und Nothalt.

## 3. Busprotokoll

-   **Buszugriff:** Das Interface (Master) steuert den Buszugriff über ein Token-Passing-Verfahren. Es sendet Poll-Kommandos an die einzelnen Knoten, um ihnen die Sendeerlaubnis zu erteilen.
-   **Nachrichtenformat:** Ein BiDiBus-Paket (`BIDIBUS_PKT`) besteht aus:
    `P_LENGTH MESSAGE_SEQ CRC`
    -   `P_LENGTH`: Die Länge der `MESSAGE_SEQ` in Bytes.
    -   `MESSAGE_SEQ`: Eine Sequenz von einer oder mehreren BiDiB-Nachrichten.
    -   `CRC`: Eine CRC8-Prüfsumme über `P_LENGTH` und `MESSAGE_SEQ`.
-   **Anmeldevorgang (Logon):**
    1.  Das Interface sendet periodisch eine Anmeldeaufforderung (`BIDIBUS_LOGON`).
    2.  Noch nicht angemeldete Knoten antworten mit ihrer Unique-ID. Ein Kollisionsvermeidungsmechanismus (zufällige Verzögerung) reduziert die Wahrscheinlichkeit von gleichzeitigen Antworten.
    3.  Das Interface bestätigt eine erfolgreiche Anmeldung mit `MSG_LOCAL_LOGON_ACK` und weist dem Knoten eine dynamische `NODE_ADDR` zu.

## 4. Systemzeit

Zur Synchronisation der Systemzeit sendet das Interface regelmäßig `MSG_LOCAL_SYNC`-Nachrichten als Broadcast.
