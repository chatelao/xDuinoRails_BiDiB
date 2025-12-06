#ifndef BiDiB_h
#define BiDiB_h

#include <Arduino.h>

//================================================================================
// BiDiB Protocol Constants
//================================================================================

const uint8_t BIDIB_MAGIC           = 0xFE;  ///< Start and end of a BiDiB message
const uint8_t BIDIB_ESCAPE          = 0xFD; ///< Escape character for MAGIC byte

//================================================================================
// BiDiB Message Types
//================================================================================

// --- System Messages ---
const uint8_t BIDIB_MAX_NODES         =   32;
const uint8_t MSG_SYS_GET_MAGIC       =    1;
const uint8_t MSG_SYS_GET_P_VERSION   =    2;
const uint8_t MSG_SYS_GET_UNIQUE_ID   =    3;
const uint8_t MSG_SYS_ENABLE          =    4;
const uint8_t MSG_SYS_DISABLE         =    5;
const uint8_t MSG_NODETAB_GETALL      =    6;
const uint8_t MSG_NODETAB_GETNEXT     =    7;
const uint8_t MSG_LOGON               =   10;
const uint8_t MSG_SYS_MAGIC           = 0x81;
const uint8_t MSG_SYS_P_VERSION       = 0x82;
const uint8_t MSG_SYS_UNIQUE_ID       = 0x83;
const uint8_t MSG_NODETAB_COUNT       = 0x86;
const uint8_t MSG_NODETAB             = 0x87;
const uint8_t MSG_NODE_NA             = 0x88;
const uint8_t MSG_NODE_NEW            = 0x89;
const uint8_t MSG_NODE_LOST           = 0x8A;
const uint8_t MSG_LOGON_ACK           = 0x8B;

// --- Feature Messages ---
const uint8_t MSG_FEATURE_GETALL      =   11;
const uint8_t MSG_FEATURE_GETNEXT     =   12;
const uint8_t MSG_FEATURE_GET         =   13;
const uint8_t MSG_FEATURE_SET         =   14;
const uint8_t MSG_FEATURE_COUNT       = 0x8C;
const uint8_t MSG_FEATURE             = 0x8D;
const uint8_t MSG_FEATURE_NA          = 0x8E;

// --- Command Station Messages ---
const uint8_t MSG_CS_ALLOCATE         = 0x60;
const uint8_t MSG_CS_SET_STATE        = 0x62;
const uint8_t MSG_CS_DRIVE            = 0x64;
const uint8_t MSG_CS_ACCESSORY        = 0x65;
const uint8_t MSG_CS_BIN_STATE        = 0x66;
const uint8_t MSG_CS_POM              = 0x67;
const uint8_t MSG_CS_RCPLUS           = 0x68;
const uint8_t MSG_CS_PROG             = 0x6F;

const uint8_t MSG_CS_DRIVE_ACK        = 0xE2;
const uint8_t MSG_CS_ACCESSORY_ACK    = 0xE3;
const uint8_t MSG_CS_POM_ACK          = 0xE4; // MSG_UGEN + 0x04 = 0xE4
const uint8_t MSG_CS_DRIVE_MANUAL     = 0xE5;
const uint8_t MSG_CS_DRIVE_EVENT      = 0xE6;
const uint8_t MSG_CS_STATE            = 0xE9; // MSG_UGEN + 0x09 = 0xE9

// --- Occupancy Messages ---
const uint8_t MSG_BM_GET_RANGE        = 0xA0;
const uint8_t MSG_BM_MULTIPLE         = 0xA1;
const uint8_t MSG_BM_OCC              = 0xA2;
const uint8_t MSG_BM_FREE             = 0xA3;
const uint8_t MSG_BM_MIRROR_OCC       = 0xA8;
const uint8_t MSG_BM_MIRROR_FREE      = 0xA9;
const uint8_t MSG_BM_MIRROR_MULTIPLE  = 0xAA;
const uint8_t MSG_BM_ADDRESS          = 0xA4;
const uint8_t MSG_BM_SPEED            = 0xA5;
const uint8_t MSG_BM_CV               = 0xA6;

// --- Accessory Control Messages (Standard BiDiB Accessories) ---
const uint8_t MSG_ACCESSORY_SET       = 0x38;
const uint8_t MSG_ACCESSORY_GET       = 0x39;
const uint8_t MSG_ACCESSORY_STATE     = 0xB8;
const uint8_t MSG_ACCESSORY_NOTIFY    = 0xB9;

// --- Control / Switching Functions (LC) ---
const uint8_t MSG_LC_PORT_QUERY_ALL   = 0x3F; // MSG_DLC + 0x00
const uint8_t MSG_LC_OUTPUT           = 0x40; // MSG_DLC + 0x01
const uint8_t MSG_LC_CONFIG_SET       = 0x41; // Deprecated
const uint8_t MSG_LC_CONFIG_GET       = 0x42; // Deprecated
const uint8_t MSG_LC_KEY_QUERY        = 0x43; // Deprecated
const uint8_t MSG_LC_OUTPUT_QUERY     = 0x44; // Deprecated
const uint8_t MSG_LC_PORT_QUERY       = 0x44; // MSG_DLC + 0x05
const uint8_t MSG_LC_CONFIGX_GET_ALL  = 0x45; // MSG_DLC + 0x06
const uint8_t MSG_LC_CONFIGX_SET      = 0x46; // MSG_DLC + 0x07
const uint8_t MSG_LC_CONFIGX_GET      = 0x47; // MSG_DLC + 0x08

const uint8_t MSG_LC_STAT             = 0xC0; // MSG_ULC + 0x00
const uint8_t MSG_LC_NA               = 0xC1; // MSG_ULC + 0x01
const uint8_t MSG_LC_CONFIG           = 0xC2; // Deprecated
const uint8_t MSG_LC_KEY              = 0xC3; // Deprecated
const uint8_t MSG_LC_WAIT             = 0xC4; // MSG_ULC + 0x04
const uint8_t MSG_LC_CONFIGX          = 0xC6; // MSG_ULC + 0x06

// --- Macros ---
const uint8_t MSG_LC_MACRO_HANDLE     = 0x48; // MSG_DMAC + 0x00
const uint8_t MSG_LC_MACRO_SET        = 0x49; // MSG_DMAC + 0x01
const uint8_t MSG_LC_MACRO_GET        = 0x4A; // MSG_DMAC + 0x02
const uint8_t MSG_LC_MACRO_PARA_SET   = 0x4B; // MSG_DMAC + 0x03
const uint8_t MSG_LC_MACRO_PARA_GET   = 0x4C; // MSG_DMAC + 0x04

const uint8_t MSG_LC_MACRO_STATE      = 0xC8; // MSG_UMAC + 0x00
const uint8_t MSG_LC_MACRO            = 0xC9; // MSG_UMAC + 0x01
const uint8_t MSG_LC_MACRO_PARA       = 0xCA; // MSG_UMAC + 0x02

// --- Booster Messages ---
const uint8_t MSG_BOOST_ON            = 0x50;
const uint8_t MSG_BOOST_OFF           = 0x51;
const uint8_t MSG_BOOST_QUERY         = 0x52;
const uint8_t MSG_BOOST_STAT          = 0xD0;
const uint8_t MSG_BOOST_DIAGNOSTIC    = 0xD1;

// --- User Configuration Messages ---
const uint8_t MSG_VENDOR_ENABLE       = 0x70;
const uint8_t MSG_VENDOR_DISABLE      = 0x71;
const uint8_t MSG_VENDOR_SET          = 0x72;
const uint8_t MSG_VENDOR_GET          = 0x73;
const uint8_t MSG_VENDOR              = 0xF0;
const uint8_t MSG_VENDOR_ACK          = 0xF1;

// --- Firmware Update Messages ---
const uint8_t MSG_FW_UPDATE_OP        = 0x78;
const uint8_t MSG_FW_UPDATE_STAT      = 0xF8;


// --- Command Station Constants ---
const uint8_t BIDIB_CS_STATE_OFF           = 0;  ///< Track voltage is off
const uint8_t BIDIB_CS_STATE_STOP          = 1; ///< Track voltage is on, but zero speed commands are sent
const uint8_t BIDIB_CS_STATE_GO            = 2;   ///< Track voltage is on, normal operation

// --- POM Opcodes ---
const uint8_t BIDIB_CS_POM_RD_BLOCK        = 0x00;
const uint8_t BIDIB_CS_POM_RD_BYTE         = 0x01;
const uint8_t BIDIB_CS_POM_WR_BIT          = 0x02;
const uint8_t BIDIB_CS_POM_WR_BYTE         = 0x03;

// --- Booster Constants ---
const uint8_t BIDIB_BST_STATE_OFF          = 0x00;         ///< Booster is off (general)
const uint8_t BIDIB_BST_STATE_OFF_SHORT    = 0x01;  ///< Booster is off due to short circuit
const uint8_t BIDIB_BST_STATE_OFF_OVERHEAT = 0x02;///< Booster is off due to overheating
const uint8_t BIDIB_BST_STATE_ON           = 0x80;         ///< Booster is on
const uint8_t BIDIB_BST_STATE_ON_LIMIT     = 0x81;   ///< Booster is on and in current-limiting mode

const uint8_t BIDIB_BST_DIAG_CURRENT       = 0x00;     ///< Diagnostic key for current
const uint8_t BIDIB_BST_DIAG_VOLTAGE       = 0x01;     ///< Diagnostic key for voltage
const uint8_t BIDIB_BST_DIAG_TEMP          = 0x02;        ///< Diagnostic key for temperature

// --- Firmware Update Constants ---
const uint8_t BIDIB_MSG_FW_UPDATE_OP_ENTER   = 0x00;   ///< Enter update mode
const uint8_t BIDIB_MSG_FW_UPDATE_OP_EXIT    = 0x01;    ///< Exit update mode
const uint8_t BIDIB_MSG_FW_UPDATE_OP_SETDEST = 0x02; ///< Set destination memory
const uint8_t BIDIB_MSG_FW_UPDATE_OP_DATA    = 0x03;    ///< Transmit firmware data
const uint8_t BIDIB_MSG_FW_UPDATE_OP_DONE    = 0x04;    ///< End of data transmission

const uint8_t BIDIB_MSG_FW_UPDATE_STAT_READY =   0; ///< Node is ready
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_EXIT  =   1;  ///< Node is exiting
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_DATA  =   2;  ///< Node is expecting data
const uint8_t BIDIB_MSG_FW_UPDATE_STAT_ERROR = 255;  ///< Error occurred

// --- LC Port Types ---
const uint8_t BIDIB_PORTTYPE_SWITCH        = 0;     // standard port (on/off)
const uint8_t BIDIB_PORTTYPE_LIGHT         = 1;     // light port
const uint8_t BIDIB_PORTTYPE_SERVO         = 2;     // servo port
const uint8_t BIDIB_PORTTYPE_SOUND         = 3;     // sound
const uint8_t BIDIB_PORTTYPE_MOTOR         = 4;     // motor
const uint8_t BIDIB_PORTTYPE_ANALOGOUT     = 5;     // analog
const uint8_t BIDIB_PORTTYPE_BACKLIGHT     = 6;     // backlight
const uint8_t BIDIB_PORTTYPE_SWITCHPAIR    = 7;     // width: 2, exclusive usage
const uint8_t BIDIB_PORTTYPE_INPUT         = 15;    // simple input (open/closed)

// --- LC Configuration Parameters ---
const uint8_t BIDIB_PCFG_NONE              = 0x00;
const uint8_t BIDIB_PCFG_LEVEL_PORT_ON     = 0x01;
const uint8_t BIDIB_PCFG_LEVEL_PORT_OFF    = 0x02;
const uint8_t BIDIB_PCFG_DIMM_UP           = 0x03;
const uint8_t BIDIB_PCFG_DIMM_DOWN         = 0x04;
const uint8_t BIDIB_PCFG_OUTPUT_MAP        = 0x06;
const uint8_t BIDIB_PCFG_SERVO_ADJ_L       = 0x07;
const uint8_t BIDIB_PCFG_SERVO_ADJ_H       = 0x08;
const uint8_t BIDIB_PCFG_SERVO_SPEED       = 0x09;
const uint8_t BIDIB_PCFG_IO_CTRL           = 0x0a;
const uint8_t BIDIB_PCFG_TICKS             = 0x0b;
const uint8_t BIDIB_PCFG_SWITCH_CTRL       = 0x0d;
const uint8_t BIDIB_PCFG_INPUT_CTRL        = 0x0e;
const uint8_t BIDIB_PCFG_DIMM_UP_8_8       = 0x43;
const uint8_t BIDIB_PCFG_DIMM_DOWN_8_8     = 0x44;
const uint8_t BIDIB_PCFG_RGB               = 0x80;
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

const uint8_t BIDIB_MAX_FEATURES = 64; // Increased to accommodate more features

// --- Feature Constants ---
const uint8_t BIDIB_FEATURE_FW_UPDATE_SUPPORT = 0;     ///< 1 if firmware update is supported
const uint8_t BIDIB_FEATURE_STRING_SIZE       = 1;           ///< Maximum size of strings
const uint8_t BIDIB_FEATURE_MSG_RECEIVE_COUNT = 2;     ///< How many messages can be received at once
const uint8_t FEATURE_BM_SECACK_AVAILABLE     = 2;         ///< Indicates if Secure-ACK is supported (Legacy ID?)
const uint8_t FEATURE_BM_SECACK_ON            = 3;                ///< Enables the Secure-ACK mechanism

// Control Features
const uint8_t FEATURE_CTRL_INPUT_COUNT        = 50;
const uint8_t FEATURE_CTRL_INPUT_NOTIFY       = 51;
const uint8_t FEATURE_CTRL_SWITCH_COUNT       = 52;
const uint8_t FEATURE_CTRL_LIGHT_COUNT        = 53;
const uint8_t FEATURE_CTRL_SERVO_COUNT        = 54;
const uint8_t FEATURE_CTRL_SOUND_COUNT        = 55;
const uint8_t FEATURE_CTRL_MOTOR_COUNT        = 56;
const uint8_t FEATURE_CTRL_ANALOGOUT_COUNT    = 57;
const uint8_t FEATURE_CTRL_STRETCH_DIMM       = 58;
const uint8_t FEATURE_CTRL_BACKLIGHT_COUNT    = 59;
const uint8_t FEATURE_CTRL_MAC_LEVEL          = 60;
const uint8_t FEATURE_CTRL_MAC_SAVE           = 61;
const uint8_t FEATURE_CTRL_MAC_COUNT          = 62;
const uint8_t FEATURE_CTRL_MAC_SIZE           = 63;
const uint8_t FEATURE_CTRL_MAC_START_MAN      = 64;
const uint8_t FEATURE_CTRL_MAC_START_DCC      = 65;
const uint8_t FEATURE_CTRL_PORT_QUERY_AVAILABLE = 66;

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

/// @brief Callback for LC Port Status
typedef void (*LcStatCallback)(uint8_t portL, uint8_t portH, uint8_t portStat);

/// @brief Callback for LC Port Configuration
typedef void (*LcConfigXCallback)(uint8_t portL, uint8_t portH, uint8_t paramCount, const uint8_t* params);

/// @brief Callback for LC NA (Error/Done)
typedef void (*LcNaCallback)(uint8_t portL, uint8_t portH, uint8_t errCause);

/// @brief Callback for LC Wait
typedef void (*LcWaitCallback)(uint8_t portL, uint8_t portH, uint8_t time);

/// @brief Callback for Macro State
typedef void (*LcMacroStateCallback)(uint8_t macroIdx, uint8_t state);

/// @brief Callback for Macro Content
typedef void (*LcMacroCallback)(uint8_t macroIdx, uint8_t itemIdx, uint8_t delay, uint8_t portL, uint8_t portH, uint8_t portStat);

/// @brief Callback for Macro Parameter
typedef void (*LcMacroParaCallback)(uint8_t macroIdx, uint8_t paraIdx, uint32_t value);


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

    // --- Control / Switching Functions (LC) ---

    /// @brief Sets the output state of a port.
    /// @param portL Low byte of port address.
    /// @param portH High byte of port address.
    /// @param state The desired state (see BIDIB_PORT_* operations).
    void setLcOutput(uint8_t portL, uint8_t portH, uint8_t state);

    /// @brief Queries the status of a specific port.
    /// @param portL Low byte of port address.
    /// @param portH High byte of port address.
    void queryLcPort(uint8_t portL, uint8_t portH);

    /// @brief Queries the configuration of a specific port.
    /// @param portL Low byte of port address.
    /// @param portH High byte of port address.
    void getLcConfigX(uint8_t portL, uint8_t portH);

    /// @brief Sets the configuration for a specific port.
    /// @param portL Low byte of port address.
    /// @param portH High byte of port address.
    /// @param paramCount The number of configuration parameters.
    /// @param params Pointer to an array of parameter type/value pairs.
    void setLcConfigX(uint8_t portL, uint8_t portH, uint8_t paramCount, const uint8_t* params);

    // --- Macro Functions ---

    /// @brief Controls a macro (Start, Stop, Restore, Save, Delete).
    /// @param macroIdx The index of the macro.
    /// @param opcode The operation code (see BIDIB_MACRO_*).
    void handleMacro(uint8_t macroIdx, uint8_t opcode);

    /// @brief Sets a macro step.
    /// @param macroIdx The index of the macro.
    /// @param itemIdx The index of the step.
    /// @param delay Delay in ticks.
    /// @param portL Low byte of port address.
    /// @param portH High byte of port address.
    /// @param status Status/Action for the port.
    void setMacro(uint8_t macroIdx, uint8_t itemIdx, uint8_t delay, uint8_t portL, uint8_t portH, uint8_t status);

    /// @brief Gets a macro step.
    /// @param macroIdx The index of the macro.
    /// @param itemIdx The index of the step.
    void getMacro(uint8_t macroIdx, uint8_t itemIdx);

    /// @brief Sets a macro parameter.
    /// @param macroIdx The index of the macro.
    /// @param paraIdx The parameter index.
    /// @param value The value (uint32_t, will be split into 4 bytes).
    void setMacroParameter(uint8_t macroIdx, uint8_t paraIdx, uint32_t value);

    /// @brief Gets a macro parameter.
    /// @param macroIdx The index of the macro.
    /// @param paraIdx The parameter index.
    void getMacroParameter(uint8_t macroIdx, uint8_t paraIdx);

    // --- Callbacks for LC ---
    void onLcStat(LcStatCallback callback);
    void onLcConfigX(LcConfigXCallback callback);
    void onLcNa(LcNaCallback callback);
    void onLcWait(LcWaitCallback callback);
    void onLcMacroState(LcMacroStateCallback callback);
    void onLcMacro(LcMacroCallback callback);
    void onLcMacroPara(LcMacroParaCallback callback);


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

    // LC Callbacks
    LcStatCallback _lcStatCallback;
    LcConfigXCallback _lcConfigXCallback;
    LcNaCallback _lcNaCallback;
    LcWaitCallback _lcWaitCallback;
    LcMacroStateCallback _lcMacroStateCallback;
    LcMacroCallback _lcMacroCallback;
    LcMacroParaCallback _lcMacroParaCallback;

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
    uint8_t protocol_version[2] = {0, 1}; // V 0.1

    PendingSecureAck _pendingSecureAcks[MAX_PENDING_SECURE_ACKS];
};

#endif
