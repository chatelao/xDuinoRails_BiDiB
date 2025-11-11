#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

// BiDiB-Protokoll-Konstanten
const uint8_t BIDIB_MAGIC = 0xFE;
const uint8_t BIDIB_ESCAPE = 0xFD;

// BiDiB Message Types
// --- System Messages ---
const uint8_t MSG_SYS_GET_MAGIC     = 1;
const uint8_t MSG_SYS_GET_P_VERSION = 2;
const uint8_t MSG_SYS_GET_UNIQUE_ID = 3;
const uint8_t MSG_NODETAB_GETALL    = 6;
const uint8_t MSG_NODETAB_GETNEXT   = 7;
const uint8_t MSG_SYS_MAGIC         = 129; // 0x81
const uint8_t MSG_SYS_P_VERSION     = 130; // 0x82
const uint8_t MSG_SYS_UNIQUE_ID     = 131; // 0x83
const uint8_t MSG_NODETAB_COUNT     = 134; // 0x86
const uint8_t MSG_NODETAB           = 135; // 0x87
const uint8_t MSG_NODE_NA           = 136; // 0x88


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

    // Verarbeitet die letzte empfangene Nachricht
    void handleMessages();

    // Sendet eine vollständige, formatierte BiDiB-Nachricht
    void sendMessage(const BiDiBMessage& msg);

    // Returns true if a message has been received and is waiting to be processed
    bool messageAvailable();

    // Returns the last received message
    BiDiBMessage getLastMessage();

    // Helper to calculate CRC for testing
    uint8_t calculateCrc(const uint8_t* data, size_t size);

    // Node properties
    uint8_t unique_id[7];
    uint8_t node_table_version;
    uint8_t node_count;

private:
    // Empfängt und validiert eine BiDiB-Nachricht
    bool receiveMessage(BiDiBMessage& msg);

    // Sendet ein einzelnes Byte und wendet bei Bedarf das Escaping an
    void sendByte(uint8_t byte, uint8_t &crc);

    // Hilfsmethode zur Aktualisierung der CRC-Prüfsumme
    void updateCrc(uint8_t byte, uint8_t &crc);

    Stream* bidib_serial;
    BiDiBMessage _lastMessage;
    bool _messageAvailable;
    uint8_t protocol_version[2] = {0, 1}; // V 0.1
};

#endif
