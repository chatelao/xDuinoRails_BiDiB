# BiDiB - Serielle Verbindung

Dieses Dokument beschreibt den Transport von BiDiB-Nachrichten über eine serielle Schnittstelle (RS232 oder virtueller COM-Port via USB).

## 1. Übertragungsparameter

-   **Baudrate:** 19200, 115200 oder 1 Mbaud (bevorzugt). Die korrekte Rate wird vom Host beim Verbindungsaufbau durch Abfrage der System-ID (`MSG_SYS_GET_MAGIC`) ermittelt.
-   **Parameter:** 8 Datenbits, 1 Stopbit, keine Parity (8N1).
-   **Flusskontrolle:** CTS und RTS werden verwendet.

## 2. Framing und Datensicherung

Ein serielles Paket ist wie folgt aufgebaut:
`[MAGIC] MESSAGE_SEQ CRC MAGIC`

-   **`MAGIC` (0xFE):** Ein spezielles Byte, das den Anfang und das Ende eines Pakets markiert. Das `MAGIC`-Byte am Ende eines Pakets kann gleichzeitig der Anfang des nächsten sein.
-   **`MESSAGE_SEQ`:** Eine Sequenz von einer oder mehreren vollständigen BiDiB-Nachrichten.
-   **`CRC` (CRC8):** Eine Prüfsumme, die über die `MESSAGE_SEQ` berechnet wird, um die Datenintegrität zu sichern.

### Escaping

Um zu verhindern, dass Datenbytes innerhalb der `MESSAGE_SEQ` oder der `CRC` fälschlicherweise als `MAGIC` (0xFE) oder `ESCAPE` (0xFD) interpretiert werden, wird ein Escape-Mechanismus verwendet:
-   Ein `MAGIC`-Byte im Datenstrom wird durch die Sequenz `0xFD 0xDE` ersetzt (`0xDE = 0xFE ^ 0x20`).
-   Ein `ESCAPE`-Byte im Datenstrom wird durch die Sequenz `0xFD 0xDD` ersetzt (`0xDD = 0xFD ^ 0x20`).

## 3. Verbindungsaufbau und -kontrolle

Die Verbindung zwischen Host und Interface ist eine Punkt-zu-Punkt-Verbindung.

-   **Verbindungsaufbau:** Der Host kann die Verbindung direkt mit `MSG_SYS_GET_MAGIC` initiieren.
-   **Verlust der Verbindung:** Wenn ein Knoten länger als 5 Sekunden keine Nachricht vom Host empfängt, nimmt er einen Verbindungsabbruch an. Der Host sollte daher spätestens alle 4 Sekunden eine Nachricht (ggf. `MSG_LOCAL_PING`) senden.
-   **Logon/Logoff:** Die lokalen Nachrichten `MSG_LOCAL_LOGON`, `MSG_LOCAL_LOGON_ACK`, `MSG_LOCAL_LOGON_REJECTED` und `MSG_LOCAL_LOGOFF` werden verwendet, um das Kontrollverhältnis zwischen Host (Interface) und Knoten zu steuern.

## 4. Lokale Nachrichten

-   **`MSG_LOCAL_PING` / `MSG_LOCAL_PONG`:** Dienen der Aufrechterhaltung der Verbindung, wenn keine anderen Daten gesendet werden.
-   **`MSG_LOCAL_SYNC`:** Dient zur Übertragung der Systemzeit vom Host zum Knoten, um zeitbasierte Messungen zu synchronisieren.

## 5. Code-Beispiel: Senden einer Nachricht

```c
void send_bidib_message(unsigned char *message)
{
  unsigned char i=0;
  unsigned char length;
  unsigned char tx_crc = 0;

  bidib_send_delimiter(); // Sendet start MAGIC (0xFE)

  length = message[0];
  bidib_send(length); // Sendet MSG_LENGTH (mit escaping)
  tx_crc = crc_array[length ^ tx_crc];

  for (i=1; i<=length; i++)
  {
    bidib_send(message[i]); // Sendet Datenbytes (mit escaping)
    tx_crc = crc_array[message[i] ^ tx_crc];
  }
  bidib_send(tx_crc); // Sendet CRC (mit escaping)
  bidib_send_delimiter(); // Sendet ende MAGIC (0xFE)
}
```
