#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

BiDiB bidib;
MockStream mockSerial;

// Helper to build a framed message using the library's own CRC logic
void build_message(uint8_t* buffer, size_t& size, const uint8_t* payload, size_t payload_size) {
    buffer[0] = BIDIB_MAGIC;
    memcpy(&buffer[1], payload, payload_size);
    buffer[payload_size + 1] = bidib.calculateCrc(payload, payload_size);
    buffer[payload_size + 2] = BIDIB_MAGIC;
    size = payload_size + 3;
}

void setUp(void) {
    ArduinoFakeReset();
    mockSerial.clear();
    bidib.begin(mockSerial);
}

void tearDown(void) {
}

void test_host_handles_logon_from_new_node() {
    // 1. Simulate a logon request from a new node
    uint8_t client_unique_id[] = {0x81, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t logon_payload[11];
    logon_payload[0] = 0x0A; // length
    logon_payload[1] = 0x00; // address 0
    logon_payload[2] = 0x01; // msg_num 1
    logon_payload[3] = MSG_LOGON;
    memcpy(&logon_payload[4], client_unique_id, 7);

    uint8_t logon_request[sizeof(logon_payload) + 3];
    size_t logon_request_size;
    build_message(logon_request, logon_request_size, logon_payload, sizeof(logon_payload));
    mockSerial.addIncoming(logon_request, logon_request_size);

    for (int i = 0; i < 20; ++i) { bidib.update(); }
    bidib.handleMessages();

    // 2. Verify that the host sends a LOGON_ACK
    TEST_ASSERT_GREATER_THAN(0, mockSerial.available_outgoing());

    uint8_t expected_ack_payload[13];
    expected_ack_payload[0] = 0x0C; // length
    expected_ack_payload[1] = 0x00; // address
    expected_ack_payload[2] = 0x01; // msg_num
    expected_ack_payload[3] = MSG_LOGON_ACK;
    expected_ack_payload[4] = 0x00; // node_table_version
    expected_ack_payload[5] = 0x01; // new node address
    memcpy(&expected_ack_payload[6], client_unique_id, 7);

    uint8_t expected_ack[sizeof(expected_ack_payload) + 3];
    size_t expected_ack_size;
    build_message(expected_ack, expected_ack_size, expected_ack_payload, sizeof(expected_ack_payload));

    uint8_t actual_ack[expected_ack_size];
    mockSerial.read_outgoing(actual_ack, expected_ack_size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_ack, actual_ack, expected_ack_size);

    // 3. Verify that the host sends a NODE_NEW broadcast
    TEST_ASSERT_GREATER_THAN(0, mockSerial.available_outgoing());

    uint8_t expected_node_new_payload[13];
    expected_node_new_payload[0] = 0x0C; // length
    expected_node_new_payload[1] = 0x00; // broadcast address
    expected_node_new_payload[2] = 0x00; // msg_num 0
    expected_node_new_payload[3] = MSG_NODE_NEW;
    expected_node_new_payload[4] = 0x00; // node_table_version
    expected_node_new_payload[5] = 0x01; // new node address
    memcpy(&expected_node_new_payload[6], client_unique_id, 7);

    uint8_t expected_node_new[sizeof(expected_node_new_payload) + 3];
    size_t expected_node_new_size;
    build_message(expected_node_new, expected_node_new_size, expected_node_new_payload, sizeof(expected_node_new_payload));

    uint8_t actual_node_new[expected_node_new_size];
    mockSerial.read_outgoing(actual_node_new, expected_node_new_size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_node_new, actual_node_new, expected_node_new_size);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_host_handles_logon_from_new_node);
    UNITY_END();
    return 0;
}
