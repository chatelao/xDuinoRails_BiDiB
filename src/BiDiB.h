#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

// BiDiB-Protokoll-Konstanten
const uint8_t BIDIB_MAGIC = 0xFE;
const uint8_t BIDIB_ESCAPE = 0xFD;

class BiDiB {
public:
    BiDiB();

    // Sendet eine vollständige, formatierte BiDiB-Nachricht
    void sendMessage(uint8_t length, uint8_t* address, uint8_t msg_num, uint8_t msg_type, uint8_t* data, Stream &serial);

    // Empfängt und validiert eine BiDiB-Nachricht (noch nicht implementiert)
    bool receiveMessage(Stream &serial);

private:
    // Sendet ein einzelnes Byte und wendet bei Bedarf das Escaping an
    void sendByte(uint8_t byte, Stream &serial, uint8_t &crc);

    // Hilfsmethode zur Aktualisierung der CRC-Prüfsumme
    void updateCrc(uint8_t byte, uint8_t &crc);
};

#endif
