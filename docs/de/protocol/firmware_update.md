# BiDiB - Firmware Update

Dieses Dokument beschreibt den Firmware-Update-Prozess im BiDiB-Protokoll.

## 1. Allgemeines

Knoten können die Fähigkeit besitzen, ihre Firmware zu aktualisieren. Dies wird durch das Feature `FEATURE_FW_UPDATE_MODE` signalisiert.

## 2. Ablauf eines Firmware-Updates

Wenn `FEATURE_FW_UPDATE_MODE = 1` ist, läuft ein Update typischerweise wie folgt ab:

1.  **Update-Modus starten:** Der Host sendet `MSG_FW_UPDATE_OP` mit dem Parameter `BIDIB_MSG_FW_UPDATE_OP_ENTER` an den Knoten. Die Nachricht enthält die Unique-ID des Knotens zur Sicherheit. Der Knoten wechselt in einen eingeschränkten Modus, in dem nur noch Firmware-Update-Befehle akzeptiert werden.
2.  **Bereitschaft signalisieren:** Der Knoten antwortet mit einer `Ready`-Nachricht (`MSG_FW_UPDATE_STAT`), um zu signalisieren, dass er für den Empfang der Firmware bereit ist.
3.  **Zielspeicher festlegen:** Der Host sendet eine Nachricht, um den Zielspeicher (z.B. Flash) für die folgenden Daten anzugeben.
4.  **Empfang bestätigen:** Der Knoten quittiert den Befehl und signalisiert, dass er die Daten erwartet.
5.  **Daten übertragen:** Der Host sendet die Firmware-Datei (typischerweise im Intel-Hex-Format) Zeile für Zeile an den Knoten.
6.  **Jede Zeile quittieren:** Der Knoten bestätigt den Empfang jeder Zeile. Der Host darf die nächste Zeile erst nach Erhalt der Bestätigung senden.
7.  **Übertragung abschließen:** Der Host sendet eine Nachricht, um das Ende der Übertragung zu signalisieren.
8.  **Bereitschaft für weitere Pakete:** Der Knoten sendet eine `Ready`-Nachricht und ist bereit für ein weiteres Firmware-Paket.
9.  **Update-Modus beenden:** Am Ende des Updates sendet der Host einen Befehl zum Neustart (`BIDIB_MSG_FW_UPDATE_OP_EXIT`). Der Knoten bestätigt, wartet eine Sekunde, startet neu und meldet sich wieder am Bus an.

## 3. Features für FW-Update

-   **FEATURE_FW_UPDATE_MODE (254):**
    -   `0`: Kein Firmware-Update möglich.
    -   `1`: Firmware-Update im Intel-Hex-Format möglich.

## 4. Nachrichten für FW-Update

### 4.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_FW_UPDATE_OP:** Leitet eine Firmware-Update-Operation ein.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_ENTER` (0x00):** Weist den Knoten an, in den Update-Modus zu wechseln. Benötigt die 7-Byte Unique-ID des Knotens.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_EXIT` (0x01):** Weist den Knoten an, den Update-Modus zu verlassen und neu zu starten.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_SETDEST` (0x02):** Legt den Zielspeicherbereich fest (z.B. 0 für Flash, 1 für EEPROM).
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_DATA` (0x03):** Überträgt eine Zeile der Firmware-Datei.
    -   **Parameter `BIDIB_MSG_FW_UPDATE_OP_DONE` (0x04):** Signalisiert das Ende der Datenübertragung für den aktuellen Zielspeicher.

### 4.2. Uplink: Nachrichten vom Knoten zum Host

-   **MSG_FW_UPDATE_STAT:** Statusnachricht des Knotens als Antwort auf `MSG_FW_UPDATE_OP`.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_READY` (0x00):** Knoten ist im Update-Modus und betriebsbereit.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_EXIT` (0x01):** Knoten verlässt den Update-Modus.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_DATA` (0x02):** Knoten erwartet (weitere) Daten.
    -   **Status `BIDIB_MSG_FW_UPDATE_STAT_ERROR` (255):** Ein Fehler ist aufgetreten. Der zweite Parameter enthält einen spezifischen Fehlercode.
