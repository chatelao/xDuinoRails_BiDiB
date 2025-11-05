# BiDiB - Allgemeine Protokollteile

Dieses Dokument beschreibt die allgemeinen Teile des BiDiB-Protokolls.

## 1. Allgemeines

### 1.1. Zielsetzung, besondere Eigenschaften

Das Protokoll BiDiB dient zur Steuerung einer Modellbahn. Es ermöglicht die Kontrolle und Ansteuerung von Loks, Weichen und Zubehör sowie die sichere Übertragung von Rückmelderinformationen aus der Modellbahnanlage an den steuernden Rechner. BiDiB steht für BiDirektionaler Bus und bietet folgende Vorteile:
- Automatische Hardwarezuordnung, keine Programmierung erforderlich (Plug&Play).
- Weitgehend freie Anordnung von Baugruppen.
- Einfache Skalierbarkeit und Erweiterbarkeit durch Hub-Bausteine.
- Extrem schnelle Datenübertragung, durch CRC abgesichert.
- Überwachung der gesamten Übertragungskette mit Secure-ACK-Technik.
- Volle Unterstützung BiDi-fähiger Rückmelder.
- Meldung mehrerer Loks in einem Abschnitt.
- Mischbetrieb normaler und BiDi-fähiger Rückmelder.
- Flexibles, offenes Protokoll, zukunftssicher.

### 1.2. Design-Grundlagen

- BiDiB ist als zustandsloses Protokoll angelegt.
- Nachrichten sind idempotent.
- Kommunikationsfehler werden nur einfach gemeldet.
- Spontane Ereignisse werden nicht wiederholt.
- Rückwärtskompatibilität wird bei neuen Revisionen beachtet.

### 1.3. Glossar

- **BiDiB:** Der Protokollstandard.
- **BiDiBus:** Eine mögliche physikalische Implementierung (RS485).
- **Bussystem:** Der gesamte Aufbau.
- **Class:** Einteilung der Knoten nach Eigenschaften.
- **Feature:** Eine bestimmte Eigenschaft eines Knotens.
- **Host:** Der steuernde Computer.
- **Hub:** Ein Knoten, der als Interface für eine untergeordnete Ebene dient.
- **Interface:** Schnittstelle zum Host oder übergeordneten Knoten.
- **Knoten:** Ein Teilnehmer im Bussystem.
- **Logon:** Versuch eines Teilnehmers, eine logische Verbindung aufzunehmen.
- **Melder:** Ein Knoten, der die Gleisbelegung erfasst.
- **Node-Adresse:** Vom Interface vergebene Nummer für einen Knoten.
- **Magic:** System-ID zur Erkennung der Baudeinstellung.
- **Unique-ID:** Global eineindeutige Kennung eines Knotens.

### 1.4. Prinzipien der Knotenzuordnung, Adressierung

- Baumartige Organisation.
- Automatische Adressvergabe durch das Interface.
- Zuordnungstabelle wird beim Start erstellt.
- Änderungen in der Struktur werden dem Host gemeldet.
- Die Adressierung kann hierarchisch über mehrere Ebenen (Hubs) erfolgen. Die Adresse eines Knotens wird dann als Adress-Stack dargestellt, der den Pfad vom Interface zum Zielknoten beschreibt.
  - `00`: Das Interface selbst.
  - `04 00`: Ein Knoten (Hub) mit der lokalen Adresse `04` auf der ersten Ebene.
  - `04 02 00`: Ein Knoten mit der lokalen Adresse `02` hinter dem Hub `04`.

## 2. Protokollbeschreibung

### 2.1. Prinzipieller Nachrichtenaufbau

Eine Nachricht (MESSAGE) hat folgenden Aufbau:
`MESSAGE ::= MSG_LENGTH MSG_ADDR MSG_NUM MSG_TYPE DATA`

- **MSG_LENGTH:** Nachrichtengröße in Bytes (ab MSG_ADDR).
- **MSG_ADDR:** Ziel- oder Quelladresse (Stack von NODE_ADDR, endet mit 0x00).
- **MSG_NUM:** Fortlaufender Nachrichtenindex (1-255). 0 für keine Sequenzprüfung.
- **MSG_TYPE:** Nachrichtentyp, kodiert Art und Richtung.
- **DATA:** Optionale Parameter.

### 2.2. Datensicherung und Framing

- **CRC-Prüfung:** Jede Nachricht wird durch eine CRC8-Prüfsumme gesichert. Diese wird über die gesamte Nachricht (von `MSG_LENGTH` bis zum Ende von `DATA`) berechnet. Das verwendete Polynom ist `x^8 + x^5 + x^4 + 1` (0x31).
- **Framing (Seriell):** Bei der seriellen Übertragung wird jede Nachricht von `MAGIC`-Bytes (0xFE) eingerahmt. Um zu verhindern, dass Datenbytes fälschlicherweise als `MAGIC`-Byte interpretiert werden, wird ein Escape-Mechanismus verwendet:
  - Wenn ein Datenbyte den Wert `0xFE` (MAGIC) oder `0xFD` (ESCAPE) hat, wird ihm ein `ESCAPE`-Byte (0xFD) vorangestellt und das ursprüngliche Byte mit `0x20` XOR-verknüpft.

#### Beispiel für eine serielle Nachricht (`MSG_SYS_GET_MAGIC`):
`0xFE 0x03 0x00 0x00 0x01 0xD6 0xFE`
- `0xFE`: Start-MAGIC
- `0x03`: MSG_LENGTH
- `0x00`: MSG_ADDR (Interface)
- `0x00`: MSG_NUM
- `0x01`: MSG_TYPE
- `0xD6`: CRC8
- `0xFE`: End-MAGIC

### 2.3. Unique-ID

Die Unique-ID besteht aus 7 Bytes:
- **Byte 1-2: ClassID:** Bitfeld der Klassenzugehörigkeit.
- **Byte 3: Vendor-ID:** Herstellerkennung (NMRA).
- **Byte 4-7: Produktkennung:** Produkttyp und Seriennummer.

### 2.3. Typischer Protokollstart

1.  Verbindungsaufbau mit dem Interface.
2.  Anhalten des BiDiB-Systems mit `MSG_SYS_DISABLE`.
3.  Eigenschaften des ersten Knotens einlesen (Magic, Version, Unique-ID, Features, Knotentabelle).
4.  Schritt 3 für alle weiteren Knoten wiederholen.
5.  System mit `MSG_SYS_ENABLE` für Spontanmeldungen freigeben.

#### Beispiel für eine Start-Sequenz
Eine typische Abfolge von Nachrichten beim Start sieht so aus:
1.  Host sendet `MSG_SYS_GET_MAGIC` (um die Verbindung und Baudrate zu prüfen).
2.  Knoten antwortet mit `MSG_SYS_MAGIC`.
3.  Host sendet `MSG_SYS_RESET`.
4.  Host fragt grundlegende Informationen ab: `MSG_SYS_GET_P_VERSION`, `MSG_SYS_GET_UNIQUE_ID`, `MSG_SYS_GET_SW_VERSION`.
5.  Host fragt die Knotentabelle ab (`MSG_NODETAB_GETALL`) und danach alle Features (`MSG_FEATURE_GETALL`).

## 3. System-Nachrichten

### 3.1. Downlink: System-Nachrichten

- **MSG_SYS_GET_MAGIC:** Systemkennung abfragen.
- **MSG_SYS_GET_P_VERSION:** Protokollversion abfragen.
- **MSG_SYS_ENABLE:** Spontane Meldungen für bestimmte Klassen erlauben.
- **MSG_SYS_DISABLE:** Spontane Meldungen für bestimmte Klassen unterbinden.
- **MSG_SYS_GET_UNIQUE_ID:** Unique-ID und Konfigurations-Fingerprint abfragen.
- **MSG_SYS_GET_SW_VERSION:** Softwareversion abfragen.
- **MSG_SYS_PING:** Lebenszeichen eines Knotens prüfen.
- **MSG_SYS_IDENTIFY:** Identifikationsanzeige (z.B. LED-Blinken) ein-/ausschalten.
- **MSG_SYS_GET_ERROR:** Letzten Fehler auslesen.
- **MSG_SYS_RESET:** System zurücksetzen und Knotenzuordnung neu durchführen.
- **MSG_NODETAB_GETALL:** Komplette Knotentabelle anfordern.
- **MSG_NODETAB_GETNEXT:** Nächsten Eintrag der Knotentabelle anfordern.
- **MSG_NODE_CHANGED_ACK:** Bestätigung für `MSG_NODE_NEW` oder `MSG_NODE_LOST`.
- **MSG_SYS_CLOCK:** Anlagen-Modelluhrzeit übertragen.

### 3.2. Uplink: System-Nachrichten

- **MSG_SYS_MAGIC:** Antwort auf `MSG_SYS_GET_MAGIC`. Enthält die Systemkennung.
- **MSG_SYS_PONG:** Antwort auf `MSG_SYS_PING`.
- **MSG_SYS_P_VERSION:** Antwort auf `MSG_SYS_GET_P_VERSION`. Enthält die Protokollversion.
- **MSG_SYS_UNIQUE_ID:** Antwort auf `MSG_SYS_GET_UNIQUE_ID`. Enthält die 7-Byte Unique-ID.
- **MSG_SYS_SW_VERSION:** Antwort auf `MSG_SYS_GET_SW_VERSION`. Enthält die Softwareversion.
- **MSG_SYS_IDENTIFY_STATE:** Meldet den Zustand der Identifikationsanzeige.
- **MSG_SYS_ERROR:** Fehlermeldung eines Knotens.
- **MSG_NODETAB_COUNT:** Antwort auf `MSG_NODETAB_GETALL`. Enthält die Anzahl der Einträge in der Knotentabelle.
- **MSG_NODETAB:** Antwort auf `MSG_NODETAB_GETNEXT`. Enthält einen Eintrag der Knotentabelle.
- **MSG_NODE_NA:** Wird gesendet, wenn ein angefragter Knoten nicht existiert.
- **MSG_NODE_LOST:** Meldet, dass ein Knoten nicht mehr erreichbar ist.
- **MSG_NODE_NEW:** Meldet, dass ein neuer Knoten erkannt wurde.
- **MSG_STALL:** Meldet, dass der Ausgangspuffer eines Knotens voll ist.

## 4. Feature-Nachrichten

Features dienen der Abfrage und Konfiguration von Knoteneigenschaften.

### 4.1. Downlink: Feature-Nachrichten

- **MSG_FEATURE_GETALL:** Abfrage aller Featurewerte starten.
- **MSG_FEATURE_GETNEXT:** Nächsten Featurewert abfragen.
- **MSG_FEATURE_GET:** Einzelnes Feature abfragen.
- **MSG_FEATURE_SET:** Einzelnes Feature einstellen.

### 4.2. Uplink: Feature-Nachrichten

- **MSG_FEATURE:** Antwort auf eine Feature-Abfrage. Enthält Feature-Nummer und Wert.
- **MSG_FEATURE_NA:** Wird gesendet, wenn ein angefragtes Feature nicht verfügbar ist.
- **MSG_FEATURE_COUNT:** Antwort auf `MSG_FEATURE_GETALL`. Enthält die Anzahl der Features.

#### Beispiel für eine Abfrage aller Features
1.  **Host → Knoten:** `MSG_FEATURE_GETALL`
2.  **Knoten → Host:** `MSG_FEATURE_COUNT` (z.B. mit Wert 3)
3.  **Host → Knoten:** `MSG_FEATURE_GETNEXT`
4.  **Knoten → Host:** `MSG_FEATURE` (für das erste Feature)
5.  **Host → Knoten:** `MSG_FEATURE_GETNEXT`
6.  **Knoten → Host:** `MSG_FEATURE` (für das zweite Feature)
7.  **Host → Knoten:** `MSG_FEATURE_GETNEXT`
8.  **Knoten → Host:** `MSG_FEATURE` (für das dritte Feature)
9.  **Host → Knoten:** `MSG_FEATURE_GETNEXT`
10. **Knoten → Host:** `MSG_FEATURE_NA` (mit speziellem Wert 255, um das Ende zu signalisieren)

## 5. Nachrichten zur Userkonfiguration (Vendor-spezifisch)

Diese Nachrichten dienen der Übertragung herstellerspezifischer Parameter.

### 5.1. Downlink: Userkonfiguration

- **MSG_VENDOR_ENABLE:** Aktiviert den Userkonfigurations-Modus für einen Knoten.
- **MSG_VENDOR_DISABLE:** Deaktiviert den Userkonfigurations-Modus.
- **MSG_VENDOR_SET:** Setzt einen herstellerspezifischen Parameter.
- **MSG_VENDOR_GET:** Liest einen herstellerspezifischen Parameter.
- **MSG_STRING_SET:** Sendet einen String an den Knoten.
- **MSG_STRING_GET:** Frägt einen String vom Knoten ab.

### 5.2. Uplink: Userkonfiguration

- **MSG_VENDOR:** Antwort auf `MSG_VENDOR_GET` oder `MSG_VENDOR_SET`.
- **MSG_VENDOR_ACK:** Bestätigung für `MSG_VENDOR_ENABLE` oder `MSG_VENDOR_DISABLE`.
- **MSG_STRING:** Antwort auf `MSG_STRING_GET` oder `MSG_STRING_SET`.
