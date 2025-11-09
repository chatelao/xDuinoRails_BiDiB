#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

// BiDiB-Protokoll-Konstanten
const uint8_t BIDIB_MAGIC = 0xFE;
const uint8_t BIDIB_ESCAPE = 0xFD;

// Datenstruktur für eine BiDiB-Nachricht
struct BiDiBMessage {
    uint8_t length;
    uint8_t address[4];
    uint8_t msg_num;
    uint8_t msg_type;
    uint8_t data[64]; // Maximale Datenlänge annehmen
};

class BiDiB {
public:
    BiDiB();

    // Initialisiert die BiDiB-Instanz mit einem seriellen Port
    void begin(Stream &serial);

    // Verarbeitet eingehende Daten und muss regelmäßig aufgerufen werden
    void update();

    // Sendet eine vollständige, formatierte BiDiB-Nachricht
    void sendMessage(const BiDiBMessage& msg);

private:
    // Empfängt und validiert eine BiDiB-Nachricht
    bool receiveMessage(BiDiBMessage& msg);

    // Sendet ein einzelnes Byte und wendet bei Bedarf das Escaping an
    void sendByte(uint8_t byte, uint8_t &crc);

    // Hilfsmethode zur Aktualisierung der CRC-Prüfsumme
    void updateCrc(uint8_t byte, uint8_t &crc);

    Stream* bidib_serial;
};

#endif
