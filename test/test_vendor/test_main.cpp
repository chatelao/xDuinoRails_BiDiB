#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

MockStream mockStream;
BiDiB bidib;

//================================================================================
// Test Variables
//================================================================================

uint8_t vendorAckNodeAddr;
uint8_t vendorAckStatus;
bool vendorAckReceived = false;

uint8_t vendorDataNodeAddr;
char vendorDataName[32];
char vendorDataValue[32];
bool vendorDataReceived = false;

//================================================================================
// Mocks
//================================================================================

void vendorAckCallback(uint8_t node_addr, uint8_t status) {
    vendorAckNodeAddr = node_addr;
    vendorAckStatus = status;
    vendorAckReceived = true;
}

void vendorDataCallback(uint8_t node_addr, const char* name, const char* value) {
    vendorDataNodeAddr = node_addr;
    strncpy(vendorDataName, name, sizeof(vendorDataName));
    strncpy(vendorDataValue, value, sizeof(vendorDataValue));
    vendorDataReceived = true;
}

//================================================================================
// Setup and Teardown
//================================================================================

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib.begin(mockStream);

    vendorAckReceived = false;
    vendorDataReceived = false;

    bidib.onVendorAck(vendorAckCallback);
    bidib.onVendorData(vendorDataCallback);
}

void tearDown(void) {}

//================================================================================
// Helper Functions
//================================================================================

void build_and_escape(uint8_t* dest, const uint8_t* payload, size_t payload_size) {
    uint8_t crc = bidib.calculateCrc(payload, payload_size);
    size_t dest_index = 0;
    dest[dest_index++] = 0xFE;

    for (size_t i=0; i < payload_size; ++i) {
        if (payload[i] == 0xFE || payload[i] == 0xFD) {
            dest[dest_index++] = 0xFD;
            dest[dest_index++] = payload[i] ^ 0x20;
        } else {
            dest[dest_index++] = payload[i];
        }
    }

    if (crc == 0xFE || crc == 0xFD) {
        dest[dest_index++] = 0xFD;
        dest[dest_index++] = crc ^ 0x20;
    } else {
        dest[dest_index++] = crc;
    }

    dest[dest_index++] = 0xFE;
}

//================================================================================
// Test Cases
//================================================================================

void test_vendorEnable() {
    bidib.vendorEnable(10);

    const uint8_t payload[] = { 0x04, 0x0A, 0x00, 0x00, 0x70 };
    uint8_t expected[sizeof(payload) + 3]; // MAGIC, CRC, MAGIC
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_vendorDisable() {
    bidib.vendorDisable(10);

    const uint8_t payload[] = { 0x04, 0x0A, 0x00, 0x00, 0x71 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_vendorGet() {
    bidib.vendorGet(10, "test_name");

    const uint8_t payload[] = { 0x0E, 0x0A, 0x00, 0x00, 0x73, 't','e','s','t','_','n','a','m','e', 0x00 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_vendorSet() {
    bidib.vendorSet(10, "test_name", "test_value");

    const uint8_t payload[] = { 0x19, 0x0A, 0x00, 0x00, 0x72, 't','e','s','t','_','n','a','m','e', '=', 't','e','s','t','_','v','a','l','u','e', 0x00 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_handleVendorAck() {
    const uint8_t payload[] = { 0x05, 0x0A, 0x00, 0x00, 0xF1, 0x01 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(vendorAckReceived);
    TEST_ASSERT_EQUAL(10, vendorAckNodeAddr);
    TEST_ASSERT_EQUAL(1, vendorAckStatus);
}

void test_handleVendorData() {
    const uint8_t payload[] = { 0x19, 0x0A, 0x00, 0x00, 0xF0, 't','e','s','t','_','n','a','m','e', '=', 't','e','s','t','_','v','a','l','u','e', 0x00 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(vendorDataReceived);
    TEST_ASSERT_EQUAL(10, vendorDataNodeAddr);
    TEST_ASSERT_EQUAL_STRING("test_name", vendorDataName);
    TEST_ASSERT_EQUAL_STRING("test_value", vendorDataValue);
}

//================================================================================
// Main
//================================================================================

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_vendorEnable);
    RUN_TEST(test_vendorDisable);
    RUN_TEST(test_vendorGet);
    RUN_TEST(test_vendorSet);
    RUN_TEST(test_handleVendorAck);
    RUN_TEST(test_handleVendorData);
    UNITY_END();
    return 0;
}
