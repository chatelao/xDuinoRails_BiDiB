#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

// Test-spezifische Klasse, um auf interne Zustände zugreifen zu können
class TestBiDiB : public BiDiB {
public:
    // Expose protected members for testing
    using BiDiB::_lastMessage;
    using BiDiB::_messageAvailable;

    uint8_t getTrackState() { return _track_state; }

    void setTestLastMessage(const BiDiBMessage& msg) {
        _lastMessage = msg;
        _messageAvailable = true;
    }
};

TestBiDiB bidib;
MockStream mockSerial;

// Global variables to check if the callback was called
bool callback_fired = false;
uint16_t callback_address = 0;
uint8_t callback_status = 0;

// Callback function for drive acknowledgements
void driveAckCallback(uint16_t address, uint8_t status) {
    callback_fired = true;
    callback_address = address;
    callback_status = status;
}

void setUp(void) {
    bidib.begin(mockSerial);
    mockSerial.clear();
    // Reset callback state before each test
    callback_address = 0;
    callback_status = 0;
    callback_fired = false;
    bidib.onDriveAck(nullptr);
}

void tearDown(void) {
    // clean up after test
}

void test_send_track_off(void) {
    bidib.setTrackState(BIDIB_CS_STATE_OFF);

    // FE 04 00 00 48 00 96 FE
    TEST_ASSERT_EQUAL(8, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(4, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_CS_SET_STATE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_OFF, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(150, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_send_track_stop(void) {
    bidib.setTrackState(BIDIB_CS_STATE_STOP);

    // FE 04 00 00 48 01 C8 FE
    TEST_ASSERT_EQUAL(8, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(4, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_CS_SET_STATE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_STOP, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(200, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_send_track_go(void) {
    bidib.setTrackState(BIDIB_CS_STATE_GO);

    // FE 04 00 00 48 02 2A FE
    TEST_ASSERT_EQUAL(8, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(4, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_CS_SET_STATE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_GO, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(42, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_receive_track_state_off(void) {
    BiDiBMessage msg;
    msg.msg_type = MSG_CS_STATE;
    msg.data[0] = BIDIB_CS_STATE_OFF;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_OFF, bidib.getTrackState());
}

void test_receive_track_state_stop(void) {
    BiDiBMessage msg;
    msg.msg_type = MSG_CS_STATE;
    msg.data[0] = BIDIB_CS_STATE_STOP;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_STOP, bidib.getTrackState());
}

void test_receive_track_state_go(void) {
    BiDiBMessage msg;
    msg.msg_type = MSG_CS_STATE;
    msg.data[0] = BIDIB_CS_STATE_GO;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_CS_STATE_GO, bidib.getTrackState());
}

void test_send_drive_command(void) {
    bidib.drive(3, 100, 0x10);

    // FE 08 00 00 40 03 00 02 64 10 01 FE
    TEST_ASSERT_EQUAL(12, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(8, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_CS_DRIVE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(3, mockSerial.read_outgoing()); // ADDR_L
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing()); // ADDR_H
    TEST_ASSERT_EQUAL(2, mockSerial.read_outgoing()); // FORMAT
    TEST_ASSERT_EQUAL(100, mockSerial.read_outgoing()); // SPEED
    TEST_ASSERT_EQUAL(0x10, mockSerial.read_outgoing()); // FUNCTIONS
    uint8_t data[] = {8, 0, 0, 0x40, 3, 0, 2, 100, 0x10};
    uint8_t crc = bidib.calculateCrc(data, sizeof(data));
    TEST_ASSERT_EQUAL(crc, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_receive_drive_ack_callback(void) {
    // 1. Register the callback
    bidib.onDriveAck(driveAckCallback);

    // 2. Simulate receiving a MSG_CS_DRIVE_ACK
    BiDiBMessage msg;
    msg.msg_type = MSG_CS_DRIVE_ACK;
    msg.data[0] = 0x03; // Address L
    msg.data[1] = 0x00; // Address H
    msg.data[2] = 0x01; // Status: OK
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();

    // 3. Check if the callback was fired with the correct data
    TEST_ASSERT_TRUE(callback_fired);
    TEST_ASSERT_EQUAL(3, callback_address);
    TEST_ASSERT_EQUAL(1, callback_status);
}

void test_receive_drive_ack_no_callback(void) {
    // 1. Do NOT register the callback
    // bidib.onDriveAck(driveAckCallback);

    // 2. Simulate receiving a MSG_CS_DRIVE_ACK
    BiDiBMessage msg;
    msg.msg_type = MSG_CS_DRIVE_ACK;
    msg.data[0] = 0x03;
    msg.data[1] = 0x00;
    msg.data[2] = 0x01;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();

    // 3. Check that the callback was NOT fired
    TEST_ASSERT_FALSE(callback_fired);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_send_track_off);
    RUN_TEST(test_send_track_stop);
    RUN_TEST(test_send_track_go);
    RUN_TEST(test_receive_track_state_off);
    RUN_TEST(test_receive_track_state_stop);
    RUN_TEST(test_receive_track_state_go);
    RUN_TEST(test_send_drive_command);
    RUN_TEST(test_receive_drive_ack_callback);
    RUN_TEST(test_receive_drive_ack_no_callback);
    UNITY_END();
    return 0;
}
