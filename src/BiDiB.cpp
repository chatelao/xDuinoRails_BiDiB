#include "BiDiB.h"
#include <string.h>

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


BiDiB::BiDiB() : _messageAvailable(false), _isLoggedIn(false), _system_enabled(true) {
    // Initialize unique_id with a placeholder.
    unique_id[0] = 0x80; unique_id[1] = 0x01; unique_id[2] = 0x02;
    unique_id[3] = 0x03; unique_id[4] = 0x04; unique_id[5] = 0x05;
    unique_id[6] = 0x06;

    for (int i=0; i<7; ++i) { _local_node.unique_id[i] = unique_id[i]; }
    _node_table[0] = _local_node; // The host itself is always node 0

    node_table_version = 0;
    _node_count = 1; // Start with 1 node (the host itself)
    _feature_count = 0;
    _next_feature_index = 0;

    // Initialize default features
    setFeature(BIDIB_FEATURE_FW_UPDATE_SUPPORT, 1);
    setFeature(BIDIB_FEATURE_STRING_SIZE, 32);
    setFeature(BIDIB_FEATURE_MSG_RECEIVE_COUNT, 4);
}

void BiDiB::setFeature(uint8_t feature_num, uint8_t value) {
    for (int i = 0; i < _feature_count; ++i) {
        if (_features[i].feature_num == feature_num) {
            _features[i].value = value;
            return;
        }
    }
    if (_feature_count < BIDIB_MAX_FEATURES) {
        _features[_feature_count].feature_num = feature_num;
        _features[_feature_count].value = value;
        _feature_count++;
    }
}

uint8_t BiDiB::getFeature(uint8_t feature_num) {
    for (int i = 0; i < _feature_count; ++i) {
        if (_features[i].feature_num == feature_num) {
            return _features[i].value;
        }
    }
    return 0; // Return 0 if feature not found
}

void BiDiB::logon() {
    BiDiBMessage msg;
    msg.length = 11;
    msg.address[0] = 0;
    msg.msg_num = 0; // Logon uses 0
    msg.msg_type = MSG_LOGON;
    for (int i=0; i<7; ++i) { msg.data[i] = _local_node.unique_id[i]; }
    sendMessage(msg);
}

void BiDiB::enable() {
    BiDiBMessage msg;
    msg.length = 3;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_SYS_ENABLE;
    sendMessage(msg);
}

void BiDiB::disable() {
    BiDiBMessage msg;
    msg.length = 3;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_SYS_DISABLE;
    sendMessage(msg);
}

void BiDiB::handleMessages() {
    if (!messageAvailable()) { return; }

    BiDiBMessage msg = getLastMessage();

    // Handle system enable/disable messages regardless of the current state
    if (msg.msg_type == MSG_SYS_ENABLE) {
        _system_enabled = true;
        return;
    } else if (msg.msg_type == MSG_SYS_DISABLE) {
        _system_enabled = false;
        return;
    }

    if (!_system_enabled) { return; }

    switch (msg.msg_type) {
        case MSG_SYS_GET_MAGIC: {
            BiDiBMessage response;
            response.length = 4;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_MAGIC;
            response.data[0] = 0xAF;
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_P_VERSION: {
            BiDiBMessage response;
            response.length = 5;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_P_VERSION;
            response.data[0] = protocol_version[1];
            response.data[1] = protocol_version[0];
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_UNIQUE_ID: {
            BiDiBMessage response;
            response.length = 10;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_UNIQUE_ID;
            for(int i=0; i<7; ++i) { response.data[i] = unique_id[i]; }
            sendMessage(response);
            break;
        }
        case MSG_NODETAB_GETALL: {
            if (_isLoggedIn) {
                BiDiBMessage response;
                response.length = 5;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_NODETAB_COUNT;
                response.data[0] = node_table_version;
                response.data[1] = _node_count;
                sendMessage(response);
            }
            break;
        }
        case MSG_NODETAB_GETNEXT: {
            uint8_t requested_node_index = msg.data[0];
            if (_isLoggedIn && requested_node_index < _node_count) {
                BiDiBMessage response;
                response.length = 12;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_NODETAB;
                response.data[0] = node_table_version;
                response.data[1] = requested_node_index;
                for(int i=0; i<7; ++i) { response.data[i+2] = _node_table[requested_node_index].unique_id[i]; }
                sendMessage(response);
            } else {
                BiDiBMessage response;
                response.length = 4;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_NODE_NA;
                response.data[0] = requested_node_index;
                sendMessage(response);
            }
            break;
        }
        case MSG_LOGON: {
            if (findNode(msg.data) != -1) { break; } // Node already logged on.
            if (_node_count >= BIDIB_MAX_NODES) { break; } // Node table full.

            // 1. Add node to table
            BiDiBNode newNode;
            for(int i=0; i<7; ++i) { newNode.unique_id[i] = msg.data[i]; }
            _node_table[_node_count] = newNode;

            // 2. Send LOGON_ACK to the new node
            BiDiBMessage ack;
            ack.length = 12;
            ack.address[0] = 0;
            ack.msg_num = msg.msg_num;
            ack.msg_type = MSG_LOGON_ACK;
            ack.data[0] = node_table_version;
            ack.data[1] = _node_count; // The new node's address
            for(int i=0; i<7; ++i) { ack.data[i+2] = msg.data[i]; }
            sendMessage(ack);

            // 3. Send NODE_NEW to all other nodes (broadcast)
            BiDiBMessage nodeNew;
            nodeNew.length = 12;
            nodeNew.address[0] = 0; // Broadcast
            nodeNew.msg_num = 0; // System message
            nodeNew.msg_type = MSG_NODE_NEW;
            nodeNew.data[0] = node_table_version;
            nodeNew.data[1] = _node_count; // The new node's address
            for(int i=0; i<7; ++i) { nodeNew.data[i+2] = msg.data[i]; }
            sendMessage(nodeNew);

            _node_count++;
            break;
        }
        case MSG_LOGON_ACK: {
            _isLoggedIn = true;
            _node_count = 1;
            break;
        }
        case MSG_FEATURE_GETALL: {
            _next_feature_index = 0; // Reset index for GETNEXT sequence
            BiDiBMessage response;
            response.length = 4;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_FEATURE_COUNT;
            response.data[0] = _feature_count;
            sendMessage(response);
            break;
        }
        case MSG_FEATURE_GETNEXT: {
            if (_next_feature_index < _feature_count) {
                BiDiBMessage response;
                response.length = 5;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_FEATURE;
                response.data[0] = _features[_next_feature_index].feature_num;
                response.data[1] = _features[_next_feature_index].value;
                sendMessage(response);
                _next_feature_index++;
            } else {
                BiDiBMessage response;
                response.length = 4;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_FEATURE_NA;
                response.data[0] = 255; // End of list
                sendMessage(response);
                _next_feature_index = 0; // Reset after sequence
            }
            break;
        }
        case MSG_FEATURE_GET: {
            uint8_t feature_num = msg.data[0];
            bool found = false;
            for (int i = 0; i < _feature_count; ++i) {
                if (_features[i].feature_num == feature_num) {
                    BiDiBMessage response;
                    response.length = 5;
                    response.address[0] = 0;
                    response.msg_num = msg.msg_num;
                    response.msg_type = MSG_FEATURE;
                    response.data[0] = _features[i].feature_num;
                    response.data[1] = _features[i].value;
                    sendMessage(response);
                    found = true;
                    break;
                }
            }
            if (!found) {
                BiDiBMessage response;
                response.length = 4;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_FEATURE_NA;
                response.data[0] = feature_num;
                sendMessage(response);
            }
            break;
        }
        case MSG_FEATURE_SET: {
            uint8_t feature_num = msg.data[0];
            uint8_t value = msg.data[1];
            setFeature(feature_num, value);

            // Respond with the new value
            BiDiBMessage response;
            response.length = 5;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_FEATURE;
            response.data[0] = feature_num;
            response.data[1] = getFeature(feature_num);
            sendMessage(response);
            break;
        }
    }
}

int BiDiB::findNode(const uint8_t* unique_id) {
    for (int i = 0; i < _node_count; ++i) {
        if (memcmp(_node_table[i].unique_id, unique_id, 7) == 0) {
            return i; // Node found
        }
    }
    return -1; // Node not found
}

uint8_t BiDiB::calculateCrc(const uint8_t* data, size_t size) {
    uint8_t crc = 0;
    for (size_t i = 0; i < size; ++i) {
        updateCrc(data[i], crc);
    }
    return crc;
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
    uint8_t addr_len = 0;
    for (int i = 0; i < 4; i++) {
        addr_len++;
        if (msg.address[i] == 0) break;
    }
    for (int i = 0; i < addr_len; ++i) { sendByte(msg.address[i], crc); }
    sendByte(msg.msg_num, crc);
    sendByte(msg.msg_type, crc);
    uint8_t data_len = msg.length - addr_len - 2;
    for (int i = 0; i < data_len; ++i) { sendByte(msg.data[i], crc); }
    if (crc == BIDIB_MAGIC || crc == BIDIB_ESCAPE) {
        bidib_serial->write(BIDIB_ESCAPE);
        bidib_serial->write(crc ^ 0x20);
    } else {
        bidib_serial->write(crc);
    }
    bidib_serial->write(BIDIB_MAGIC);
}

bool BiDiB::receiveMessage(BiDiBMessage& msg) {
    if (bidib_serial->read() != BIDIB_MAGIC) { return false; }
    uint8_t crc = 0;
    auto readContentByte = [&]() {
        uint8_t byte = bidib_serial->read();
        if (byte == BIDIB_ESCAPE) { byte = bidib_serial->read() ^ 0x20; }
        updateCrc(byte, crc);
        return byte;
    };
    msg.length = readContentByte();
    uint8_t addr_len = 0;
    for (int i = 0; i < 4; ++i) {
        msg.address[i] = readContentByte();
        addr_len++;
        if (msg.address[i] == 0) break;
    }
    msg.msg_num = readContentByte();
    msg.msg_type = readContentByte();
    uint8_t data_len = msg.length - addr_len - 2;
    for (int i = 0; i < data_len; ++i) { msg.data[i] = readContentByte(); }
    uint8_t received_crc = bidib_serial->read();
    if (received_crc == BIDIB_ESCAPE) { received_crc = bidib_serial->read() ^ 0x20; }
    if (bidib_serial->read() != BIDIB_MAGIC) { return false; }
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
    _messageAvailable = false;
    return _lastMessage;
}
