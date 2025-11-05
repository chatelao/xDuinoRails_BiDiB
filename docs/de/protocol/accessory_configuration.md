# BiDiB - Zubehör – Konfiguration

Dieses Dokument beschreibt die Ansteuerung und Konfiguration von allgemeinen Schaltfunktionen (Ports) und Makros im BiDiB-Protokoll.

## 1. Ansteuerung Zubehör / Schaltfunktionen

Knoten mit Schaltfunktionen (z.B. für Licht, Animationen, Sound) haben das Class-ID-Flag 'Schalten' gesetzt. Sie verfügen über Ein- und Ausgänge, die als 'Ports' angesprochen werden.

Es gibt zwei Adressierungsmodelle für Ports:
-   **Typorientiertes Portmodell:** Ports sind fest einem Typ zugeordnet (z.B. Schalter, Licht, Servo) und werden innerhalb dieses Typs nummeriert.
-   **Flaches Portmodell:** Alle Ports teilen sich einen gemeinsamen Adressraum. Der Typ eines Ports kann per Konfiguration geändert werden.

### Porttypen
- `SWITCH`: Schaltausgang
- `LIGHT`: Lichtausgang (dimmbar, blinkend)
- `SERVO`: Servoausgang
- `SOUND`: Tonausgang
- `MOTOR`: Motorausgang
- `ANALOGOUT`: Analogausgang
- `BACKLIGHT`: Lichtausgang für Hintergrundbeleuchtung
- `SWITCHPAIR`: Zwei exklusiv geschaltete Ausgänge
- `INPUT`: Kontakteingang

## 2. Features für Schaltfunktionen

-   **FEATURE_CTRL_INPUT_COUNT (50):** Anzahl der Eingänge.
-   **FEATURE_CTRL_INPUT_NOTIFY (51):** Erlaubt spontane Meldungen von Eingängen.
-   **FEATURE_CTRL_..._COUNT (52-57, 59):** Anzahl der Ausgänge des jeweiligen Typs (SWITCH, LIGHT, SERVO, SOUND, MOTOR, ANALOGOUT, BACKLIGHT).
-   **FEATURE_CTRL_PORT_FLAT_MODEL (70):** Aktiviert das flache Portmodell und gibt die Anzahl der Ports an.
-   **FEATURE_CTRL_PORT_QUERY_AVAILABLE (66):** Gibt an, ob Portzustände abgefragt werden können.

## 3. Nachrichten für Schaltfunktionen

### 3.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_LC_OUTPUT:** Direkte Ansteuerung eines Ausgangs-Ports mit einem bestimmten Zustand.
-   **MSG_LC_PORT_QUERY:** Abfrage des Zustands eines einzelnen Ports.
-   **MSG_LC_PORT_QUERY_ALL:** Abfrage des Zustands aller oder mehrerer Ports.
-   **MSG_LC_CONFIGX_SET:** Konfiguration eines Ports über Schlüssel-Wert-Paare.
    -   **Beispiel:** Um die Helligkeit eines Lichtausgangs (`LIGHT`) zu konfigurieren, sendet der Host `MSG_LC_CONFIGX_SET` mit den Parametern für `BIDIB_PCFG_LEVEL_PORT_ON` und `BIDIB_PCFG_LEVEL_PORT_OFF`.
-   **MSG_LC_CONFIGX_GET:** Abfrage der Konfiguration eines Ports.
-   **MSG_LC_CONFIGX_GET_ALL:** Abfrage der Konfiguration aller oder mehrerer Ports.

### 3.2. Uplink: Nachrichten vom Knoten zum Host

-   **MSG_LC_NA:** Meldet eine Ausnahme oder einen Fehler an einem Port.
-   **MSG_LC_STAT:** Überträgt den Zustand eines Ports (als Antwort auf eine Abfrage oder spontan bei Eingängen).
-   **MSG_LC_WAIT:** Wird gesendet, wenn eine Ausgangsoperation länger als 100ms dauert, und gibt die geschätzte Restzeit an.
-   **MSG_LC_CONFIGX:** Antwort auf `MSG_LC_CONFIGX_GET` oder `MSG_LC_CONFIGX_SET`, enthält die Konfigurationsparameter des Ports.

## 4. Lokale Makros

Makros fassen Schaltfunktionen zu lokalen Abläufen zusammen. Ein Makro ist eine Schrittkette von Aktionen, die auf einem Knoten ausgeführt werden.

### 4.1. Features für lokale Makros

-   **FEATURE_CTRL_MAC_LEVEL (60):** Unterstützter Makro-Level (0: keine, 1: einfache Listen, 2: mit Abfragen, Start/Stop).
-   **FEATURE_CTRL_MAC_SAVE (61):** Anzahl der permanenten Speicherplätze für Makros.
-   **FEATURE_CTRL_MAC_COUNT (62):** Anzahl der möglichen Makros.
-   **FEATURE_CTRL_MAC_SIZE (63):** Maximale Anzahl der Einträge pro Makroliste.

### 4.2. Nachrichten für lokale Makros

#### Downlink
-   **MSG_LC_MACRO_HANDLE:** Steuert ein Makro (Start, Stop, Speichern, Löschen).
-   **MSG_LC_MACRO_SET:** Definiert einen einzelnen Schritt (Schaltpunkt) in einem Makro.
-   **MSG_LC_MACRO_GET:** Frägt einen Makroschritt ab.
-   **MSG_LC_MACRO_PARA_SET:** Setzt allgemeine Parameter für ein Makro (z.B. Geschwindigkeit, Wiederholungen).
-   **MSG_LC_MACRO_PARA_GET:** Frägt Makroparameter ab.

#### Uplink
-   **MSG_LC_MACRO_STATE:** Antwort auf `MSG_LC_MACRO_HANDLE`, meldet den Zustand des Makros.
-   **MSG_LC_MACRO:** Antwort auf `MSG_LC_MACRO_GET`, enthält die Daten des Makroschritts.
-   **MSG_LC_MACRO_PARA:** Antwort auf `MSG_LC_MACRO_PARA_GET`, enthält die Makroparameter.
