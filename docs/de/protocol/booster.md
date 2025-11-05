# BiDiB - Booster

Dieses Dokument beschreibt die Ansteuerung und Überwachung von Boostern im BiDiB-Protokoll.

## 1. Allgemeines

Booster dienen zur Verstärkung des DCC-Gleissignals. Sie können Diagnosefunktionen (Strom, Spannung, Temperatur), Bedienmöglichkeiten (An-/Abschalten) und einen globalen Railcom-Detektor enthalten.

Booster-Befehle (`MSG_BOOST_ON`/`MSG_BOOST_OFF`) werden vom Host typischerweise als Broadcast an alle Booster gesendet, um ein synchrones Verhalten sicherzustellen. Jeder Booster meldet seinen Statuswechsel einzeln zurück.

## 2. Features für Booster

-   **FEATURE_BST_VOLT_ADJUSTABLE (15):** Gibt an, ob die Ausgangsspannung einstellbar ist.
-   **FEATURE_BST_VOLT (16):** Der aktuelle Wert der Ausgangsspannung in Volt.
-   **FEATURE_BST_CUTOUT_AVAILABLE (17):** Gibt an, ob der Booster eine Railcom-Austastlücke erzeugen kann.
-   **FEATURE_BST_TURNOFF_TIME (19):** Abschaltzeit bei Kurzschluss in ms.
-   **FEATURE_BST_AMPERE_ADJUSTABLE (21):** Gibt an, ob der maximale Ausgangsstrom einstellbar ist.
-   **FEATURE_BST_AMPERE (22):** Der maximal eingestellte Ausgangsstrom.
-   **FEATURE_BST_CURMEAS_INTERVAL (23):** Aktiviert die Übermittlung von Diagnosewerten und legt das Intervall fest.
-   **FEATURE_BST_INHIBIT_AUTOSTART (26):** Verhindert das automatische Einschalten des Boosters bei anliegendem DCC-Signal.
-   **FEATURE_BST_INHIBIT_LOCAL_ONOFF (27):** Legt fest, ob lokale Taster (Go/Stop) direkt wirken oder nur an den Host gemeldet werden.

## 3. Nachrichten für Booster

### 3.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_BOOST_ON:** Schaltet die Gleisspannung ein. Ein Parameter (`UNICAST`) steuert, ob der Befehl an alle (Broadcast) oder nur an den adressierten Booster gesendet wird.
-   **MSG_BOOST_OFF:** Schaltet die Gleisspannung aus (ebenfalls mit `UNICAST`-Parameter).
-   **MSG_BOOST_QUERY:** Frägt den Status und ggf. Diagnosewerte des Boosters ab.

### 3.2. Uplink: Nachrichten vom Knoten zum Host

-   **MSG_BOOST_STAT:** Meldet den aktuellen Status des Boosters. Diese Nachricht wird nach jeder Zustandsänderung gesendet.
    -   `0x00`: Aus (allgemein)
    -   `0x01`: Aus (wegen Kurzschluss)
    -   `0x02`: Aus (wegen Übertemperatur)
    -   `0x80`: An
    -   `0x81`: An (in Strombegrenzung)
-   **MSG_BOOST_DIAGNOSTIC:** Übermittelt eine Liste von Diagnosewerten als Schlüssel-Wert-Paare.
    -   `0x00`: Stromwert
    -   `0x01`: Spannungswert
    -   `0x02`: Temperaturwert
