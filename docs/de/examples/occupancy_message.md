# BiDiB - Beispiel: Belegtmeldung vom Knoten zum Host

Dieses Dokument beschreibt den detaillierten Weg einer Belegtmeldung (`MSG_BM_OCC`) von einem Knoten (z.B. einem Belegtmelder) über den BiDiBus zum Interface (Host).

## Szenario

Ein Belegtmelder mit der lokalen `NODE_ADDR` **4** auf dem BiDiBus sendet seine **12.** Nachricht (`MSG_NUM` = 12), um zu melden, dass sein Melder mit der Nummer **5** (`MNUM` = 5) nun belegt ist.

---

### 1. Logische Nachricht (Uplink)

Die zu übertragende Information ist eine Belegtmeldung.

-   **Nachrichtentyp:** `MSG_BM_OCC` (dezimal 162, hexadezimal `0xA2`)
-   **Parameter:** Meldernummer `MNUM` = **5**

---

### 2. BiDiB-Nachricht (`MESSAGE`)

Der Knoten stellt die vollständige BiDiB-Nachricht zusammen. Da die Nachricht vom Knoten selbst stammt und an das übergeordnete Interface geht, ist der Adress-Stack leer (`MSG_ADDR` = `0x00`).

-   **`MSG_LENGTH`:** 4 (Länge ab `MSG_ADDR`)
-   **`MSG_ADDR`:** `0x00` (Nachricht stammt vom Knoten selbst)
-   **`MSG_NUM`:** 12
-   **`MSG_TYPE`:** `0xA2` (`MSG_BM_OCC`)
-   **`DATA`:** `0x05` (Parameter `MNUM`)

**Die `MESSAGE`-Sequenz in Bytes:**
`0x04 0x00 0x0C 0xA2 0x05`

---

### 3. BiDiBus-Paket (`BIDIBUS_PKT`)

Für die Übertragung auf dem BiDiBus wird die `MESSAGE` in ein `BIDIBUS_PKT` verpackt.

1.  **`P_LENGTH` (Paketlänge):** Die Länge der `MESSAGE_SEQ`. Da nur eine Nachricht gesendet wird, entspricht dies der Länge der `MESSAGE`, also **5** Bytes.
2.  **`MESSAGE_SEQ`:** Die oben erstellte Byte-Sequenz.
3.  **`CRC8` (Prüfsumme):** Die CRC8-Prüfsumme wird über `P_LENGTH` und die `MESSAGE_SEQ` berechnet.
    -   Zu berechnende Bytes: `0x05 0x04 0x00 0x0C 0xA2 0x05`
    -   Das Ergebnis der CRC8-Berechnung (Polynom `0x31`) ist **`0x1B`**.

**Das vollständige `BIDIBUS_PKT` in Bytes:**
`0x05 0x04 0x00 0x0C 0xA2 0x05 0x1B`

---

### 4. Bit-Ebene (Übertragung auf RS485)

Der Knoten wartet, bis er vom Interface mit seiner Adresse (`NODE_ADDR` = 4) gepollt wird. Sobald er die Sendeerlaubnis hat, sendet er das `BIDIBUS_PKT` als serielle Daten (500 kBaud, 9N1) auf den RS485-Bus.

**Die finale Byte-Sequenz, die auf dem Bus übertragen wird:**
`0x05 0x04 0x00 0x0C 0xA2 0x05 0x1B`

Das Interface empfängt dieses Paket, prüft die CRC, fügt die `NODE_ADDR` des sendenden Knotens (4) zum Adress-Stack hinzu und leitet die Nachricht an den Host weiter.
