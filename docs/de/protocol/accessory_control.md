# BiDiB - Zubehör – Schalten (Accessory Control)

Dieses Dokument beschreibt die Ansteuerung von Fahrwegzubehör (Accessories) im BiDiB-Protokoll.

## 1. Allgemeines

BiDiB unterscheidet zwischen `Accessory`-Funktionen (für den sicheren Zugbetrieb, z.B. Weichen, Signale) und `Schaltanwendungen` (für sekundäre Effekte, z.B. Beleuchtung). Dieses Dokument behandelt `Accessory`-Funktionen.

Accessory-Objekte haben einen Zustand, den sogenannten "Begriff" (Aspect), z.B. "Gerade" oder "Abzweig" bei einer Weiche. Der Zustand wird vom Host kontrolliert. Ein Schaltvorgang kann Zeit in Anspruch nehmen; der Knoten meldet den Beginn und das Ende des Vorgangs. Unvorhergesehene Zustandsänderungen (z.B. durch Handverstellung) werden spontan gemeldet (`MSG_ACCESSORY_NOTIFY`).

## 2. Features für Accessory-Funktionen

-   **FEATURE_ACCESSORY_COUNT (40):** Anzahl der steuerbaren Objekte (Weichen, Signale etc.) auf dem Knoten.
-   **FEATURE_ACCESSORY_SURVEILLED (41):** Gibt an, ob der Knoten spontane Zustandsänderungen (z.B. durch Handverstellung) meldet.
-   **FEATURE_ACCESSORY_MACROMAPPED (42):** Gibt an, ob Begriffe auf Makros abgebildet werden können.

## 3. Nachrichten für Accessory-Funktionen

### 3.1. Downlink: Nachrichten vom Host zum Knoten

-   **MSG_ACCESSORY_SET:** Stellt ein Zubehörobjekt auf einen bestimmten Begriff (Aspect).
    -   Parameter: `ANUM` (Objektnummer), `ASPECT` (Begriff).
-   **MSG_ACCESSORY_GET:** Frägt den aktuellen Zustand eines Objektes ab.
    -   Parameter: `ANUM`.
-   **MSG_ACCESSORY_GETALL:** Frägt den Zustand aller Objekte auf dem Knoten ab.
-   **MSG_ACCESSORY_PARA_SET:** Stellt Konfigurations-Parameter eines Objektes ein (optional).
    -   Parameter: `ANUM`, `PARA_NUM`, `DATA`.
-   **MSG_ACCESSORY_PARA_GET:** Frägt Konfigurations-Parameter eines Objektes ab (optional).
    -   Parameter: `ANUM`, `PARA_NUM`.

### 3.2. Uplink: Nachrichten vom Knoten zum Host

-   **MSG_ACCESSORY_STATE:** Meldet den Zustand eines Objektes. Dies geschieht als Antwort auf einen Befehl oder nach Abschluss eines Stellvorgangs.
    -   Parameter: `ANUM`, `ASPECT`, `TOTAL` (Anzahl der Begriffe), `EXECUTE` (Ausführungsstatus), `WAIT` (Restzeit oder Fehlercode).
-   **MSG_ACCESSORY_NOTIFY:** Meldet eine spontane Zustandsänderung (z.B. durch Handverstellung oder Fehler). Die Parameter sind identisch zu `MSG_ACCESSORY_STATE`.
-   **MSG_ACCESSORY_PARA:** Antwort auf `MSG_ACCESSORY_PARA_GET` oder `MSG_ACCESSORY_PARA_SET` (optional).
