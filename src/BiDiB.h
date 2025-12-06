#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

//================================================================================
// BiDiB Protocol Constants
//================================================================================

const uint8_t BIDIB_MAGIC           = 0xFE;  ///< Start and end of a BiDiB message
const uint8_t BIDIB_ESCAPE          = 0xFD;  ///< Escape character for MAGIC byte
const uint8_t BIDIB_VERSION         = 7;     ///< BiDiB Protocol Version 0.7

//================================================================================
// BiDiB Message Types
//================================================================================

// --- System Messages (Downstream 0x00 - 0x0F) ---
const uint8_t MSG_SYS_GET_MAGIC       = 0x01;
const uint8_t MSG_SYS_GET_P_VERSION   = 0x02;
const uint8_t MSG_SYS_ENABLE          = 0x03;
const uint8_t MSG_SYS_DISABLE         = 0x04;
const uint8_t MSG_SYS_GET_UNIQUE_ID   = 0x05;
const uint8_t MSG_SYS_GET_SW_VERSION  = 0x06;
const uint8_t MSG_SYS_PING            = 0x07;
const uint8_t MSG_SYS_IDENTIFY        = 0x08;
const uint8_t MSG_SYS_RESET           = 0x09;
const uint8_t MSG_NODETAB_GETALL      = 0x0B;
const uint8_t MSG_NODETAB_GETNEXT     = 0x0C;
const uint8_t MSG_NODE_CHANGED_ACK    = 0x0D;
const uint8_t MSG_SYS_GET_ERROR       = 0x0E;
const uint8_t MSG_FW_UPDATE_OP        = 0x0F;

// --- Feature Messages (Downstream 0x10 - 0x1F) ---
const uint8_t MSG_FEATURE_GETALL      = 0x10;
const uint8_t MSG_FEATURE_GETNEXT     = 0x11;
const uint8_t MSG_FEATURE_GET         = 0x12;
const uint8_t MSG_FEATURE_SET         = 0x13;
const uint8_t MSG_VENDOR_ENABLE       = 0x14;
const uint8_t MSG_VENDOR_DISABLE      = 0x15;
const uint8_t MSG_VENDOR_SET          = 0x16;
const uint8_t MSG_VENDOR_GET          = 0x17;
const uint8_t MSG_SYS_CLOCK           = 0x18;
const uint8_t MSG_STRING_GET          = 0x19;
const uint8_t MSG_STRING_SET          = 0x1A;

// --- Occupancy Messages (Downstream 0x20 - 0x2F) ---
const uint8_t MSG_BM_GET_RANGE        = 0x20;
const uint8_t MSG_BM_MIRROR_MULTIPLE  = 0x21;
const uint8_t MSG_BM_MIRROR_OCC       = 0x22;
const uint8_t MSG_BM_MIRROR_FREE      = 0x23;
const uint8_t MSG_BM_ADDR_GET_RANGE   = 0x24;
const uint8_t MSG_BM_GET_CONFIDENCE   = 0x25;
const uint8_t MSG_BM_MIRROR_POSITION  = 0x26;

// --- Booster Messages (Downstream 0x30 - 0x37) ---
const uint8_t MSG_BOOST_OFF           = 0x30;
const uint8_t MSG_BOOST_ON            = 0x31;
const uint8_t MSG_BOOST_QUERY         = 0x32;

// --- Accessory Control Messages (Downstream 0x38 - 0x3E) ---
const uint8_t MSG_ACCESSORY_SET       = 0x38;
const uint8_t MSG_ACCESSORY_GET       = 0x39;
const uint8_t MSG_ACCESSORY_PARA_SET  = 0x3A;
const uint8_t MSG_ACCESSORY_PARA_GET  = 0x3B;

// --- Light Control & Port Config (Downstream 0x3F - 0x47) ---
const uint8_t MSG_LC_PORT_QUERY_ALL   = 0x3F;
const uint8_t MSG_LC_OUTPUT           = 0x40;
const uint8_t MSG_LC_CONFIG_SET       = 0x41; // Deprecated
const uint8_t MSG_LC_CONFIG_GET       = 0x42; // Deprecated
const uint8_t MSG_LC_KEY_QUERY        = 0x43; // Deprecated
const uint8_t MSG_LC_PORT_QUERY       = 0x44;
const uint8_t MSG_LC_CONFIGX_GET_ALL  = 0x45;
const uint8_t MSG_LC_CONFIGX_SET      = 0x46;
const uint8_t MSG_LC_CONFIGX_GET      = 0x47;

// --- Macro Messages (Downstream 0x48 - 0x4F) ---
const uint8_t MSG_LC_MACRO_HANDLE     = 0x48;
const uint8_t MSG_LC_MACRO_SET        = 0x49;
const uint8_t MSG_LC_MACRO_GET        = 0x4A;
const uint8_t MSG_LC_MACRO_PARA_SET   = 0x4B;
const uint8_t MSG_LC_MACRO_PARA_GET   = 0x4C;

// --- DCC Gen Messages (Downstream 0x60 - 0x6F) ---
const uint8_t MSG_CS_ALLOCATE         = 0x60;
const uint8_t MSG_CS_SET_STATE        = 0x62;
const uint8_t MSG_CS_DRIVE            = 0x64;
const uint8_t MSG_CS_ACCESSORY        = 0x65;
const uint8_t MSG_CS_BIN_STATE        = 0x66;
const uint8_t MSG_CS_POM              = 0x67;
const uint8_t MSG_CS_RCPLUS           = 0x68;
const uint8_t MSG_CS_PROG             = 0x6F;

// --- Local Messages (Downstream 0x70 - 0x7F) ---
const uint8_t MSG_LOGON_ACK           = 0x70;
const uint8_t MSG_LOCAL_PING          = 0x71;
const uint8_t MSG_LOGON_REJECTED      = 0x72;
const uint8_t MSG_LOCAL_ACCESSORY     = 0x73;
const uint8_t MSG_LOCAL_SYNC          = 0x74;


// --- System Messages (Upstream 0x80 - 0x8F) ---
const uint8_t MSG_SYS_MAGIC           = 0x81;
const uint8_t MSG_SYS_PONG            = 0x82;
const uint8_t MSG_SYS_P_VERSION       = 0x83;
const uint8_t MSG_SYS_UNIQUE_ID       = 0x84;
const uint8_t MSG_SYS_SW_VERSION      = 0x85;
const uint8_t MSG_SYS_ERROR           = 0x86;
const uint8_t MSG_SYS_IDENTIFY_STATE  = 0x87;
const uint8_t MSG_NODETAB_COUNT       = 0x88;
const uint8_t MSG_NODETAB             = 0x89;
const uint8_t MSG_PKT_CAPACITY        = 0x8A;
const uint8_t MSG_NODE_NA             = 0x8B;
const uint8_t MSG_NODE_LOST           = 0x8C;
const uint8_t MSG_NODE_NEW            = 0x8D;
const uint8_t MSG_STALL               = 0x8E;
const uint8_t MSG_FW_UPDATE_STAT      = 0x8F;

// --- Feature Messages (Upstream 0x90 - 0x9F) ---
const uint8_t MSG_FEATURE             = 0x90;
const uint8_t MSG_FEATURE_NA          = 0x91;
const uint8_t MSG_FEATURE_COUNT       = 0x92;
const uint8_t MSG_VENDOR              = 0x93;
const uint8_t MSG_VENDOR_ACK          = 0x94;
const uint8_t MSG_STRING              = 0x95;

// --- Occupancy Messages (Upstream 0xA0 - 0xAF) ---
const uint8_t MSG_BM_OCC              = 0xA0;
const uint8_t MSG_BM_FREE             = 0xA1;
const uint8_t MSG_BM_MULTIPLE         = 0xA2;
const uint8_t MSG_BM_ADDRESS          = 0xA3;
const uint8_t MSG_BM_ACCESSORY        = 0xA4;
const uint8_t MSG_BM_CV               = 0xA5;
const uint8_t MSG_BM_SPEED            = 0xA6;
const uint8_t MSG_BM_CURRENT          = 0xA7;
const uint8_t MSG_BM_XPOM             = 0xA8;
const uint8_t MSG_BM_CONFIDENCE       = 0xA9;
const uint8_t MSG_BM_DYN_STATE        = 0xAA;
const uint8_t MSG_BM_RCPLUS           = 0xAB;
const uint8_t MSG_BM_POSITION         = 0xAC;

// --- Booster Messages (Upstream 0xB0 - 0xB7) ---
const uint8_t MSG_BOOST_STAT          = 0xB0;
const uint8_t MSG_BOOST_CURRENT       = 0xB1; // Deprecated
const uint8_t MSG_BOOST_DIAGNOSTIC    = 0xB2;

// --- Accessory Control Messages (Upstream 0xB8 - 0xBF) ---
const uint8_t MSG_ACCESSORY_STATE     = 0xB8;
const uint8_t MSG_ACCESSORY_PARA      = 0xB9;
const uint8_t MSG_ACCESSORY_NOTIFY    = 0xBA;

// --- Light Control (Upstream 0xC0 - 0xC7) ---
const uint8_t MSG_LC_STAT             = 0xC0;
const uint8_t MSG_LC_NA               = 0xC1;
const uint8_t MSG_LC_CONFIG           = 0xC2; // Deprecated
const uint8_t MSG_LC_KEY              = 0xC3; // Deprecated
const uint8_t MSG_LC_WAIT             = 0xC4;
const uint8_t MSG_LC_CONFIGX          = 0xC6;

// --- Macro Messages (Upstream 0xC8 - 0xCF) ---
const uint8_t MSG_LC_MACRO_STATE      = 0xC8;
const uint8_t MSG_LC_MACRO            = 0xC9;
const uint8_t MSG_LC_MACRO_PARA       = 0xCA;

// --- DCC Gen Messages (Upstream 0xE0 - 0xEF) ---
const uint8_t MSG_CS_ALLOC_ACK        = 0xE0;
const uint8_t MSG_CS_STATE            = 0xE1;
const uint8_t MSG_CS_DRIVE_ACK        = 0xE2;
const uint8_t MSG_CS_ACCESSORY_ACK    = 0xE3;
const uint8_t MSG_CS_POM_ACK          = 0xE4;
const uint8_t MSG_CS_DRIVE_MANUAL     = 0xE5;
const uint8_t MSG_CS_DRIVE_EVENT      = 0xE6;
const uint8_t MSG_CS_ACCESSORY_MANUAL = 0xE7;
const uint8_t MSG_CS_RCPLUS_ACK       = 0xE8;
const uint8_t MSG_CS_PROG_STATE       = 0xEF;

// --- Local Messages (Upstream 0xF0 - 0xFF) ---
const uint8_t MSG_LOGON               = 0xF0;
const uint8_t MSG_LOCAL_PONG          = 0xF1;


// --- Command Station Constants ---
const uint8_t BIDIB_CS_STATE_OFF           = 0;   ///< Track voltage is off
const uint8_t BIDIB_CS_STATE_STOP          = 1;   ///< Track voltage is on, but zero speed commands are sent
const uint8_t BIDIB_CS_STATE_SOFTSTOP      = 2;   ///< Soft stop is progress
const uint8_t BIDIB_CS_STATE_GO            = 3;   ///< Track voltage is on, normal operation
const uint8_t BIDIB_CS_STATE_GO_IGN_WD     = 4;   ///< Track voltage is on, watchdog ignored
const uint8_t BIDIB_CS_STATE_PROG          = 8;   ///< In Programming Mode (ready for commands)
const uint8_t BIDIB_CS_STATE_PROGBUSY      = 9;   ///< In Programming Mode (busy)
const uint8_t BIDIB_CS_STATE_BUSY          = 13;  ///< Busy

// --- POM Opcodes ---
const uint8_t BIDIB_CS_POM_RD_BLOCK        = 0x00;
const uint8_t BIDIB_CS_POM_RD_BYTE         = 0x01;
const uint8_t BIDIB_CS_POM_WR_BIT          = 0x02;
const uint8_t BIDIB_CS_POM_WR_BYTE         = 0x03;

// --- Booster Constants ---
const uint8_t BIDIB_BST_STATE_OFF          = 0x00;     ///< Booster is off (general)
const uint8_t BIDIB_BST_STATE_OFF_SHORT    = 0x01;     ///< Booster is off due to short circuit
const uint8_t BIDIB_BST_STATE_OFF_OVERHEAT = 0x02;     ///< Booster is off due to overheating (OFF_HOT)
const uint8_t BIDIB_BST_STATE_OFF_NOPOWER  = 0x03;     ///< Booster has no mains
const uint8_t BIDIB_BST_STATE_OFF_GO_REQ   = 0x04;     ///< Booster off and local go request is present
const uint8_t BIDIB_BST_STATE_OFF_HERE     = 0x05;     ///< Booster off (was turned off by a local key)
const uint8_t BIDIB_BST_STATE_OFF_NO_DCC   = 0x06;     ///< Booster is off (no DCC input)
const uint8_t BIDIB_BST_STATE_ON           = 0x80;     ///< Booster is on
const uint8_t BIDIB_BST_STATE_ON_LIMIT     = 0x81;     ///< Booster is on and in current-limiting mode
const uint8_t BIDIB_BST_STATE_ON_HOT       = 0x82;     ///< Booster on and is getting hot
const uint8_t BIDIB_BST_STATE_ON_STOP_REQ  = 0x83;     ///< Booster on and a local stop request is present
const uint8_t BIDIB_BST_STATE_ON_HERE      = 0x84;     ///< Booster on (was turned on by a local key)

const uint8_t BIDIB_BST_DIAG_CURRENT       = 0x00;     ///< Diagnostic key for current (DIAG_I)
const uint8_t BIDIB_BST_DIAG_VOLTAGE       = 0x01;     ///< Diagnostic key for voltage (DIAG_V)
const uint8_t BIDIB_BST_DIAG_TEMP          = 0x02;     ///< Diagnostic key for temperature (DIAG_T)

// --- Firmware Update Constants ---
const uint8_t BIDIB_MSG_FW_UPDATE_OP_ENTER   = 0x00;   ///< Enter update mode
const uint8_t BIDIB_MSG_FW_UPDATE_OP_EXIT    = 0x01;   ///< Exit update mode
const uint8_t BIDIB_MSG_FW_UPDATE_OP_SETDEST = 0x02;   ///< Set destination memory
const uint8_t BIDIB_MSG_FW_UPDATE_OP_DATA    = 0x03;   ///< Transmit firmware data
const uint8_t BIDIB_MSG_FW_UPDATE_OP_DONE    = 0x04;   ///< End of data transmission

const uint8_t BIDIB_MSG_FW_UPDATE_STAT_READY =   0;    ///< Node is ready
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_EXIT  =   1;    ///< Node is exiting
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_DATA  =   2;    ///< Node is expecting data
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_ERROR = 255;    ///< Error occurred

const uint8_t BIDIB_MAX_NODES = 32;

// --- Port Types ---
const uint8_t BIDIB_PORTTYPE_SWITCH        = 0;
const uint8_t BIDIB_PORTTYPE_LIGHT         = 1;
const uint8_t BIDIB_PORTTYPE_SERVO         = 2;
const uint8_t BIDIB_PORTTYPE_SOUND         = 3;
const uint8_t BIDIB_PORTTYPE_MOTOR         = 4;
const uint8_t BIDIB_PORTTYPE_ANALOGOUT     = 5;
const uint8_t BIDIB_PORTTYPE_BACKLIGHT     = 6;
const uint8_t BIDIB_PORTTYPE_SWITCHPAIR    = 7;
const uint8_t BIDIB_PORTTYPE_INPUT         = 15;

// --- Port Configuration Enums ---
const uint8_t BIDIB_PCFG_NONE              = 0x00;
const uint8_t BIDIB_PCFG_LEVEL_PORT_ON     = 0x01;
const uint8_t BIDIB_PCFG_LEVEL_PORT_OFF    = 0x02;
const uint8_t BIDIB_PCFG_DIMM_UP           = 0x03;
const uint8_t BIDIB_PCFG_DIMM_DOWN         = 0x04;
const uint8_t BIDIB_PCFG_OUTPUT_MAP        = 0x06;
const uint8_t BIDIB_PCFG_SERVO_ADJ_L       = 0x07;
const uint8_t BIDIB_PCFG_SERVO_ADJ_H       = 0x08;
const uint8_t BIDIB_PCFG_SERVO_SPEED       = 0x09;
const uint8_t BIDIB_PCFG_TICKS             = 0x0B;
const uint8_t BIDIB_PCFG_SWITCH_CTRL       = 0x0D;
const uint8_t BIDIB_PCFG_INPUT_CTRL        = 0x0E;
const uint8_t BIDIB_PCFG_RECONFIG          = 0x81;
const uint8_t BIDIB_PCFG_CONTINUE          = 0xFF;

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
const uint8_t BIDIB_FEATURE_STRING_SIZE       = 1;     ///< Maximum size of strings
const uint8_t BIDIB_FEATURE_MSG_RECEIVE_COUNT = 2;     ///< How many messages can be received at once
const uint8_t FEATURE_BM_SECACK_AVAILABLE     = 2;     ///< Indicates if Secure-ACK is supported
const uint8_t FEATURE_BM_SECACK_ON            = 3;     ///< Enables the Secure-ACK mechanism

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

/// @brief Callback function type for vendor ACK reports.
/// @param node_addr The address of the node that sent the ACK.
/// @param status The acknowledgement status.
typedef void (*VendorAckCallback)(uint8_t node_addr, uint8_t status);

/// @brief Callback function type for vendor data reports.
/// @param node_addr The address of the node that sent the data.
/// @param name The name of the vendor-specific value.
/// @param value The value of the vendor-specific parameter.
typedef void (*VendorDataCallback)(uint8_t node_addr, const char* name, const char* value);

/// @brief Callback function type for firmware update status reports.
/// @param status The status of the firmware update (see BIDIB_MSG_FW_UPDATE_STAT_* constants).
/// @param detail Additional detail for the status (e.g., error code).
typedef void (*FirmwareUpdateStatusCallback)(uint8_t status, uint8_t detail);

/// @brief Callback function type for LC port status reports.
/// @param portType The type of the port.
/// @param portNum The number of the port.
/// @param state The state of the port.
typedef void (*LcStatCallback)(uint8_t portType, uint8_t portNum, uint8_t state);

/// @brief Callback function type for LC port configuration reports.
/// @param portType The type of the port.
/// @param portNum The number of the port.
/// @param p_enum The configuration parameter enum.
/// @param p_val The configuration parameter value.
typedef void (*LcConfigXCallback)(uint8_t portType, uint8_t portNum, uint8_t p_enum, uint8_t p_val);

/// @brief Callback function type for LC wait notifications.
/// @param portType The type of the port.
/// @param portNum The number of the port.
/// @param time The wait time.
typedef void (*LcWaitCallback)(uint8_t portType, uint8_t portNum, uint8_t time);


//================================================================================
// Secure ACK Configuration
//================================================================================

const unsigned long SECURE_ACK_TIMEOUT      = 1000; ///< Timeout in milliseconds for Secure-ACK
const uint8_t       SECURE_ACK_RETRIES      =    3; ///< Number of retries for a Secure-ACK message
const uint8_t       MAX_PENDING_SECURE_ACKS =    8; ///< Maximum number of parallel Secure-ACKs

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
    virtual void update();

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

    // --- Vendor-Specific Functions ---

    /// @brief Enables vendor-specific configuration mode on a node.
    /// @param node_addr The address of the target node.
    void vendorEnable(uint8_t node_addr);

    /// @brief Disables vendor-specific configuration mode on a node.
    /// @param node_addr The address of the target node.
    void vendorDisable(uint8_t node_addr);

    /// @brief Registers a callback function to be called when a vendor acknowledgement is received.
    /// @param callback The function to be called.
    void onVendorAck(VendorAckCallback callback);

    /// @brief Reads a vendor-specific parameter from a node.
    /// @param node_addr The address of the target node.
    /// @param name The name of the parameter to read.
    void vendorGet(uint8_t node_addr, const char* name);

    /// @brief Sets a vendor-specific parameter on a node.
    /// @param node_addr The address of the target node.
    /// @param name The name of the parameter to set.
    /// @param value The value to set for the parameter.
    void vendorSet(uint8_t node_addr, const char* name, const char* value);

    /// @brief Registers a callback function to be called when vendor data is received.
    /// @param callback The function to be called.
    void onVendorData(VendorDataCallback callback);

    // --- Firmware Update Functions ---

    /// @brief Sends a firmware update operation to a node.
    /// @param node_addr The address of the target node.
    /// @param op The operation to perform (see BIDIB_MSG_FW_UPDATE_OP_* constants).
    /// @param data A pointer to the data payload for the operation.
    /// @param len The length of the data payload.
    void firmwareUpdateOperation(uint8_t node_addr, uint8_t op, const uint8_t* data = nullptr, size_t len = 0);

    /// @brief Instructs a node to enter firmware update mode.
    /// @param node_addr The address of the target node.
    void enterFirmwareUpdateMode(uint8_t node_addr);

    /// @brief Instructs a node to exit firmware update mode and restart.
    /// @param node_addr The address of the target node.
    void exitFirmwareUpdateMode(uint8_t node_addr);

    /// @brief Sets the destination memory area for the firmware update.
    /// @param node_addr The address of the target node.
    /// @param destination The destination memory (e.g., 0 for Flash, 1 for EEPROM).
    void setFirmwareUpdateDestination(uint8_t node_addr, uint8_t destination);

    /// @brief Sends a line of firmware data to the node.
    /// @param node_addr The address of the target node.
    /// @param data A pointer to the data to send.
    /// @param len The length of the data.
    void sendFirmwareUpdateData(uint8_t node_addr, const uint8_t* data, size_t len);

    /// @brief Signals the end of data transmission for the current memory destination.
    /// @param node_addr The address of the target node.
    void signalFirmwareUpdateDone(uint8_t node_addr);

    /// @brief Registers a callback function to be called when a firmware update status report is received.
    /// @param callback The function to be called.
    void onFirmwareUpdateStatus(FirmwareUpdateStatusCallback callback);

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

    // --- Light Control Functions ---

    /// @brief Sets the state of a Light Control port.
    /// @param portType The type of the port (BIDIB_PORTTYPE_*).
    /// @param portNum The number of the port.
    /// @param state The state to set.
    void setLcOutput(uint8_t portType, uint8_t portNum, uint8_t state);

    /// @brief Sets a configuration parameter for a Light Control port.
    /// @param portType The type of the port.
    /// @param portNum The number of the port.
    /// @param enumVal The configuration enum (BIDIB_PCFG_*).
    /// @param value The configuration value.
    void setLcConfigX(uint8_t portType, uint8_t portNum, uint8_t enumVal, uint8_t value);

    /// @brief Requests the configuration of a Light Control port.
    /// @param portType The type of the port.
    /// @param portNum The number of the port.
    void getLcConfigX(uint8_t portType, uint8_t portNum);

    /// @brief Registers a callback function for LC port status reports.
    void onLcStat(LcStatCallback callback);

    /// @brief Registers a callback function for LC configuration reports.
    void onLcConfigX(LcConfigXCallback callback);

    /// @brief Registers a callback function for LC wait notifications.
    void onLcWait(LcWaitCallback callback);

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
public:
    BiDiBNode _node_table[BIDIB_MAX_NODES];
    uint8_t _node_count;
protected:
    bool _isLoggedIn;
    uint8_t _track_state;
    DriveAckCallback _driveAckCallback;
    AccessoryAckCallback _accessoryAckCallback;
    PomAckCallback _pomAckCallback;
    BoosterStatusCallback _boosterStatusCallback;
    BoosterDiagnosticCallback _boosterDiagnosticCallback;
    VendorAckCallback _vendorAckCallback;
    VendorDataCallback _vendorDataCallback;
    OccupancyCallback _occupancyCallback;
    OccupancyMultipleCallback _occupancyMultipleCallback;
    AddressCallback _addressCallback;
    SpeedCallback _speedCallback;
    CvCallback _cvCallback;
    AccessoryStateCallback _accessoryStateCallback;
    FirmwareUpdateStatusCallback _firmwareUpdateStatusCallback;
    LcStatCallback _lcStatCallback;
    LcConfigXCallback _lcConfigXCallback;
    LcWaitCallback _lcWaitCallback;

protected:
    // Receive buffer state
    enum FSM_STATE { FSM_IDLE, FSM_IN_MSG, FSM_IN_MSG_ESCAPED };
    FSM_STATE _rx_state;
    uint8_t _rx_buffer[256];
    uint8_t _rx_ptr;

    /// @brief Receives and validates an incoming BiDiB message from the serial stream.
    /// @param msg A reference to a BiDiBMessage object to store the received message.
    /// @return True if a complete and valid message was received, false otherwise.
    virtual bool receiveMessage(BiDiBMessage &msg);

    /// @brief Processes a single byte from the serial stream for message assembly.
    /// @param b The byte to process.
    /// @return True if a complete message has been assembled, false otherwise.
    virtual bool processByte(uint8_t b);

private:
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

protected:
    Stream *bidib_serial;
private:
    uint8_t protocol_version[2] = {0, 7}; // V 0.7

    PendingSecureAck _pendingSecureAcks[MAX_PENDING_SECURE_ACKS];
};

#endif
