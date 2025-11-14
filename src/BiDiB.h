#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

//================================================================================
// BiDiB Protocol Constants
//================================================================================

const uint8_t BIDIB_MAGIC = 0xFE;  ///< Start and end of a BiDiB message
const uint8_t BIDIB_ESCAPE = 0xFD; ///< Escape character for MAGIC byte

//================================================================================
// BiDiB Message Types
//================================================================================

// --- System Messages ---
const uint8_t BIDIB_MAX_NODES = 32;
const uint8_t MSG_SYS_GET_MAGIC = 1;
const uint8_t MSG_SYS_GET_P_VERSION = 2;
const uint8_t MSG_SYS_GET_UNIQUE_ID = 3;
const uint8_t MSG_SYS_ENABLE = 4;
const uint8_t MSG_SYS_DISABLE = 5;
const uint8_t MSG_NODETAB_GETALL = 6;
const uint8_t MSG_NODETAB_GETNEXT = 7;
const uint8_t MSG_LOGON = 10;
const uint8_t MSG_SYS_MAGIC = 0x81;
const uint8_t MSG_SYS_P_VERSION = 0x82;
const uint8_t MSG_SYS_UNIQUE_ID = 0x83;
const uint8_t MSG_NODETAB_COUNT = 0x86;
const uint8_t MSG_NODETAB = 0x87;
const uint8_t MSG_NODE_NA = 0x88;
const uint8_t MSG_NODE_NEW = 0x89;
const uint8_t MSG_NODE_LOST = 0x8A;
const uint8_t MSG_LOGON_ACK = 0x8B;

// --- Feature Messages ---
const uint8_t MSG_FEATURE_GETALL = 11;
const uint8_t MSG_FEATURE_GETNEXT = 12;
const uint8_t MSG_FEATURE_GET = 13;
const uint8_t MSG_FEATURE_SET = 14;
const uint8_t MSG_FEATURE_COUNT = 0x8C;
const uint8_t MSG_FEATURE = 0x8D;
const uint8_t MSG_FEATURE_NA = 0x8E;

// --- Command Station Messages ---
const uint8_t MSG_CS_SET_STATE = 0x48;
const uint8_t MSG_CS_DRIVE = 0x40;
const uint8_t MSG_CS_ACCESSORY = 0x42;
const uint8_t MSG_CS_POM = 0x44;
const uint8_t MSG_CS_DRIVE_ACK = 0xE0;
const uint8_t MSG_CS_ACCESSORY_ACK = 0xE2;
const uint8_t MSG_CS_POM_ACK = 0xE4;
const uint8_t MSG_CS_STATE = 0xE9;

// --- Occupancy Messages ---
const uint8_t MSG_BM_GET_RANGE = 0xA0;
const uint8_t MSG_BM_MULTIPLE = 0xA1;
const uint8_t MSG_BM_OCC = 0xA2;
const uint8_t MSG_BM_FREE = 0xA3;
const uint8_t MSG_BM_MIRROR_OCC = 0xA8;
const uint8_t MSG_BM_MIRROR_FREE = 0xA9;
const uint8_t MSG_BM_MIRROR_MULTIPLE = 0xAA;
const uint8_t MSG_BM_ADDRESS = 0xA4;
const uint8_t MSG_BM_SPEED = 0xA5;
const uint8_t MSG_BM_CV = 0xA6;

// --- Accessory Control Messages ---
const uint8_t MSG_ACCESSORY_SET = 0x38;
const uint8_t MSG_ACCESSORY_GET = 0x39;
const uint8_t MSG_ACCESSORY_STATE = 0xB8;
const uint8_t MSG_ACCESSORY_NOTIFY = 0xB9;

// --- Booster Messages ---
const uint8_t MSG_BOOST_ON = 0x50;
const uint8_t MSG_BOOST_OFF = 0x51;
const uint8_t MSG_BOOST_QUERY = 0x52;
const uint8_t MSG_BOOST_STAT = 0xD0;
const uint8_t MSG_BOOST_DIAGNOSTIC = 0xD1;


// --- Command Station Constants ---
const uint8_t BIDIB_CS_STATE_OFF = 0;  ///< Track voltage is off
const uint8_t BIDIB_CS_STATE_STOP = 1; ///< Track voltage is on, but zero speed commands are sent
const uint8_t BIDIB_CS_STATE_GO = 2;   ///< Track voltage is on, normal operation

// --- POM Opcodes ---
const uint8_t BIDIB_CS_POM_RD_BLOCK = 0x00;
const uint8_t BIDIB_CS_POM_RD_BYTE = 0x01;
const uint8_t BIDIB_CS_POM_WR_BIT = 0x02;
const uint8_t BIDIB_CS_POM_WR_BYTE = 0x03;

// --- Booster Constants ---
const uint8_t BIDIB_BST_STATE_OFF = 0x00;         ///< Booster is off (general)
const uint8_t BIDIB_BST_STATE_OFF_SHORT = 0x01;  ///< Booster is off due to short circuit
const uint8_t BIDIB_BST_STATE_OFF_OVERHEAT = 0x02;///< Booster is off due to overheating
const uint8_t BIDIB_BST_STATE_ON = 0x80;         ///< Booster is on
const uint8_t BIDIB_BST_STATE_ON_LIMIT = 0x81;   ///< Booster is on and in current-limiting mode

const uint8_t BIDIB_BST_DIAG_CURRENT = 0x00;     ///< Diagnostic key for current
const uint8_t BIDIB_BST_DIAG_VOLTAGE = 0x01;     ///< Diagnostic key for voltage
const uint8_t BIDIB_BST_DIAG_TEMP = 0x02;        ///< Diagnostic key for temperature

//================================================================================
// BiDiB Data Structures
//================================================================================

/// @brief Structure representing a BiDiB message.
struct BiDiBMessage
{
    uint8_t length;
    uint8_t address[4];
    uint8_t msg_num;
    uint8_t msg_type;
    uint8_t data[64]; // Assuming a maximum data length
};

const uint8_t BIDIB_MAX_FEATURES = 16;

// --- Feature Constants ---
const uint8_t BIDIB_FEATURE_FW_UPDATE_SUPPORT = 0;     ///< 1 if firmware update is supported
const uint8_t BIDIB_FEATURE_STRING_SIZE = 1;           ///< Maximum size of strings
const uint8_t BIDIB_FEATURE_MSG_RECEIVE_COUNT = 2;     ///< How many messages can be received at once
const uint8_t FEATURE_BM_SECACK_AVAILABLE = 2;         ///< Indicates if Secure-ACK is supported
const uint8_t FEATURE_BM_SECACK_ON = 3;                ///< Enables the Secure-ACK mechanism

/// @brief Structure representing a node on the BiDiB bus.
struct BiDiBNode
{
    uint8_t unique_id[7];
};

/// @brief Structure representing a feature of a BiDiB node.
struct BiDiBFeature
{
    uint8_t feature_num;
    uint8_t value;
};

/// @brief Callback function type for drive acknowledgements.
/// @param address The DCC address of the locomotive.
/// @param status The acknowledgement status.
typedef void (*DriveAckCallback)(uint16_t address, uint8_t status);

/// @brief Callback function type for accessory acknowledgements.
/// @param address The DCC address of the accessory.
/// @param status The acknowledgement status.
typedef void (*AccessoryAckCallback)(uint16_t address, uint8_t status);

/// @brief Callback function type for PoM acknowledgements.
/// @param address The DCC address of the decoder.
/// @param status The acknowledgement status.
typedef void (*PomAckCallback)(uint16_t address, uint8_t status);

/// @brief Callback function type for single occupancy detector events.
/// @param detectorNum The number of the detector.
/// @param occupied True if the detector is occupied (MSG_BM_OCC), false if it is free (MSG_BM_FREE).
typedef void (*OccupancyCallback)(uint8_t detectorNum, bool occupied);

/// @brief Callback function type for a range of occupancy detectors.
/// @param baseNum The base number of the first detector.
/// @param size The number of detectors reported.
/// @param data Pointer to the bitmap data representing the states.
typedef void (*OccupancyMultipleCallback)(uint8_t baseNum, uint8_t size, const uint8_t* data);

/// @brief Callback function type for address reporting events.
/// @param detectorNum The number of the detector.
/// @param address The DCC address of the decoder.
typedef void (*AddressCallback)(uint8_t detectorNum, uint16_t address);

/// @brief Callback function type for speed reporting events.
/// @param address The DCC address of the locomotive.
/// @param speed The speed of the locomotive.
typedef void (*SpeedCallback)(uint16_t address, uint16_t speed);

/// @brief Callback function type for CV reporting events.
/// @param address The DCC address of the decoder.
/// @param cv The CV number.
/// @param value The value of the CV.
typedef void (*CvCallback)(uint16_t address, uint16_t cv, uint8_t value);

/// @brief Callback function type for native accessory state reports.
/// @param accessoryNum The number of the accessory.
/// @param aspect The current aspect (state) of the accessory.
typedef void (*AccessoryStateCallback)(uint8_t accessoryNum, uint8_t aspect);

/// @brief Callback function type for booster status reports.
/// @param status The current status of the booster (see BIDIB_BST_STATE_* constants).
typedef void (*BoosterStatusCallback)(uint8_t status);

/// @brief Callback function type for booster diagnostic reports.
/// @param type The type of diagnostic value (see BIDIB_BST_DIAG_* constants).
/// @param value The diagnostic value.
typedef void (*BoosterDiagnosticCallback)(uint8_t type, uint16_t value);


//================================================================================
// Secure ACK Configuration
//================================================================================

const unsigned long SECURE_ACK_TIMEOUT = 1000; ///< Timeout in milliseconds for Secure-ACK
const uint8_t SECURE_ACK_RETRIES = 3;          ///< Number of retries for a Secure-ACK message
const uint8_t MAX_PENDING_SECURE_ACKS = 8;     ///< Maximum number of parallel Secure-ACKs

/// @brief Structure to hold information about a pending Secure-ACK message.
struct PendingSecureAck
{
    bool active;
    BiDiBMessage message;
    unsigned long timestamp;
    uint8_t retries;
};


//================================================================================
// BiDiB Class Definition
//================================================================================

class BiDiB
{
public:
    BiDiB();

    // --- Core Functions ---

    /// @brief Initializes the BiDiB instance with a serial port.
    /// @param serial The Arduino Stream object to use for communication (e.g., Serial, Serial1).
    void begin(Stream &serial);

    /// @brief Processes incoming data from the serial port. This must be called regularly in the main loop.
    void update();

    /// @brief Handles the last fully received message.
    void handleMessages();

    /// @brief Sends a complete, formatted BiDiB message.
    /// @param msg The BiDiBMessage object to send.
    virtual void sendMessage(const BiDiBMessage &msg);

    /// @brief Checks if a message has been received and is waiting to be processed.
    /// @return True if a message is available, false otherwise.
    bool messageAvailable();

    /// @brief Gets the last received message.
    /// @return The last BiDiBMessage object received.
    BiDiBMessage getLastMessage();

    /// @brief Helper function to calculate the CRC8 checksum for a data block.
    /// @param data Pointer to the data array.
    /// @param size The size of the data array.
    /// @return The calculated CRC8 checksum.
    uint8_t calculateCrc(const uint8_t *data, size_t size);

    // --- System-Level Functions ---

    /// @brief Initiates the logon sequence to connect to the BiDiB master.
    void logon();

    /// @brief Enables the BiDiB node, allowing it to send and receive messages.
    void enable();

    /// @brief Disables the BiDiB node.
    void disable();

    /// @brief Checks if the node is currently logged in and enabled.
    /// @return True if the node is logged in, false otherwise.
    bool isLoggedIn();

    // --- Feature Management ---

    /// @brief Sets the value of a feature for this node.
    /// @param feature_num The feature number to set.
    /// @param value The value to set for the feature.
    void setFeature(uint8_t feature_num, uint8_t value);

    /// @brief Gets the current value of a feature for this node.
    /// @param feature_num The feature number to get.
    /// @return The value of the feature.
    uint8_t getFeature(uint8_t feature_num);

    // --- Command Station Functions ---

    /// @brief Sets the state of the DCC track power.
    /// @param state The desired track state (BIDIB_CS_STATE_OFF, BIDIB_CS_STATE_STOP, BIDIB_CS_STATE_GO).
    void setTrackState(uint8_t state);

    /// @brief Sends a drive command to a locomotive.
    /// @param address The DCC address of the locomotive.
    /// @param speed The speed of the locomotive (-127 to 127).
    /// @param functions A bitmask representing the active functions (F0-F7).
    void drive(uint16_t address, int8_t speed, uint8_t functions);

    /// @brief Registers a callback function to be called when a drive acknowledgement is received.
    /// @param callback The function to be called.
    void onDriveAck(DriveAckCallback callback);

    /// @brief Sends a command to a DCC accessory.
    /// @param address The DCC address of the accessory.
    /// @param output The output to control (0-3).
    /// @param state The desired state of the output (0 = off, 1 = on).
    void accessory(uint16_t address, uint8_t output, uint8_t state);

    /// @brief Registers a callback function to be called when an accessory acknowledgement is received.
    /// @param callback The function to be called.
    void onAccessoryAck(AccessoryAckCallback callback);

    /// @brief Writes a single byte to a CV on the main track (PoM).
    /// @param address The DCC address of the decoder.
    /// @param cv The CV number to write to (1-1024).
    /// @param value The byte value to write.
    void pomWriteByte(uint16_t address, uint16_t cv, uint8_t value);

    /// @brief Registers a callback function to be called when a PoM acknowledgement is received.
    /// @param callback The function to be called.
    void onPomAck(PomAckCallback callback);

    // --- Booster Functions ---

    /// @brief Sets the state of a booster (on or off).
    /// @param on True to turn the booster on, false to turn it off.
    /// @param node_addr The address of the booster to control. Use 0 for a broadcast to all boosters.
    void setBoosterState(bool on, uint8_t node_addr = 0);

    /// @brief Queries the status and diagnostic values of a booster.
    /// @param node_addr The address of the booster to query. Use 0 for a broadcast to all boosters.
    void queryBooster(uint8_t node_addr = 0);

    /// @brief Registers a callback function to be called when a booster status report is received.
    /// @param callback The function to be called.
    void onBoosterStatus(BoosterStatusCallback callback);

    /// @brief Registers a callback function to be called when a booster diagnostic report is received.
    /// @param callback The function to be called.
    void onBoosterDiagnostic(BoosterDiagnosticCallback callback);

    // --- Accessory Control Functions ---

    /// @brief Sets the state (aspect) of a native BiDiB accessory.
    /// @param accessoryNum The number of the accessory.
    /// @param aspect The desired aspect (state) to set.
    void setAccessory(uint8_t accessoryNum, uint8_t aspect);

    /// @brief Requests the current state of a native BiDiB accessory.
    /// @param accessoryNum The number of the accessory to query.
    void getAccessory(uint8_t accessoryNum);

    /// @brief Registers a callback function to be called when a native accessory state report is received.
    /// @param callback The function to be called.
    void onAccessoryState(AccessoryStateCallback callback);

    // --- Occupancy Reporting ---

    /// @brief Registers a callback function to be called for single occupancy detector events (occupied/free).
    /// @param callback The function to be called.
    void onOccupancy(OccupancyCallback callback);

    /// @brief Registers a callback function to be called for multiple occupancy detector reports.
    /// @param callback The function to be called.
    void onOccupancyMultiple(OccupancyMultipleCallback callback);

    /// @brief Registers a callback function to be called for address reporting events (e.g., from Railcom detectors).
    /// @param callback The function to be called.
    void onAddress(AddressCallback callback);

    /// @brief Registers a callback function to be called for speed reporting events.
    /// @param callback The function to be called.
    void onSpeedUpdate(SpeedCallback callback);

    /// @brief Registers a callback function to be called for CV reporting events.
    /// @param callback The function to be called.
    void onCvUpdate(CvCallback callback);

    /// @brief Sends an occupancy report for a single detector. If Secure-ACK is enabled, this will be handled automatically.
    /// @param detectorNum The number of the detector (0-255).
    /// @param occupied True if the detector is occupied, false if it is free.
    void sendOccupancySingle(uint8_t detectorNum, bool occupied);

    /// @brief Sends an occupancy report for a range of detectors. If Secure-ACK is enabled, this will be handled automatically.
    /// @param baseNum The base number of the first detector.
    /// @param size The number of detectors to report.
    /// @param data A pointer to the bitmap data representing the detector states.
    void sendOccupancyMultiple(uint8_t baseNum, uint8_t size, const uint8_t* data);

    // --- Node Properties ---
    uint8_t unique_id[7];       ///< The unique ID of this node.
    uint8_t node_table_version; ///< The version of the node table.

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
    uint8_t _track_state;
    DriveAckCallback _driveAckCallback;
    AccessoryAckCallback _accessoryAckCallback;
    PomAckCallback _pomAckCallback;
    BoosterStatusCallback _boosterStatusCallback;
    BoosterDiagnosticCallback _boosterDiagnosticCallback;
    OccupancyCallback _occupancyCallback;
    OccupancyMultipleCallback _occupancyMultipleCallback;
    AddressCallback _addressCallback;
    SpeedCallback _speedCallback;
    CvCallback _cvCallback;
    AccessoryStateCallback _accessoryStateCallback;

private:
    /// @brief Receives and validates an incoming BiDiB message from the serial stream.
    /// @param msg A reference to a BiDiBMessage object to store the received message.
    /// @return True if a complete and valid message was received, false otherwise.
    bool receiveMessage(BiDiBMessage &msg);

    /// @brief Finds a node in the internal node table by its unique ID.
    /// @param unique_id A pointer to the 7-byte unique ID of the node to find.
    /// @return The index of the node in the table, or -1 if not found.
    int findNode(const uint8_t *unique_id);

    /// @brief Sends a single byte and applies escaping if necessary.
    /// @param byte The byte to send.
    /// @param crc A reference to the running CRC checksum, which will be updated.
    void sendByte(uint8_t byte, uint8_t &crc);

    /// @brief Updates the CRC checksum with a new byte.
    /// @param byte The byte to add to the CRC calculation.
    /// @param crc A reference to the CRC checksum to update.
    void updateCrc(uint8_t byte, uint8_t &crc);

    /// @brief Adds a message to the pending Secure-ACK list.
    /// @param msg The message to add.
    void addPendingSecureAck(const BiDiBMessage &msg);

    Stream *bidib_serial;
    uint8_t protocol_version[2] = {0, 1}; // V 0.1

    PendingSecureAck _pendingSecureAcks[MAX_PENDING_SECURE_ACKS];
};

#endif
