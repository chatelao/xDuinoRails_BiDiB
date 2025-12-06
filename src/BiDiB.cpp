#include "BiDiB.h"
#include "crc8.h"
#include <string.h>

BiDiB::BiDiB() : _messageAvailable(false), _system_enabled(true), _isLoggedIn(false) {
    // Initialize unique_id with a default placeholder value.
    // IMPORTANT: The user should set a truly unique ID in their setup() function.
    unique_id[0] = 0x80; unique_id[1] = 0x01; unique_id[2] = 0x02;
    unique_id[3] = 0x03; unique_id[4] = 0x04; unique_id[5] = 0x05;
    unique_id[6] = 0x06;

    // Copy the unique ID to the local node representation.
    for (int i=0; i<7; ++i) { _local_node.unique_id[i] = unique_id[i]; }

    // The host itself is always considered the first node in the table.
    _node_table[0] = _local_node;

    node_table_version = 0;
    _node_count = 1; // Start with 1 node (the host itself)
    _feature_count = 0;
    _next_feature_index = 0;

    // Initialize default features as per BiDiB specification.
    setFeature(BIDIB_FEATURE_FW_UPDATE_SUPPORT, 1);
    setFeature(BIDIB_FEATURE_STRING_SIZE, 32);
    setFeature(BIDIB_FEATURE_MSG_RECEIVE_COUNT, 4);

    _track_state = BIDIB_CS_STATE_OFF;
    _driveAckCallback = nullptr;
    _accessoryAckCallback = nullptr;
    _pomAckCallback = nullptr;
    _boosterStatusCallback = nullptr;
    _boosterDiagnosticCallback = nullptr;
    _occupancyCallback = nullptr;
    _occupancyMultipleCallback = nullptr;
    _addressCallback = nullptr;
    _accessoryStateCallback = nullptr;
    _firmwareUpdateStatusCallback = nullptr;

    // Initialize LC Callbacks
    _lcStatCallback = nullptr;
    _lcConfigXCallback = nullptr;
    _lcNaCallback = nullptr;
    _lcWaitCallback = nullptr;
    _lcMacroStateCallback = nullptr;
    _lcMacroCallback = nullptr;
    _lcMacroParaCallback = nullptr;

    // Initialize the pending Secure-ACKs list.
    for (int i = 0; i < MAX_PENDING_SECURE_ACKS; ++i) {
        _pendingSecureAcks[i].active = false;
    }

    // Initialize receiver FSM state
    _rx_state = FSM_IDLE;
    _rx_ptr = 0;
}

// =============================================================================
// Command Station Functions
// =============================================================================

void BiDiB::drive(uint16_t address, int8_t speed, uint8_t functions) {
    BiDiBMessage msg;
    msg.length = 8;
    msg.address[0] = 0; // Broadcast to command station
    msg.msg_num = 0;
    msg.msg_type = MSG_CS_DRIVE;
    msg.data[0] = address & 0xFF;
    msg.data[1] = (address >> 8) & 0xFF;
    msg.data[2] = 2; // Speed format: DCC128
    msg.data[3] = speed;
    msg.data[4] = functions;
    sendMessage(msg);
}

void BiDiB::onDriveAck(DriveAckCallback callback) {
    _driveAckCallback = callback;
}

void BiDiB::accessory(uint16_t address, uint8_t output, uint8_t state) {
    BiDiBMessage msg;
    msg.length = 7;
    msg.address[0] = 0; // Broadcast to command station
    msg.msg_num = 0;
    msg.msg_type = MSG_CS_ACCESSORY;
    msg.data[0] = address & 0xFF;
    msg.data[1] = (address >> 8) & 0xFF;
    msg.data[2] = output;
    msg.data[3] = state;
    sendMessage(msg);
}

void BiDiB::onAccessoryAck(AccessoryAckCallback callback) {
    _accessoryAckCallback = callback;
}

void BiDiB::pomWriteByte(uint16_t address, uint16_t cv, uint8_t value) {
    BiDiBMessage msg;
    msg.length = 15;
    msg.address[0] = 0; // Broadcast to command station
    msg.msg_num = 0;
    msg.msg_type = MSG_CS_POM;
    msg.data[0] = address & 0xFF;
    msg.data[1] = (address >> 8) & 0xFF;
    msg.data[2] = 0; // ADDR_XL
    msg.data[3] = 0; // ADDR_XH
    msg.data[4] = 0; // MID
    msg.data[5] = BIDIB_CS_POM_WR_BYTE; // OPCODE
    msg.data[6] = (cv - 1) & 0xFF; // CV_L
    msg.data[7] = ((cv - 1) >> 8) & 0xFF; // CV_H
    msg.data[8] = 0; // CV_X
    msg.data[9] = value;
    sendMessage(msg);
}

void BiDiB::onPomAck(PomAckCallback callback) {
    _pomAckCallback = callback;
}

void BiDiB::setTrackState(uint8_t state) {
    BiDiBMessage msg;
    msg.length = 4;
    msg.address[0] = 0; // Broadcast to command station
    msg.msg_num = 0;
    msg.msg_type = MSG_CS_SET_STATE;
    msg.data[0] = state;
    sendMessage(msg);
}

// =============================================================================
// Booster Functions
// =============================================================================

void BiDiB::setBoosterState(bool on, uint8_t node_addr) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.msg_num = 0;
    msg.msg_type = on ? MSG_BOOST_ON : MSG_BOOST_OFF;
    msg.length = (node_addr == 0) ? 3 : 4;
    sendMessage(msg);
}

void BiDiB::queryBooster(uint8_t node_addr) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.length = (node_addr == 0) ? 3 : 4;
    msg.msg_num = 0;
    msg.msg_type = MSG_BOOST_QUERY;
    sendMessage(msg);
}

void BiDiB::onBoosterStatus(BoosterStatusCallback callback) {
    _boosterStatusCallback = callback;
}

void BiDiB::onBoosterDiagnostic(BoosterDiagnosticCallback callback) {
    _boosterDiagnosticCallback = callback;
}

// =============================================================================
// Vendor-Specific Functions
// =============================================================================

void BiDiB::vendorEnable(uint8_t node_addr) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.length = 4;
    msg.msg_num = 0;
    msg.msg_type = MSG_VENDOR_ENABLE;
    sendMessage(msg);
}

void BiDiB::vendorDisable(uint8_t node_addr) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.length = 4;
    msg.msg_num = 0;
    msg.msg_type = MSG_VENDOR_DISABLE;
    sendMessage(msg);
}

void BiDiB::onVendorAck(VendorAckCallback callback) {
    _vendorAckCallback = callback;
}

void BiDiB::vendorGet(uint8_t node_addr, const char* name) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_VENDOR_GET;
    strncpy((char*)msg.data, name, sizeof(msg.data));
    msg.length = 4 + strlen(name) + 1;
    sendMessage(msg);
}

void BiDiB::vendorSet(uint8_t node_addr, const char* name, const char* value) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_VENDOR_SET;

    uint8_t name_len = strlen(name);
    strncpy((char*)msg.data, name, sizeof(msg.data));
    msg.data[name_len] = '=';
    strncpy((char*)msg.data + name_len + 1, value, sizeof(msg.data) - name_len - 1);

    msg.length = 4 + strlen(name) + 1 + strlen(value) + 1;
    sendMessage(msg);
}

void BiDiB::onVendorData(VendorDataCallback callback) {
    _vendorDataCallback = callback;
}

// =============================================================================
// Firmware Update Functions
// =============================================================================

void BiDiB::firmwareUpdateOperation(uint8_t node_addr, uint8_t op, const uint8_t* data, size_t len) {
    BiDiBMessage msg;
    msg.address[0] = node_addr;
    msg.address[1] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_FW_UPDATE_OP;
    msg.data[0] = op;
    if (data != nullptr && len > 0) {
        memcpy(&msg.data[1], data, len);
    }
    // Length = addr_len + msg_num + msg_type + data_payload_len
    // data_payload for this message is [op, ...data]
    uint8_t addr_len = (node_addr == 0) ? 1 : 2;
    msg.length = addr_len + 1 /*msg_num*/ + 1 /*msg_type*/ + (1 + len) /*op + data*/;
    sendMessage(msg);
}

void BiDiB::onFirmwareUpdateStatus(FirmwareUpdateStatusCallback callback) {
    _firmwareUpdateStatusCallback = callback;
}

void BiDiB::enterFirmwareUpdateMode(uint8_t node_addr) {
    // Entering update mode requires the node's unique ID for security.
    // We assume the node is in the local node table.
    int node_index = -1;
    // node_addr is the index in the node table
    if (node_addr < _node_count) {
        node_index = node_addr;
    }

    if (node_index != -1) {
        firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_ENTER, _node_table[node_index].unique_id, 7);
    }
}

void BiDiB::exitFirmwareUpdateMode(uint8_t node_addr) {
    firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_EXIT);
}

void BiDiB::setFirmwareUpdateDestination(uint8_t node_addr, uint8_t destination) {
    firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_SETDEST, &destination, 1);
}

void BiDiB::sendFirmwareUpdateData(uint8_t node_addr, const uint8_t* data, size_t len) {
    firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_DATA, data, len);
}

void BiDiB::signalFirmwareUpdateDone(uint8_t node_addr) {
    firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_DONE);
}

// =============================================================================
// Accessory Control Functions
// =============================================================================

void BiDiB::setAccessory(uint8_t accessoryNum, uint8_t aspect) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0; // Address to the local node, will be filled by master
    msg.msg_num = 0;    // Accessory messages can have sequence numbers, but 0 is fine for simple commands
    msg.msg_type = MSG_ACCESSORY_SET;
    msg.data[0] = accessoryNum;
    msg.data[1] = aspect;
    sendMessage(msg);
}

void BiDiB::getAccessory(uint8_t accessoryNum) {
    BiDiBMessage msg;
    msg.length = 4;
    msg.address[0] = 0; // Address to the local node
    msg.msg_num = 0;
    msg.msg_type = MSG_ACCESSORY_GET;
    msg.data[0] = accessoryNum;
    sendMessage(msg);
}

void BiDiB::onAccessoryState(AccessoryStateCallback callback) {
    _accessoryStateCallback = callback;
}

// =============================================================================
// Occupancy Reporting
// =============================================================================

void BiDiB::onOccupancy(OccupancyCallback callback) {
    _occupancyCallback = callback;
}

void BiDiB::onOccupancyMultiple(OccupancyMultipleCallback callback) {
    _occupancyMultipleCallback = callback;
}

void BiDiB::onAddress(AddressCallback callback) {
    _addressCallback = callback;
}

void BiDiB::onSpeedUpdate(SpeedCallback callback) {
    _speedCallback = callback;
}

void BiDiB::onCvUpdate(CvCallback callback) {
    _cvCallback = callback;
}

void BiDiB::sendOccupancySingle(uint8_t detectorNum, bool occupied) {
    BiDiBMessage msg;
    msg.length = 4;
    msg.address[0] = 0; // The node's own address will be added by the master
    msg.msg_num = 0;    // Occupancy messages use sequence numbers, handled by master
    msg.msg_type = occupied ? MSG_BM_OCC : MSG_BM_FREE;
    msg.data[0] = detectorNum;

    if (getFeature(FEATURE_BM_SECACK_ON)) {
        addPendingSecureAck(msg);
    } else {
        sendMessage(msg);
    }
}

void BiDiB::sendOccupancyMultiple(uint8_t baseNum, uint8_t size, const uint8_t* data) {
    BiDiBMessage msg;
    msg.length = 5 + size; // Base length + size of data
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_BM_MULTIPLE;
    msg.data[0] = baseNum;
    msg.data[1] = size;
    memcpy(&msg.data[2], data, size);

    if (getFeature(FEATURE_BM_SECACK_ON)) {
        addPendingSecureAck(msg);
    } else {
        sendMessage(msg);
    }
}

// =============================================================================
// Feature Management
// =============================================================================

void BiDiB::setFeature(uint8_t feature_num, uint8_t value) {
    // First, check if the feature already exists and update it.
    for (int i = 0; i < _feature_count; ++i) {
        if (_features[i].feature_num == feature_num) {
            _features[i].value = value;
            return;
        }
    }
    // If not, add it as a new feature if there's space.
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
    return 0; // Return 0 if the feature is not found.
}

// =============================================================================
// Control / Switching Functions (LC)
// =============================================================================

void BiDiB::setLcOutput(uint8_t portL, uint8_t portH, uint8_t state) {
    BiDiBMessage msg;
    msg.length = 6;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_OUTPUT;
    msg.data[0] = portL;
    msg.data[1] = portH;
    msg.data[2] = state;
    sendMessage(msg);
}

void BiDiB::queryLcPort(uint8_t portL, uint8_t portH) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_PORT_QUERY;
    msg.data[0] = portL;
    msg.data[1] = portH;
    sendMessage(msg);
}

void BiDiB::getLcConfigX(uint8_t portL, uint8_t portH) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_CONFIGX_GET;
    msg.data[0] = portL;
    msg.data[1] = portH;
    sendMessage(msg);
}

void BiDiB::setLcConfigX(uint8_t portL, uint8_t portH, uint8_t paramCount, const uint8_t* params) {
    // params is expected to be a flat array of [type, value, type, value, ...]

    // Prevent buffer overflow (max data length is 64, minus 2 bytes for port = 62)
    if (paramCount > 62) {
        paramCount = 62;
    }

    BiDiBMessage msg;
    msg.length = 5 + paramCount;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_CONFIGX_SET;
    msg.data[0] = portL;
    msg.data[1] = portH;
    memcpy(&msg.data[2], params, paramCount);
    sendMessage(msg);
}

void BiDiB::handleMacro(uint8_t macroIdx, uint8_t opcode) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_MACRO_HANDLE;
    msg.data[0] = macroIdx;
    msg.data[1] = opcode;
    sendMessage(msg);
}

void BiDiB::setMacro(uint8_t macroIdx, uint8_t itemIdx, uint8_t delay, uint8_t portL, uint8_t portH, uint8_t status) {
    BiDiBMessage msg;
    msg.length = 9;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_MACRO_SET;
    msg.data[0] = macroIdx;
    msg.data[1] = itemIdx;
    msg.data[2] = delay;
    msg.data[3] = portL;
    msg.data[4] = portH;
    msg.data[5] = status;
    sendMessage(msg);
}

void BiDiB::getMacro(uint8_t macroIdx, uint8_t itemIdx) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_MACRO_GET;
    msg.data[0] = macroIdx;
    msg.data[1] = itemIdx;
    sendMessage(msg);
}

void BiDiB::setMacroParameter(uint8_t macroIdx, uint8_t paraIdx, uint32_t value) {
    BiDiBMessage msg;
    msg.length = 9;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_MACRO_PARA_SET;
    msg.data[0] = macroIdx;
    msg.data[1] = paraIdx;
    msg.data[2] = value & 0xFF;
    msg.data[3] = (value >> 8) & 0xFF;
    msg.data[4] = (value >> 16) & 0xFF;
    msg.data[5] = (value >> 24) & 0xFF;
    sendMessage(msg);
}

void BiDiB::getMacroParameter(uint8_t macroIdx, uint8_t paraIdx) {
    BiDiBMessage msg;
    msg.length = 5;
    msg.address[0] = 0;
    msg.msg_num = 0;
    msg.msg_type = MSG_LC_MACRO_PARA_GET;
    msg.data[0] = macroIdx;
    msg.data[1] = paraIdx;
    sendMessage(msg);
}

// --- LC Callbacks ---
void BiDiB::onLcStat(LcStatCallback callback) { _lcStatCallback = callback; }
void BiDiB::onLcConfigX(LcConfigXCallback callback) { _lcConfigXCallback = callback; }
void BiDiB::onLcNa(LcNaCallback callback) { _lcNaCallback = callback; }
void BiDiB::onLcWait(LcWaitCallback callback) { _lcWaitCallback = callback; }
void BiDiB::onLcMacroState(LcMacroStateCallback callback) { _lcMacroStateCallback = callback; }
void BiDiB::onLcMacro(LcMacroCallback callback) { _lcMacroCallback = callback; }
void BiDiB::onLcMacroPara(LcMacroParaCallback callback) { _lcMacroParaCallback = callback; }

// =============================================================================
// System-Level Functions
// =============================================================================

void BiDiB::logon() {
    BiDiBMessage msg;
    msg.length = 11;
    msg.address[0] = 0;
    msg.msg_num = 0; // Logon always uses message number 0
    msg.msg_type = MSG_LOGON;
    memcpy(msg.data, _local_node.unique_id, 7);
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

bool BiDiB::isLoggedIn() {
    return _isLoggedIn;
}

// =============================================================================
// Message Handling
// =============================================================================

void BiDiB::handleMessages() {
    if (!messageAvailable()) { return; }

    // Once we start handling, we consume the message.
    _messageAvailable = false;
    BiDiBMessage msg = _lastMessage;

    // Handle system enable/disable immediately, regardless of the current state.
    if (msg.msg_type == MSG_SYS_ENABLE) {
        _system_enabled = true;
        return;
    } else if (msg.msg_type == MSG_SYS_DISABLE) {
        _system_enabled = false;
        return;
    }

    // If the system is disabled, ignore all other messages.
    if (!_system_enabled) { return; }

    switch (msg.msg_type) {
        // --- Basic System Information ---
        case MSG_SYS_GET_MAGIC: {
            BiDiBMessage response;
            response.length = 4;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_MAGIC;
            response.data[0] = 0xAF; // BiDiB magic value
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_P_VERSION: {
            BiDiBMessage response;
            response.length = 5;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_P_VERSION;
            response.data[0] = protocol_version[1]; // Minor version
            response.data[1] = protocol_version[0]; // Major version
            sendMessage(response);
            break;
        }
        case MSG_SYS_GET_UNIQUE_ID: {
            BiDiBMessage response;
            response.length = 10;
            response.address[0] = 0;
            response.msg_num = msg.msg_num;
            response.msg_type = MSG_SYS_UNIQUE_ID;
            memcpy(response.data, unique_id, 7);
            sendMessage(response);
            break;
        }

        // --- Node and Logon Management ---
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
        case MSG_VENDOR_ACK: {
            if (_vendorAckCallback != nullptr) {
                _vendorAckCallback(msg.address[0], msg.data[0]);
            }
            break;
        }
        case MSG_VENDOR: {
            if (_vendorDataCallback != nullptr) {
                const char* data_str = (const char*)msg.data;
                const char* separator = strchr(data_str, '=');
                if (separator != nullptr) {
                    char name[32];
                    char value[32];
                    strncpy(name, data_str, separator - data_str);
                    name[separator - data_str] = '\0';
                    strcpy(value, separator + 1);
                    _vendorDataCallback(msg.address[0], name, value);
                }
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
                memcpy(response.data + 2, _node_table[requested_node_index].unique_id, 7);
                sendMessage(response);
            } else {
                // Node index is out of bounds
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
            if (findNode(msg.data) != -1) { break; } // Ignore if node is already logged on.
            if (_node_count >= BIDIB_MAX_NODES) { break; } // Ignore if the node table is full.

            // 1. Add the new node to the local table.
            BiDiBNode newNode;
            memcpy(newNode.unique_id, msg.data, 7);
            _node_table[_node_count] = newNode;

            // 2. Send LOGON_ACK back to the new node.
            BiDiBMessage ack;
            ack.length = 12;
            ack.address[0] = 0;
            ack.msg_num = msg.msg_num;
            ack.msg_type = MSG_LOGON_ACK;
            ack.data[0] = node_table_version;
            ack.data[1] = _node_count; // The new node's address
            memcpy(ack.data + 2, msg.data, 7);
            sendMessage(ack);

            // 3. Announce the new node to all other nodes (broadcast).
            BiDiBMessage nodeNew;
            nodeNew.length = 12;
            nodeNew.address[0] = 0; // Broadcast address
            nodeNew.msg_num = 0;    // System message
            nodeNew.msg_type = MSG_NODE_NEW;
            nodeNew.data[0] = node_table_version;
            nodeNew.data[1] = _node_count; // The new node's address
            memcpy(nodeNew.data + 2, msg.data, 7);
            sendMessage(nodeNew);

            _node_count++;
            break;
        }
        case MSG_LOGON_ACK: {
            _isLoggedIn = true;
            _node_count = 1; // Reset local node count, will be updated by NODETAB messages.
            break;
        }

        // --- Feature Handling ---
        case MSG_FEATURE_GETALL: {
            _next_feature_index = 0; // Reset index for subsequent GETNEXT messages.
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
                // End of feature list
                BiDiBMessage response;
                response.length = 4;
                response.address[0] = 0;
                response.msg_num = msg.msg_num;
                response.msg_type = MSG_FEATURE_NA;
                response.data[0] = 255; // Indicates end of list
                sendMessage(response);
                _next_feature_index = 0; // Reset for next time
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

            // Acknowledge by sending the new value back.
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

        // --- Command Station State ---
        case MSG_CS_STATE: {
            _track_state = msg.data[0];
            break;
        }
        case MSG_CS_DRIVE_ACK: {
            if (_driveAckCallback != nullptr) {
                uint16_t address = msg.data[0] | (msg.data[1] << 8);
                uint8_t status = msg.data[2];
                _driveAckCallback(address, status);
            }
            break;
        }
        case MSG_CS_ACCESSORY_ACK: {
            if (_accessoryAckCallback != nullptr) {
                uint16_t address = msg.data[0] | (msg.data[1] << 8);
                uint8_t status = msg.data[2];
                _accessoryAckCallback(address, status);
            }
            break;
        }
        case MSG_CS_POM_ACK: {
            if (_pomAckCallback != nullptr) {
                uint16_t address = msg.data[0] | (msg.data[1] << 8);
                uint8_t status = msg.data[5];
                _pomAckCallback(address, status);
            }
            break;
        }

        // --- Occupancy Reporting ---
        case MSG_BM_OCC: {
            if (_occupancyCallback != nullptr) {
                uint8_t detectorNum = msg.data[0];
                _occupancyCallback(detectorNum, true);
            }
            break;
        }
        case MSG_BM_FREE: {
            if (_occupancyCallback != nullptr) {
                uint8_t detectorNum = msg.data[0];
                _occupancyCallback(detectorNum, false);
            }
            break;
        }
        case MSG_BM_MULTIPLE: {
            if (_occupancyMultipleCallback != nullptr) {
                uint8_t baseNum = msg.data[0];
                uint8_t size = msg.data[1];
                _occupancyMultipleCallback(baseNum, size, &msg.data[2]);
            }
            break;
        }
        case MSG_BM_ADDRESS: {
            if (_addressCallback != nullptr) {
                uint8_t detectorNum = msg.data[0];
                uint16_t address = msg.data[2] | (msg.data[3] << 8);
                _addressCallback(detectorNum, address);
            }
            break;
        }
        case MSG_BM_SPEED: {
            if (_speedCallback != nullptr) {
                uint16_t address = msg.data[0] | (msg.data[1] << 8);
                uint16_t speed = msg.data[2] | (msg.data[3] << 8);
                _speedCallback(address, speed);
            }
            break;
        }
        case MSG_BM_CV: {
            if (_cvCallback != nullptr) {
                uint16_t address = msg.data[0] | (msg.data[1] << 8);
                uint16_t cv = msg.data[3] | (msg.data[4] << 8);
                uint8_t value = msg.data[5];
                _cvCallback(address, cv, value);
            }
            break;
        }

        // --- Accessory Control ---
        case MSG_ACCESSORY_STATE:
        case MSG_ACCESSORY_NOTIFY: {
            if (_accessoryStateCallback != nullptr) {
                uint8_t accessoryNum = msg.data[0];
                uint8_t aspect = msg.data[1];
                _accessoryStateCallback(accessoryNum, aspect);
            }
            break;
        }

        // --- Booster Status ---
        case MSG_BOOST_STAT: {
            if (_boosterStatusCallback != nullptr) {
                _boosterStatusCallback(msg.data[0]);
            }
            break;
        }
        case MSG_BOOST_DIAGNOSTIC: {
            if (_boosterDiagnosticCallback != nullptr) {
                int addr_len = 0;
                for (int i=0; i<4; ++i) { if (msg.address[i] == 0) { addr_len = i + 1; break; } }
                int data_len = msg.length - addr_len - 2;
                int data_index = 0;
                while (data_index < data_len) {
                    uint8_t type = msg.data[data_index];
                    uint16_t value = msg.data[data_index + 1] | (msg.data[data_index + 2] << 8);
                    _boosterDiagnosticCallback(type, value);
                    data_index += 3;
                }
            }
            break;
        }

        // --- Firmware Update ---
        case MSG_FW_UPDATE_STAT: {
            if (_firmwareUpdateStatusCallback != nullptr) {
                uint8_t addr_len = 0;
                for (int i=0; i<4; ++i) { if (msg.address[i] == 0) { addr_len = i + 1; break; } }

                uint8_t status = msg.data[0];
                uint8_t detail = (msg.length > (addr_len + 3)) ? msg.data[1] : 0;
                _firmwareUpdateStatusCallback(status, detail);
            }
            break;
        }

        // --- Secure-ACK Handling ---
        case MSG_BM_MIRROR_OCC:
        case MSG_BM_MIRROR_FREE: {
            uint8_t expected_type = (msg.msg_type == MSG_BM_MIRROR_OCC) ? MSG_BM_OCC : MSG_BM_FREE;
            for (int i = 0; i < MAX_PENDING_SECURE_ACKS; ++i) {
                if (_pendingSecureAcks[i].active &&
                    _pendingSecureAcks[i].message.msg_type == expected_type &&
                    _pendingSecureAcks[i].message.data[0] == msg.data[0]) {
                    _pendingSecureAcks[i].active = false; // ACK received
                    break;
                }
            }
            break;
        }
        case MSG_BM_MIRROR_MULTIPLE: {
            for (int i = 0; i < MAX_PENDING_SECURE_ACKS; ++i) {
                if (_pendingSecureAcks[i].active &&
                    _pendingSecureAcks[i].message.msg_type == MSG_BM_MULTIPLE &&
                    _pendingSecureAcks[i].message.data[0] == msg.data[0]) {
                    _pendingSecureAcks[i].active = false; // ACK received
                    break;
                }
            }
            break;
        }

        // --- Control / Switching Callbacks ---
        case MSG_LC_STAT: {
            if (_lcStatCallback != nullptr) {
                _lcStatCallback(msg.data[0], msg.data[1], msg.data[2]);
            }
            break;
        }
        case MSG_LC_WAIT: {
            if (_lcWaitCallback != nullptr) {
                _lcWaitCallback(msg.data[0], msg.data[1], msg.data[2]);
            }
            break;
        }
        case MSG_LC_NA: {
            if (_lcNaCallback != nullptr) {
                // Determine if error cause is present (msg len > addr + 3)
                // Len = addr_len + 1(msg_num) + 1(type) + 2(port) + 1(err)?
                // Minimal length for LC_NA with error is usually checked via total length
                // Addr len for us is 0 (uplink from node usually has 0 address byte len if from self?? No, uplink has address of sender)
                // Let's assume standard parsing logic where we look at payload data.
                // data[0]=portL, data[1]=portH. If available, data[2]=errCause.
                uint8_t errCause = 0;
                // Calculate payload length: msg.length - (addr_len + 2)
                int addr_len = 0;
                for(int i=0; i<4; ++i) { if(msg.address[i]==0) {addr_len=i+1; break;} }
                int payload_len = msg.length - addr_len - 2;
                if (payload_len > 2) {
                    errCause = msg.data[2];
                }
                _lcNaCallback(msg.data[0], msg.data[1], errCause);
            }
            break;
        }
        case MSG_LC_CONFIGX: {
            if (_lcConfigXCallback != nullptr) {
                // data[0]=portL, data[1]=portH, data[2..]=params
                int addr_len = 0;
                for(int i=0; i<4; ++i) { if(msg.address[i]==0) {addr_len=i+1; break;} }
                int payload_len = msg.length - addr_len - 2;
                if (payload_len > 2) {
                    _lcConfigXCallback(msg.data[0], msg.data[1], payload_len - 2, &msg.data[2]);
                } else {
                     _lcConfigXCallback(msg.data[0], msg.data[1], 0, nullptr);
                }
            }
            break;
        }
        case MSG_LC_MACRO_STATE: {
            if (_lcMacroStateCallback != nullptr) {
                _lcMacroStateCallback(msg.data[0], msg.data[1]);
            }
            break;
        }
        case MSG_LC_MACRO: {
            if (_lcMacroCallback != nullptr) {
                _lcMacroCallback(msg.data[0], msg.data[1], msg.data[2], msg.data[3], msg.data[4], msg.data[5]);
            }
            break;
        }
        case MSG_LC_MACRO_PARA: {
            if (_lcMacroParaCallback != nullptr) {
                uint32_t val = (uint32_t)msg.data[2] | ((uint32_t)msg.data[3] << 8) | ((uint32_t)msg.data[4] << 16) | ((uint32_t)msg.data[5] << 24);
                _lcMacroParaCallback(msg.data[0], msg.data[1], val);
            }
            break;
        }
    }
}

// =============================================================================
// Internal Helper Functions
// =============================================================================

void BiDiB::addPendingSecureAck(const BiDiBMessage &msg) {
    for (int i = 0; i < MAX_PENDING_SECURE_ACKS; ++i) {
        if (!_pendingSecureAcks[i].active) {
            _pendingSecureAcks[i].active = true;
            _pendingSecureAcks[i].message = msg;
            _pendingSecureAcks[i].timestamp = millis();
            _pendingSecureAcks[i].retries = 0;
            sendMessage(msg);
            return; // Found a slot and sent the message
        }
    }
    // If no slot is found, the message is dropped.
    // An alternative could be to log an error.
}

int BiDiB::findNode(const uint8_t* unique_id) {
    for (int i = 0; i < _node_count; ++i) {
        if (memcmp(_node_table[i].unique_id, unique_id, 7) == 0) {
            return i; // Node found at index i
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

    // Send length, address, message number, and type
    sendByte(msg.length, crc);
    uint8_t addr_len = 0;
    for (int i = 0; i < 4; i++) {
        addr_len++;
        if (msg.address[i] == 0) break;
    }
    for (int i = 0; i < addr_len; ++i) { sendByte(msg.address[i], crc); }
    sendByte(msg.msg_num, crc);
    sendByte(msg.msg_type, crc);

    // Send data payload
    uint8_t data_len = msg.length - addr_len - 2;
    for (int i = 0; i < data_len; ++i) { sendByte(msg.data[i], crc); }

    // Send the calculated CRC
    if (crc == BIDIB_MAGIC || crc == BIDIB_ESCAPE) {
        bidib_serial->write(BIDIB_ESCAPE);
        bidib_serial->write(crc ^ 0x20);
    } else {
        bidib_serial->write(crc);
    }

    bidib_serial->write(BIDIB_MAGIC);
}

bool BiDiB::processByte(uint8_t b) {
    switch (_rx_state)
    {
    case FSM_IDLE:
        if (b == BIDIB_MAGIC)
        {
            _rx_ptr = 0;
            _rx_state = FSM_IN_MSG;
        }
        break;

    case FSM_IN_MSG:
        if (b == BIDIB_ESCAPE)
        {
            _rx_state = FSM_IN_MSG_ESCAPED;
        }
        else if (b == BIDIB_MAGIC)
        {
            // End of message
            if (_rx_ptr > 0)
            {
                uint8_t final_crc = 0;
                for (int i = 0; i < _rx_ptr; ++i) {
                    updateCrc(_rx_buffer[i], final_crc);
                }

                if (final_crc == 0)
                {
                    // CRC is valid, message is complete
                    _rx_state = FSM_IDLE;
                    return true;
                }
            }
            // Invalid message or framing error, reset and start looking for next MAGIC
            _rx_ptr = 0;
            _rx_state = FSM_IDLE;
        }
        else
        {
            _rx_buffer[_rx_ptr++] = b;
        }
        break;

    case FSM_IN_MSG_ESCAPED:
        _rx_buffer[_rx_ptr++] = b ^ 0x20;
        _rx_state = FSM_IN_MSG;
        break;
    }
    return false;
}

bool BiDiB::receiveMessage(BiDiBMessage& msg) {
    if (!bidib_serial) { return false; }

    while (bidib_serial->available() > 0) {
        if (processByte(bidib_serial->read())) {
            // A full message has been received in _rx_buffer. Now parse it.
            msg.length = _rx_buffer[0];

            // The total content between MAGICs is in _rx_buffer, which includes:
            // [Length] [Address...] [MsgNum] [MsgType] [Data...] [CRC]
            // The length byte itself tells us the length of the part after it, up to (but not including) the CRC.

            int addr_len = 0;
            for (int i = 0; i < 4; ++i) {
                addr_len++;
                if (_rx_buffer[1 + i] == 0) break;
            }
            memcpy(msg.address, &_rx_buffer[1], addr_len);

            int current_pos = 1 + addr_len;
            msg.msg_num = _rx_buffer[current_pos++];
            msg.msg_type = _rx_buffer[current_pos++];

            // Data length is the total payload length (from length byte) minus what we've already parsed.
            int data_len = msg.length - (addr_len + 1 + 1);
            if (data_len > 0) {
                memcpy(msg.data, &_rx_buffer[current_pos], data_len);
            }

            return true;
        }
    }
    return false;
}

// =============================================================================
// Core Arduino Sketch Functions
// =============================================================================

void BiDiB::begin(Stream &serial) {
    bidib_serial = &serial;
}

void BiDiB::update() {
    // 1. Process incoming serial data
    if (bidib_serial->available() > 0) {
        if (receiveMessage(_lastMessage)) {
            _messageAvailable = true;
        }
    }

    // 2. Handle timeouts for Secure-ACKs
    if (getFeature(FEATURE_BM_SECACK_ON)) {
        unsigned long now = millis();
        for (int i = 0; i < MAX_PENDING_SECURE_ACKS; ++i) {
            if (_pendingSecureAcks[i].active) {
                if (now - _pendingSecureAcks[i].timestamp > SECURE_ACK_TIMEOUT) {
                    if (_pendingSecureAcks[i].retries < SECURE_ACK_RETRIES) {
                        // Resend the message
                        _pendingSecureAcks[i].retries++;
                        _pendingSecureAcks[i].timestamp = now;
                        sendMessage(_pendingSecureAcks[i].message);
                    } else {
                        // Max retries reached, give up
                        _pendingSecureAcks[i].active = false;
                    }
                }
            }
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
