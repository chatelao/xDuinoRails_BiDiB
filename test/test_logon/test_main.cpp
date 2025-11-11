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

void test_handle_get_node_table_all() {
    uint8_t payload[] = { 0x03, 0x00, 0x01, MSG_NODETAB_GETALL };
    uint8_t request[sizeof(payload) + 3];
    size_t request_size;
    build_message(request, request_size, payload, sizeof(payload));
    mockSerial.addIncoming(request, request_size);

    for (int i = 0; i < 10; ++i) { // Call update multiple times
        bidib.update();
    }
    bidib.handleMessages();

    TEST_ASSERT_GREATER_THAN(0, mockSerial.available_outgoing());

    uint8_t expected_payload[] = { 0x05, 0x00, 0x01, MSG_NODETAB_COUNT, 0x00, 0x00 };
    uint8_t expected[sizeof(expected_payload) + 3];
    size_t expected_size;
    build_message(expected, expected_size, expected_payload, sizeof(expected_payload));

    uint8_t actual[expected_size];
    mockSerial.read_outgoing(actual, expected_size);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, expected_size);
}

void test_handle_get_node_table_next() {
    uint8_t payload[] = { 0x03, 0x00, 0x02, MSG_NODETAB_GETNEXT };
    uint8_t request[sizeof(payload) + 3];
    size_t request_size;
    build_message(request, request_size, payload, sizeof(payload));
    mockSerial.addIncoming(request, request_size);

    for (int i = 0; i < 10; ++i) { // Call update multiple times
        bidib.update();
    }
    bidib.handleMessages();

    TEST_ASSERT_GREATER_THAN(0, mockSerial.available_outgoing());

    uint8_t expected_payload[] = { 0x04, 0x00, 0x02, MSG_NODE_NA, 0x00 };
    uint8_t expected[sizeof(expected_payload) + 3];
    size_t expected_size;
    build_message(expected, expected_size, expected_payload, sizeof(expected_payload));

    uint8_t actual[expected_size];
    mockSerial.read_outgoing(actual, expected_size);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, expected_size);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_handle_get_node_table_all);
    RUN_TEST(test_handle_get_node_table_next);
    UNITY_END();
    return 0;
}
