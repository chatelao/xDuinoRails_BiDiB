#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Global state for callbacks
static uint8_t last_fw_update_status = 0;
static uint8_t last_fw_update_detail = 0;

// Mock callback function
void fwUpdateStatusCallback(uint8_t status, uint8_t detail) {
    last_fw_update_status = status;
    last_fw_update_detail = detail;
}

void setUp(void) {
    ArduinoFakeReset();
    last_fw_update_status = 0;
    last_fw_update_detail = 0;
}

void tearDown(void) {}

// Test sending a command to enter firmware update mode
void test_send_fw_update_enter() {
    MockStream mockStream;
    BiDiB bidib;
    bidib.begin(mockStream);

    uint8_t node_addr = 1;
    uint8_t test_uid[] = {0x81, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

    bidib.firmwareUpdateOperation(node_addr, BIDIB_MSG_FW_UPDATE_OP_ENTER, test_uid, 7);

    // Verify the outgoing message
    uint8_t expected_msg[] = {
        0x0C, 0x01, 0x00, 0x00, MSG_FW_UPDATE_OP,
        BIDIB_MSG_FW_UPDATE_OP_ENTER,
        0x81, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    };
    uint8_t crc = bidib.calculateCrc(expected_msg, sizeof(expected_msg));

    TEST_ASSERT_EQUAL(16, mockStream.available_outgoing()); // MAGIC + length + content + crc + MAGIC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockStream.read_outgoing());
    // The length of the address is variable. The BiDiB implementation sends 3 bytes here.
    uint8_t actual_msg[13];
    mockStream.read_outgoing(actual_msg, 13);
    for (int i=0; i < sizeof(expected_msg); ++i) {
        TEST_ASSERT_EQUAL(expected_msg[i], actual_msg[i]);
    }
    TEST_ASSERT_EQUAL(crc, mockStream.read_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockStream.read_outgoing());
}

// Test receiving a 'ready' status message
void test_receive_fw_update_stat_ready() {
    MockStream mockStream;
    BiDiB bidib;
    bidib.begin(mockStream);
    bidib.onFirmwareUpdateStatus(fwUpdateStatusCallback);

    uint8_t incoming_msg[] = {
        0x04, 0x00, 0x00, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_READY
    };
    uint8_t crc = bidib.calculateCrc(incoming_msg, sizeof(incoming_msg));
    uint8_t magic = BIDIB_MAGIC;
    mockStream.addIncoming(&magic, 1);
    mockStream.addIncoming(incoming_msg, sizeof(incoming_msg));
    mockStream.addIncoming(&crc, 1);
    mockStream.addIncoming(&magic, 1);

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_READY, last_fw_update_status);
    TEST_ASSERT_EQUAL(0, last_fw_update_detail);
}

// Test receiving an 'error' status message
void test_receive_fw_update_stat_error() {
    MockStream mockStream;
    BiDiB bidib;
    bidib.begin(mockStream);
    bidib.onFirmwareUpdateStatus(fwUpdateStatusCallback);

    uint8_t error_code = 42; // Example error code
    uint8_t incoming_msg[] = {
        0x05, 0x00, 0x00, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_ERROR, error_code
    };
    uint8_t crc = bidib.calculateCrc(incoming_msg, sizeof(incoming_msg));
    uint8_t magic = BIDIB_MAGIC;
    mockStream.addIncoming(&magic, 1);
    mockStream.addIncoming(incoming_msg, sizeof(incoming_msg));
    mockStream.addIncoming(&crc, 1);
    mockStream.addIncoming(&magic, 1);

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_ERROR, last_fw_update_status);
    TEST_ASSERT_EQUAL(error_code, last_fw_update_detail);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_send_fw_update_enter);
    RUN_TEST(test_receive_fw_update_stat_ready);
    RUN_TEST(test_receive_fw_update_stat_error);
    UNITY_END();
    return 0;
}
