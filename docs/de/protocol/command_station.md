# BiDiB - Gleissignal (Command Station)

Dieses Dokument beschreibt die Ansteuerung von Gleisausgabe-Geräten (Command Stations) im BiDiB-Protokoll.

## 1. Allgemeines

Gleisausgabe-Geräte sind Knoten, die ein DCC-Signal erzeugen können. BiDiB ermöglicht den parallelen Betrieb mehrerer DCC-Systeme, z.B. für ein separates Programmiergleis.

### Watchdog

Um den unkontrollierten Weiterbetrieb von Zügen bei einem Verbindungsabbruch zum Host zu verhindern, gibt es eine Watchdog-Funktion. Der Host muss regelmäßig den `GO`-Zustand (`MSG_CS_SET_STATE`) erneuern. Bleibt dies aus, werden alle Loks angehalten. Das Intervall wird durch `FEATURE_GEN_WATCHDOG` festgelegt.

## 2. Features für Gleisausgabe

-   **FEATURE_GEN_SPYMODE (100):** Erlaubt das "Mithören" von lokalen Handreglerbefehlen.
-   **FEATURE_GEN_WATCHDOG (101):** Definiert das Intervall für die Hostüberwachung.
-   **FEATURE_GEN_DRIVE_ACK (102):** Aktiviert mehrstufige Quittungen für Fahrbefehle.
-   **FEATURE_GEN_SWITCH_ACK (103):** Aktiviert mehrstufige Quittungen für Schaltbefehle.
-   **FEATURE_GEN_POM_REPEAT (106):** Anzahl der Wiederholungen für POM-Befehle.
-   **FEATURE_GEN_DRIVE_BUS (107):** Steuert, ob der Knoten den DCC-Bus treibt oder empfängt.
-   **FEATURE_GEN_LOK_LOST_DETECT (108):** Aktiviert die Erkennung "verlorener" Lokomotiven.
-   **FEATURE_GEN_NOTIFY_DRIVE_MANUAL (109):** Melden von lokal bedienten Lokomotiven/Zubehör.
-   **FEATURE_GEN_START_STATE (110):** Zustand der Gleisausgabe nach dem Einschalten (An/Aus).
-   **FEATURE_GEN_EXT_AVAILABLE (111):** Bitfeld für zusätzliche unterstützte Protokolle (RailcomPlus, M4, DCCA, etc.).

## 3. Nachrichten für Gleisausgabe

### 3.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_CS_ALLOCATE:** Sperrt den Knoten für Befehle von anderen lokalen Adressen (für 2s gültig).
-   **MSG_CS_SET_STATE:** Stellt den Zustand der Gleisausgabe ein (OFF, STOP, GO, PROG, etc.).
-   **MSG_CS_DRIVE:** Sendet Fahrbefehle (Adresse, Format, Geschwindigkeit, Funktionen F0-F28).
    -   **Geschwindigkeitskodierung:** Die Geschwindigkeit wird immer als 7-Bit-Wert (0-127) plus ein Richtungsbit übergeben, analog zum DCC128-Format. Die Umrechnung in andere Formate (z.B. DCC28) erfolgt im Knoten.
        -   `0`: Halt
        -   `1`: Nothalt
        -   `2-127`: Fahrstufen
-   **MSG_CS_ACCESSORY:** Steuert Zubehördekoder über das DCC-Gleissignal.
-   **MSG_CS_POM:** Sendet Programmierbefehle für das Hauptgleis (Program on Main).
-   **MSG_CS_BIN_STATE:** Löst einzelne Aktionen bei einem Fahrzeugdekoder aus (z.B. Kupplung).
-   **MSG_CS_QUERY:** Frägt den Zustand aktiver Fahrzeuge ab.
-   **MSG_CS_PROG:** Sendet Programmierbefehle für das Programmiergleis.
-   **MSG_CS_RCPLUS:** Steuert die Railcom-Plus-Ausgabe (optional).

### 3.2. Uplink: Nachrichten vom Knoten zum Host

Es existiert ein mehrstufiges Quittungsverfahren:
-   **Ebene 0:** Nachricht vom Knoten empfangen.
-   **Ebene 1:** Befehl wurde auf das Gleis ausgegeben.
-   **Ebene 2:** Dekoder hat den Befehl per ACK (via BiDi) bestätigt.

-   **MSG_CS_STATE:** Meldet den aktuellen Zustand der Gleisausgabe.
-   **MSG_CS_DRIVE_ACK:** Quittiert einen Fahrbefehl.
-   **MSG_CS_ACCESSORY_ACK:** Quittiert einen Schaltbefehl für Zubehör.
-   **MSG_CS_POM_ACK:** Quittiert einen POM-Befehl.
-   **MSG_CS_DRIVE_MANUAL:** Meldet eine manuelle Bedienung einer Lokomotive.
-   **MSG_CS_DRIVE_EVENT:** Meldet Ereignisse bei einer Lokomotive (z.B. `LOST`).
-   **MSG_CS_ACCESSORY_MANUAL:** Meldet eine manuelle Verstellung bei einem DCC-Zubehör.
-   **MSG_CS_DRIVE_STATE:** Antwort auf `MSG_CS_QUERY` mit dem Zustand eines Fahrzeugs.
-   **MSG_CS_PROG_STATE:** Meldet das Ergebnis eines Programmiervorgangs am Programmiergleis.
-   **MSG_CS_RCPLUS_ACK:** Quittiert einen Railcom-Plus-Befehl (optional).
