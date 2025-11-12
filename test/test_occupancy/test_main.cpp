#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Global variables to be updated by the callbacks for verification
bool occupancyCallbackCalled = false;
uint8_t lastDetectorNum = 0;
bool lastOccupiedState = false;

bool occupancyMultipleCallbackCalled = false;
uint8_t lastBaseNum = 0;
uint8_t lastSize = 0;
uint8_t lastData[8]; // Max data size for this test

// Test callback functions
void testOccupancyCallback(uint8_t detectorNum, bool occupied) {
    occupancyCallbackCalled = true;
    lastDetectorNum = detectorNum;
    lastOccupiedState = occupied;
}

void testOccupancyMultipleCallback(uint8_t baseNum, uint8_t size, const uint8_t* data) {
    occupancyMultipleCallbackCalled = true;
    lastBaseNum = baseNum;
    lastSize = size;
    memcpy(lastData, data, size);
}

void setUp(void) {
    ArduinoFakeReset();
    occupancyCallbackCalled = false;
    occupancyMultipleCallbackCalled = false;
    memset(lastData, 0, sizeof(lastData));
}

void tearDown(void) {}

// Test processing of MSG_BM_OCC
void test_handle_msg_bm_occ() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);
    bidib.onOccupancy(testOccupancyCallback);

    // Simulate receiving a MSG_BM_OCC for detector 12
    uint8_t msg_core[] = { 0x04, 0x00, 0x01, MSG_BM_OCC, 12 };
    uint8_t crc = bidib.calculateCrc(msg_core, sizeof(msg_core));
    uint8_t msg_payload[sizeof(msg_core) + 3];
    msg_payload[0] = BIDIB_MAGIC;
    memcpy(&msg_payload[1], msg_core, sizeof(msg_core));
    msg_payload[sizeof(msg_core) + 1] = crc;
    msg_payload[sizeof(msg_core) + 2] = BIDIB_MAGIC;
    mockSerial.addIncoming(msg_payload, sizeof(msg_payload));

    // Update BiDiB to process the message
    bidib.update();
    bidib.handleMessages();

    // Verify that the callback was called with the correct parameters
    TEST_ASSERT_TRUE(occupancyCallbackCalled);
    TEST_ASSERT_EQUAL(12, lastDetectorNum);
    TEST_ASSERT_TRUE(lastOccupiedState);
}

// Test processing of MSG_BM_FREE
void test_handle_msg_bm_free() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);
    bidib.onOccupancy(testOccupancyCallback);

    // Simulate receiving a MSG_BM_FREE for detector 15
    uint8_t msg_core[] = { 0x04, 0x00, 0x01, MSG_BM_FREE, 15 };
    uint8_t crc = bidib.calculateCrc(msg_core, sizeof(msg_core));
    uint8_t msg_payload[sizeof(msg_core) + 3];
    msg_payload[0] = BIDIB_MAGIC;
    memcpy(&msg_payload[1], msg_core, sizeof(msg_core));
    msg_payload[sizeof(msg_core) + 1] = crc;
    msg_payload[sizeof(msg_core) + 2] = BIDIB_MAGIC;
    mockSerial.addIncoming(msg_payload, sizeof(msg_payload));

    // Update BiDiB to process the message
    bidib.update();
    bidib.handleMessages();

    // Verify that the callback was called with the correct parameters
    TEST_ASSERT_TRUE(occupancyCallbackCalled);
    TEST_ASSERT_EQUAL(15, lastDetectorNum);
    TEST_ASSERT_FALSE(lastOccupiedState);
}

// Test processing of MSG_BM_MULTIPLE
void test_handle_msg_bm_multiple() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);
    bidib.onOccupancyMultiple(testOccupancyMultipleCallback);

    // Simulate receiving a MSG_BM_MULTIPLE for base 8, size 2, data 0b10101010, 0b01010101
    uint8_t msg_data[] = { 0xAA, 0x55 };
    uint8_t msg_core[] = { 0x07, 0x00, 0x01, MSG_BM_MULTIPLE, 8, 2, msg_data[0], msg_data[1] };
    uint8_t crc = bidib.calculateCrc(msg_core, sizeof(msg_core));
    uint8_t msg_payload[sizeof(msg_core) + 3];
    msg_payload[0] = BIDIB_MAGIC;
    memcpy(&msg_payload[1], msg_core, sizeof(msg_core));
    msg_payload[sizeof(msg_core) + 1] = crc;
    msg_payload[sizeof(msg_core) + 2] = BIDIB_MAGIC;
    mockSerial.addIncoming(msg_payload, sizeof(msg_payload));

    // Update BiDiB to process the message
    bidib.update();
    bidib.handleMessages();

    // Verify that the callback was called with the correct parameters
    TEST_ASSERT_TRUE(occupancyMultipleCallbackCalled);
    TEST_ASSERT_EQUAL(8, lastBaseNum);
    TEST_ASSERT_EQUAL(2, lastSize);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_data, lastData, 2);
}


int main() {
    UNITY_BEGIN();
    RUN_TEST(test_handle_msg_bm_occ);
    RUN_TEST(test_handle_msg_bm_free);
    RUN_TEST(test_handle_msg_bm_multiple);
    UNITY_END();
    return 0;
}
