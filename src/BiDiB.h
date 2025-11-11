#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

// BiDiB-Protokoll-Konstanten
const uint8_t BIDIB_MAGIC = 0xFE;
const uint8_t BIDIB_ESCAPE = 0xFD;

// BiDiB Message Types
// --- System Messages ---
const uint8_t BIDIB_MAX_NODES       = 32;
const uint8_t MSG_SYS_GET_MAGIC     = 1;
const uint8_t MSG_SYS_GET_P_VERSION = 2;
const uint8_t MSG_SYS_GET_UNIQUE_ID = 3;
const uint8_t MSG_SYS_ENABLE        = 4;
const uint8_t MSG_SYS_DISABLE       = 5;
const uint8_t MSG_NODETAB_GETALL    = 6;
const uint8_t MSG_NODETAB_GETNEXT   = 7;
const uint8_t MSG_LOGON             = 10;
const uint8_t MSG_SYS_MAGIC         = 129; // 0x81
const uint8_t MSG_SYS_P_VERSION     = 130; // 0x82
const uint8_t MSG_SYS_UNIQUE_ID     = 131; // 0x83
const uint8_t MSG_NODETAB_COUNT     = 134; // 0x86
const uint8_t MSG_NODETAB           = 135; // 0x87
const uint8_t MSG_NODE_NA           = 136; // 0x88
const uint8_t MSG_NODE_NEW          = 137; // 0x89
const uint8_t MSG_NODE_LOST         = 138; // 0x8A
const uint8_t MSG_LOGON_ACK         = 139; // 0x8B

// --- Feature Messages ---
const uint8_t MSG_FEATURE_GETALL    = 11;
const uint8_t MSG_FEATURE_GETNEXT   = 12;
const uint8_t MSG_FEATURE_GET       = 13;
const uint8_t MSG_FEATURE_SET       = 14;
const uint8_t MSG_FEATURE_COUNT     = 140; // 0x8C
const uint8_t MSG_FEATURE           = 141; // 0x8D
const uint8_t MSG_FEATURE_NA        = 142; // 0x8E


// Datenstruktur für eine BiDiB-Nachricht
struct BiDiBMessage {
    uint8_t length;
    uint8_t address[4];
    uint8_t msg_num;
    uint8_t msg_type;
    uint8_t data[64]; // Maximale Datenlänge annehmen
};

const uint8_t BIDIB_MAX_FEATURES    = 16;

// --- Feature Constants ---
const uint8_t BIDIB_FEATURE_FW_UPDATE_SUPPORT = 0; // 1 = supported
const uint8_t BIDIB_FEATURE_STRING_SIZE       = 1; // max size of strings
const uint8_t BIDIB_FEATURE_MSG_RECEIVE_COUNT = 2; // how many msgs can be received


struct BiDiBNode {
    uint8_t unique_id[7];
};

struct BiDiBFeature {
    uint8_t feature_num;
    uint8_t value;
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
    virtual void sendMessage(const BiDiBMessage& msg);

    // Returns true if a message has been received and is waiting to be processed
    bool messageAvailable();

    // Returns the last received message
    BiDiBMessage getLastMessage();

    // Helper to calculate CRC for testing
    uint8_t calculateCrc(const uint8_t* data, size_t size);

    // Processes logon requests
    void logon();

    // Enable and disable system messages
    void enable();
    void disable();

    // Set a feature value
    void setFeature(uint8_t feature_num, uint8_t value);

    // Get a feature value
    uint8_t getFeature(uint8_t feature_num);

    // Node properties
    uint8_t unique_id[7];
    uint8_t node_table_version;

protected:
    BiDiBMessage _lastMessage;
    bool _messageAvailable;
    bool _system_enabled;
    BiDiBFeature _features[BIDIB_MAX_FEATURES];
    uint8_t _feature_count;
    uint8_t _next_feature_index;
    BiDiBNode _local_node;
    BiDiBNode _node_table[BIDIB_MAX_NODES];
    uint8_t _node_count;
    bool _isLoggedIn;

private:
    // Empfängt und validiert eine BiDiB-Nachricht
    bool receiveMessage(BiDiBMessage& msg);

    // Find a node by unique_id in the node table
    int findNode(const uint8_t* unique_id);

    // Sendet ein einzelnes Byte und wendet bei Bedarf das Escaping an
    void sendByte(uint8_t byte, uint8_t &crc);

    // Hilfsmethode zur Aktualisierung der CRC-Prüfsumme
    void updateCrc(uint8_t byte, uint8_t &crc);

    Stream* bidib_serial;
    uint8_t protocol_version[2] = {0, 1}; // V 0.1
};

#endif
