#include "BiDiB.h"

// CRC8-Prüfsummentabelle für das Polynom 0x31 (Maxim/Dallas)
static const uint8_t crc8_table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 234,
    50, 108, 142, 208, 83, 13, 235, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 238, 121, 39, 197, 155,
    244, 170, 72, 22, 87, 9, 239, 181, 41, 119, 149, 203, 14, 80, 176, 232,
    107, 53, 215, 137, 182, 232, 10, 84, 21, 75, 169, 247, 116, 42, 200, 150,
    52, 106, 136, 214, 246, 168, 74, 20, 85, 11, 233, 183, 151, 201, 43, 117
};


BiDiB::BiDiB() : _messageAvailable(false) {
    // Initialize unique_id with a placeholder.
    // In a real application, this should be a truly unique value.
    unique_id[0] = 0x80; // Vendor ID (e.g., private use)
    unique_id[1] = 0x01; // Product ID
    unique_id[2] = 0x02;
    unique_id[3] = 0x03;
    unique_id[4] = 0x04;
    unique_id[5] = 0x05;
    unique_id[6] = 0x06;
}

void BiDiB::handleMessages() {
    if (!messageAvailable()) {
        return;
    }

    BiDiBMessage msg = getLastMessage();

    switch (msg.msg_type) {
        case MSG_SYS_GET_MAGIC: {
            BiDiBMessage response;
            response.length = 4; // 1 (addr) + 1 (data) + 2 (num, type)
            response.address[0] = 0;
            response.msg_num = msg.msg_num; // Echo message number
            response.msg_type = MSG_SYS_MAGIC;
            response.data[0] = 0xAF; // BIDIB_SYS_MAGIC
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_P_VERSION: {
            BiDiBMessage response;
            response.length = 5; // 1 (addr) + 2 (data) + 2 (num, type)
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_P_VERSION;
            response.data[0] = protocol_version[1]; // Minor
            response.data[1] = protocol_version[0]; // Major
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_UNIQUE_ID: {
            BiDiBMessage response;
            response.length = 10; // 1 (addr) + 7 (data) + 2 (num, type)
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_UNIQUE_ID;
            for(int i=0; i<7; ++i) {
                response.data[i] = unique_id[i];
            }
            sendMessage(response);
            break;
        }
    }
}

void BiDiB::updateCrc(uint8_t byte, uint8_t &crc) {
    crc = crc8_table[crc ^ byte];
}

void BiDiB::sendByte(uint8_t byte, uint8_t &crc) {
    updateCrc(byte, crc);
    if (byte == BIDIB_MAGIC || byte == BIDIB_ESCAPE) {
        bidib_serial->write(BIDIB_ESCAPE);
        bidib_serial->write(byte ^ 0x20);
    } else {
        bidib_serial->write(byte);
    }
}

void BiDiB::sendMessage(const BiDiBMessage& msg) {
    uint8_t crc = 0;

    bidib_serial->write(BIDIB_MAGIC);

    sendByte(msg.length, crc);

    // Adress-Stack senden (falls vorhanden)
    uint8_t addr_len = 0;
    for (int i = 0; i < 4; i++) {
        addr_len++;
        if (msg.address[i] == 0) {
            break;
        }
    }

    for (int i = 0; i < addr_len; ++i) {
        sendByte(msg.address[i], crc);
    }

    sendByte(msg.msg_num, crc);
    sendByte(msg.msg_type, crc);

    // Daten-Bytes senden
    uint8_t data_len = msg.length - addr_len - 2; // Länge - Adresslänge - MSG_NUM - MSG_TYPE
    for (int i = 0; i < data_len; ++i) {
        sendByte(msg.data[i], crc);
    }

    // CRC senden (wird ebenfalls escaped)
    if (crc == BIDIB_MAGIC || crc == BIDIB_ESCAPE) {
        bidib_serial->write(BIDIB_ESCAPE);
        bidib_serial->write(crc ^ 0x20);
    } else {
        bidib_serial->write(crc);
    }

    bidib_serial->write(BIDIB_MAGIC);
}

bool BiDiB::receiveMessage(BiDiBMessage& msg) {
    if (bidib_serial->read() != BIDIB_MAGIC) {
        return false;
    }

    uint8_t crc = 0;

    // Helper lambda to read a content byte, handle un-escaping, and update CRC
    auto readContentByte = [&]() {
        uint8_t byte = bidib_serial->read();
        if (byte == BIDIB_ESCAPE) {
            byte = bidib_serial->read() ^ 0x20;
        }
        updateCrc(byte, crc);
        return byte;
    };

    msg.length = readContentByte();

    uint8_t addr_len = 0;
    for (int i = 0; i < 4; ++i) {
        msg.address[i] = readContentByte();
        addr_len++;
        if (msg.address[i] == 0) {
            break;
        }
    }

    msg.msg_num = readContentByte();
    msg.msg_type = readContentByte();

    uint8_t data_len = msg.length - addr_len - 2;
    for (int i = 0; i < data_len; ++i) {
        msg.data[i] = readContentByte();
    }

    // Read the CRC byte, which could also be escaped
    uint8_t received_crc = bidib_serial->read();
    if (received_crc == BIDIB_ESCAPE) {
        received_crc = bidib_serial->read() ^ 0x20;
    }

    // The next byte must be the trailing MAGIC
    if (bidib_serial->read() != BIDIB_MAGIC) {
        return false; // Malformed message
    }

    return crc == received_crc;
}

void BiDiB::begin(Stream &serial) {
    bidib_serial = &serial;
}

void BiDiB::update() {
    if (bidib_serial->available() > 0) {
        if (receiveMessage(_lastMessage)) {
            _messageAvailable = true;
        }
    }
}

bool BiDiB::messageAvailable() {
    return _messageAvailable;
}

BiDiBMessage BiDiB::getLastMessage() {
    _messageAvailable = false; // Reset the flag
    return _lastMessage;
}
