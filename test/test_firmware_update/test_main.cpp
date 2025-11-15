#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"
#include "../../src/crc8.h"
#include <vector>
#include <numeric>

using namespace fakeit;

BiDiB bidib;
MockStream mockStream;

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib.begin(mockStream);

    // Manually add a node to the table for testing fw update on a specific node
    BiDiBNode node1;
    uint8_t uid[] = {0x13, 0, 0, 0, 0, 0, 1};
    memcpy(node1.unique_id, uid, 7);
    // This is a hack, directly accessing private members.
    // In a real scenario, this would be populated by the logon process.
    memcpy(bidib._node_table[1].unique_id, uid, 7);
    bidib._node_count = 2;
}

void tearDown(void) {}

// Helper to verify the outgoing message
void verify_message(const std::vector<uint8_t>& content_bytes) {
    // Reconstruct the full message as it should be sent on the wire
    std::vector<uint8_t> expected_message;
    expected_message.push_back(BIDIB_MAGIC);

    uint8_t crc = 0;
    for (uint8_t byte : content_bytes) {
        crc = crc8_table[crc ^ byte];
        if (byte == BIDIB_MAGIC || byte == BIDIB_ESCAPE) {
            expected_message.push_back(BIDIB_ESCAPE);
            expected_message.push_back(byte ^ 0x20);
        } else {
            expected_message.push_back(byte);
        }
    }

    if (crc == BIDIB_MAGIC || crc == BIDIB_ESCAPE) {
        expected_message.push_back(BIDIB_ESCAPE);
        expected_message.push_back(crc ^ 0x20);
    } else {
        expected_message.push_back(crc);
    }
    expected_message.push_back(BIDIB_MAGIC);

    uint8_t buffer[128];
    size_t available = mockStream.available_outgoing();
    TEST_ASSERT_EQUAL(expected_message.size(), available);
    mockStream.read_outgoing(buffer, available);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_message.data(), buffer, expected_message.size());
}

volatile uint8_t fw_update_status;
volatile uint8_t fw_update_detail;
void fwUpdateCallback(uint8_t status, uint8_t detail) {
    fw_update_status = status;
    fw_update_detail = detail;
}

// Helper to build a valid incoming message stream
void create_incoming_message(const std::vector<uint8_t>& content) {
    std::vector<uint8_t> message;
    message.push_back(BIDIB_MAGIC);
    uint8_t crc = 0;
    for(uint8_t byte : content) {
        crc = crc8_table[crc ^ byte];
        message.push_back(byte);
    }
    message.push_back(crc);
    message.push_back(BIDIB_MAGIC);
    mockStream.addIncoming(message.data(), message.size());
}

void test_firmwareUpdate_end_to_end_flow() {
    uint8_t node_addr = 1;
    bidib.onFirmwareUpdateStatus(fwUpdateCallback);

    // 1. Host sends ENTER command
    bidib.enterFirmwareUpdateMode(node_addr); // This requires node 1 to exist
    // 2. Node responds with READY
    create_incoming_message({5, 1, 0, 0, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_READY});
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_READY, fw_update_status);

    // 3. Host sends SETDEST command
    bidib.setFirmwareUpdateDestination(node_addr, 0);
    // 4. Node responds with DATA (ready for data)
    create_incoming_message({5, 1, 0, 0, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_DATA});
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_DATA, fw_update_status);

    // 5. Host sends data
    const uint8_t data[] = {0x01, 0x02, 0x03};
    bidib.sendFirmwareUpdateData(node_addr, data, sizeof(data));

    // 6. Host signals DONE
    bidib.signalFirmwareUpdateDone(node_addr);
    // 7. Node responds with READY again
    create_incoming_message({5, 1, 0, 0, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_READY});
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_READY, fw_update_status);

    // 8. Host sends EXIT command
    bidib.exitFirmwareUpdateMode(node_addr);
    // 9. Node responds with EXIT
    create_incoming_message({5, 1, 0, 0, MSG_FW_UPDATE_STAT, BIDIB_MSG_FW_UPDATE_STAT_EXIT});
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(BIDIB_MSG_FW_UPDATE_STAT_EXIT, fw_update_status);
}


void test_enterFirmwareUpdateMode_sends_correct_message() {
    uint8_t node_addr = 0;
    bidib.enterFirmwareUpdateMode(node_addr);

    verify_message({
        11,   // Length
        0,    // Address
        0,    // MSG_NUM
        MSG_FW_UPDATE_OP,
        BIDIB_MSG_FW_UPDATE_OP_ENTER,
        0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 // UID from constructor
    });
}

void test_exitFirmwareUpdateMode_sends_correct_message() {
    uint8_t node_addr = 1;
    bidib.exitFirmwareUpdateMode(node_addr);

    verify_message({
        5,    // Length
        1, 0, // Address
        0,    // MSG_NUM
        MSG_FW_UPDATE_OP,
        BIDIB_MSG_FW_UPDATE_OP_EXIT,
    });
}

void test_setFirmwareUpdateDestination_sends_correct_message() {
    uint8_t node_addr = 1;
    uint8_t destination = 0; // Flash
    bidib.setFirmwareUpdateDestination(node_addr, destination);

    verify_message({
        6,    // Length
        1, 0, // Address
        0,    // MSG_NUM
        MSG_FW_UPDATE_OP,
        BIDIB_MSG_FW_UPDATE_OP_SETDEST,
        0,    // Destination
    });
}

void test_sendFirmwareUpdateData_sends_correct_message() {
    uint8_t node_addr = 1;
    const uint8_t data[] = {':', '1', '0', '0', '0', '0', '0', '0', '0', '0', '2', '4', '6', '8', 'A', 'C', 'E', '0'};

    std::vector<uint8_t> content;
    content.push_back(2 + 1 + 1 + 1 + sizeof(data)); // addr_len + num + type + op + data
    content.push_back(1); content.push_back(0); // address
    content.push_back(0); // msg_num
    content.push_back(MSG_FW_UPDATE_OP);
    content.push_back(BIDIB_MSG_FW_UPDATE_OP_DATA);
    content.insert(content.end(), data, data + sizeof(data));

    bidib.sendFirmwareUpdateData(node_addr, data, sizeof(data));
    verify_message(content);
}

void test_signalFirmwareUpdateDone_sends_correct_message() {
    uint8_t node_addr = 1;
    bidib.signalFirmwareUpdateDone(node_addr);

    verify_message({
        5,    // Length
        1, 0, // Address
        0,    // MSG_NUM
        MSG_FW_UPDATE_OP,
        BIDIB_MSG_FW_UPDATE_OP_DONE,
    });
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_enterFirmwareUpdateMode_sends_correct_message);
    RUN_TEST(test_exitFirmwareUpdateMode_sends_correct_message);
    RUN_TEST(test_setFirmwareUpdateDestination_sends_correct_message);
    RUN_TEST(test_sendFirmwareUpdateData_sends_correct_message);
    RUN_TEST(test_signalFirmwareUpdateDone_sends_correct_message);
    RUN_TEST(test_firmwareUpdate_end_to_end_flow);
    UNITY_END();
    return 0;
}
