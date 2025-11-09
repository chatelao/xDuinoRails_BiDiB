# BiDiB Arduino Library - Implementierungsplan

Dieses Dokument beschreibt die empfohlene Reihenfolge für die Implementierung einer vollständigen BiDiB-Bibliothek für Arduino. Der Plan ist in logische Phasen unterteilt, die aufeinander aufbauen.

Vor jeder neuen Implementierung ist dieses Dokument zu konsultieren. Nach Abschluss einer Implementierungsphase ist der Status in diesem Dokument zu aktualisieren.

---

## Phase 1: Protokoll-Grundlagen (Core Protocol Layer)

**Ziel:** Eine solide Basis für die gesamte Kommunikation schaffen. In dieser Phase wird die grundlegende Fähigkeit implementiert, valide BiDiB-Nachrichten zu erstellen und zu parsen.

- [x] **1.1. Nachrichten-Framing:**
    - [x] Implementierung der `MAGIC`-Bytes (0xFE) zur Erkennung von Nachrichtenanfang und -ende.
    - [x] Implementierung des Escape-Mechanismus für `0xFE` und `0xFD`.
- [x] **1.2. Datensicherung:**
    - [x] Implementierung der CRC8-Prüfsummenberechnung (Polynom: `0x31`).
- [ ] **1.3. Nachrichten-Struktur:**
    - [ ] Erstellung von Datenstrukturen zur Repräsentation von BiDiB-Nachrichten (`MSG_LENGTH`, `MSG_ADDR`, `MSG_NUM`, `MSG_TYPE`, `DATA`).
    - [ ] Entwicklung von Funktionen zum Serialisieren (Erstellen eines Byte-Streams aus einer Nachrichten-Struktur) und Deserialisieren (Parsen eines Byte-Streams in eine Nachrichten-Struktur).
- [ ] **1.4. Transport-Schicht:**
    - [ ] Implementierung einer grundlegenden seriellen Kommunikation (UART) für die Verbindung zum BiDiB-Interface.

---

## Phase 2: System- und Knoten-Management

**Ziel:** Die grundlegende Interaktion mit dem BiDiB-Bus ermöglichen, Knoten erkennen und deren Eigenschaften auslesen.

- [ ] **2.1. System-Nachrichten (Basis):**
    - [ ] Implementierung der grundlegenden System-Nachrichten für den Verbindungsaufbau:
        - [ ] `MSG_SYS_GET_MAGIC` / `MSG_SYS_MAGIC`
        - [ ] `MSG_SYS_GET_P_VERSION` / `MSG_SYS_P_VERSION`
        - [ ] `MSG_SYS_GET_UNIQUE_ID` / `MSG_SYS_UNIQUE_ID`
- [ ] **2.2. Knoten-Management:**
    - [ ] Implementierung der Logon-Sequenz.
    - [ ] Verwaltung der Knotentabelle (`MSG_NODETAB_GETALL`, `MSG_NODETAB_GETNEXT` / `MSG_NODETAB_COUNT`, `MSG_NODETAB`).
    - [ ] Handling von neuen und verlorenen Knoten (`MSG_NODE_NEW`, `MSG_NODE_LOST`).
- [ ] **2.3. Feature-Management:**
    - [ ] Implementierung der Abfrage von Knoteneigenschaften (`MSG_FEATURE_GETALL`, `MSG_FEATURE_GETNEXT`, `MSG_FEATURE_GET` / `MSG_FEATURE_COUNT`, `MSG_FEATURE`, `MSG_FEATURE_NA`).
    - [ ] Implementierung zum Setzen von Features (`MSG_FEATURE_SET`).
- [ ] **2.4. System-Steuerung:**
    - [ ] Implementierung von `MSG_SYS_ENABLE` und `MSG_SYS_DISABLE` zur Steuerung des Nachrichtenflusses.

---

## Phase 3: Command Station (Gleis-Steuerung)

**Ziel:** Die Steuerung von Lokomotiven und DCC-Zubehör über das Gleissignal ermöglichen.

- [ ] **3.1. Gleis-Zustand:**
    - [ ] Implementierung von `MSG_CS_SET_STATE` zur Steuerung der Gleisspannung (GO, STOP, OFF).
    - [ ] Handling von `MSG_CS_STATE`-Antworten.
- [ ] **3.2. Lok-Steuerung:**
    - [ ] Implementierung von `MSG_CS_DRIVE` zur Steuerung von Geschwindigkeit und Funktionen.
    - [ ] Handling von `MSG_CS_DRIVE_ACK`-Quittungen.
- [ ] **3.3. DCC-Zubehör:**
    - [ ] Implementierung von `MSG_CS_ACCESSORY` zur Steuerung von Weichen und Signalen über das Gleissignal.
    - [ ] Handling von `MSG_CS_ACCESSORY_ACK`-Quittungen.
- [ ] **3.4. Programmierung:**
    - [ ] Implementierung von PoM (`MSG_CS_POM` / `MSG_CS_POM_ACK`).

---

## Phase 4: Rückmeldung (Occupancy & Railcom)

**Ziel:** Belegtmeldungen und Adressinformationen von der Anlage empfangen und verarbeiten.

- [ ] **4.1. Belegtmeldung:**
    - [ ] Implementierung des Empfangs von `MSG_BM_OCC`, `MSG_BM_FREE` und `MSG_BM_MULTIPLE`.
- [ ] **4.2. Secure-ACK:**
    - [ ] Implementierung des Secure-ACK-Mechanismus durch Senden von `MSG_BM_MIRROR_...`-Nachrichten.
- [ ] **4.3. Adress-Rückmeldung (Railcom):**
    - [ ] Implementierung des Empfangs von `MSG_BM_ADDRESS` zur Erkennung von Loks in einem Abschnitt.
- [ ] **4.4. Weitere Rückmeldungen:**
    - [ ] Implementierung des Empfangs von CV-Antworten (`MSG_BM_CV`) und Geschwindigkeit (`MSG_BM_SPEED`).

---

## Phase 5: Natives BiDiB-Zubehör

**Ziel:** Direkte Steuerung von BiDiB-Zubehörknoten (unabhängig vom DCC-Signal).

- [ ] **5.1. Zubehör-Steuerung:**
    - [ ] Implementierung von `MSG_ACCESSORY_SET` und `MSG_ACCESSORY_GET`.
- [ ] **5.2. Zustands-Handling:**
    - [ ] Verarbeitung von `MSG_ACCESSORY_STATE` (als Antwort auf einen Befehl) und `MSG_ACCESSORY_NOTIFY` (bei spontanen Änderungen).

---

## Phase 6: Fortgeschrittene Funktionen

**Ziel:** Implementierung von speziellen Funktionen für Booster-Management und Wartung.

- [ ] **6.1. Booster-Management:**
    - [ ] Implementierung von `MSG_BOOST_ON` und `MSG_BOOST_OFF`.
    - [ ] Verarbeitung von Status- (`MSG_BOOST_STAT`) und Diagnosemeldungen (`MSG_BOOST_DIAGNOSTIC`).
- [ ] **6.2. Firmware Update:**
    - [ ] Implementierung des kompletten Firmware-Update-Prozesses (`MSG_FW_UPDATE_OP`, `MSG_FW_UPDATE_STAT`).
- [ ] **6.3. Hersteller-spezifische Konfiguration:**
    - [ ] Implementierung der `MSG_VENDOR_...`-Nachrichten.
