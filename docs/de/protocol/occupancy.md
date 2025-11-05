# BiDiB - Melden (Occupancy)

Dieses Dokument beschreibt die Belegtmeldung und die Auswertung von BiDi-Nachrichten (Railcom) im BiDiB-Protokoll.

## 1. Belegtmeldung

Belegtmeldung dient dazu, den Aufenthaltsort von Fahrzeugen zu erfassen, typischerweise über eine Strommessung. Knoten, die diese Funktion bieten, haben das Class-ID-Flag 'Melderfunktion' gesetzt.

### Absicherung
Aufgrund der Wichtigkeit von Belegtmeldungen für den sicheren Betrieb bietet BiDiB mehrere Sicherungsmechanismen:
-   **CRC und Sequenznummern:** Sichern die Übertragung.
-   **Secure-ACK:** Ein erweitertes Quittungsverfahren, bei dem der Host empfangene Meldungen zurückspiegelt. Bei Abweichungen sendet der Melder die korrekten Daten erneut.
    -   **Beispielablauf:**
        1.  **Knoten → Host:** `MSG_BM_OCC` (Melder 5 ist belegt)
        2.  **Host → Knoten:** `MSG_BM_MIRROR_OCC` (Bestätigung für Melder 5)
        3.  Wenn die Bestätigung ausbleibt, wiederholt der Knoten die `MSG_BM_OCC`-Nachricht.
-   **'Vertrauens'-Kontrolle:** Der Melder kann die "Qualität" seiner Meldung übermitteln (z.B. ob die Erfassung durch einen Kurzschluss gestört ist).
-   **'Alive'-Kontrolle:** Das Interface überwacht die Verbindung zu den Meldern.

## 2. Features für Belegtmelder

-   **FEATURE_BM_SIZE (0):** Anzahl der Melderbits des Knotens.
-   **FEATURE_BM_ON (1):** Aktiviert/Deaktiviert die spontane Übermittlung von Belegtmeldungen.
-   **FEATURE_BM_SECACK_AVAILABLE (2):** Gibt an, ob der Melder das Secure-ACK-Verfahren unterstützt.
-   **FEATURE_BM_SECACK_ON (3):** Aktiviert das Secure-ACK-Verfahren.
-   **FEATURE_BM_CURMEAS_AVAILABLE (4):** Gibt an, ob der Melder Strommessdaten liefern kann.
-   **FEATURE_BM_CURMEAS_INTERVAL (5):** Aktiviert die Übertragung von Strommessdaten und legt das Intervall fest.

## 3. Nachrichten für Belegtmelder

### 3.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_BM_GET_RANGE:** Frägt die Zustände aller Melder in einem bestimmten Bereich ab.
-   **MSG_BM_MIRROR_OCC / MSG_BM_MIRROR_FREE / MSG_BM_MIRROR_MULTIPLE:** Werden für das Secure-ACK-Verfahren verwendet, um empfangene Zustände an den Melder zurückzuspiegeln.
-   **MSG_BM_GET_CONFIDENCE:** Frägt die "Vertrauenswürdigkeit" der aktuellen Belegtmeldung ab.

### 3.2. Uplink: Nachrichten vom Knoten zum Host

-   **MSG_BM_OCC:** Meldet, dass ein einzelner Melder belegt ist.
-   **MSG_BM_FREE:** Meldet, dass ein einzelner Melder frei ist.
-   **MSG_BM_MULTIPLE:** Übermittelt die Zustände eines ganzen Bereichs von Meldern.
-   **MSG_BM_CONFIDENCE:** Übermittelt die "Vertrauenswürdigkeit" der Meldung (gültig, eingefroren, kein Gleissignal).
-   **MSG_BM_CURRENT:** Übermittelt den Stromverbrauch eines Abschnitts.

## 4. BiDi-Detektoren (Railcom)

Knoten können auch bidirektionale Nachrichten von Fahrzeugen (Railcom) auswerten.

### 4.1. Features für BiDi-Detektoren

-   **FEATURE_BM_ADDR_DETECT_AVAILABLE (8):** Gibt an, ob der Detektor Adressen erkennen kann.
-   **FEATURE_BM_ADDR_DETECT_ON (9):** Aktiviert die Übermittlung von Adressdaten.
-   **FEATURE_BM_ADDR_AND_DIR (10):** Gibt an, ob der Detektor die Aufgleisrichtung erkennen kann.
-   **FEATURE_BM_ISTSPEED_AVAILABLE (11):** Gibt an, ob der Detektor Geschwindigkeitsmeldungen (km/h) weiterleiten kann.
-   **FEATURE_BM_CV_AVAILABLE (13):** Gibt an, ob der Detektor CV-Antworten (PoM) erkennen kann.

### 4.2. Nachrichten für BiDi-Detektoren

#### Downlink
-   **MSG_BM_ADDR_GET_RANGE:** Fordert die erneute Übertragung der erkannten Lok-Adressen in einem Bereich an.
-   **MSG_BM_MIRROR_POSITION:** Wird für das Secure-ACK bei Positionsmeldungen verwendet.

#### Uplink
-   **MSG_BM_ADDRESS:** Meldet das Auftreten einer oder mehrerer Lok-Adressen in einem Abschnitt.
-   **MSG_BM_CV / MSG_BM_XPOM:** Leitet CV-Antworten eines Dekoders (aus einer PoM-Operation) weiter.
-   **MSG_BM_SPEED:** Leitet die von einem Dekoder gemeldete Geschwindigkeit (km/h) weiter.
-   **MSG_BM_DYN_STATE:** Übermittelt dynamische Zustandsdaten eines Dekoders (z.B. Signalqualität, Temperatur).
-   **MSG_BM_RCPLUS:** Meldet Rückmeldungen von Dekodern während des RailcomPlus-Anmeldeprozesses.
-   **MSG_BM_POSITION:** Meldet die von einem Dekoder übermittelte Position (z.B. von einer Ortsbake).
