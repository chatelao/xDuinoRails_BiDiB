#include <ArduinoFake.h>
#include <unity.h>

#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Global variables for callback testing
static bool callbackCalled = false;
static uint8_t receivedDetectorNum = 0;
static uint16_t receivedAddress = 0;

// Test callback function
void testAddressCallback(uint8_t detectorNum, uint16_t address) {
    callbackCalled = true;
    receivedDetectorNum = detectorNum;
    receivedAddress = address;
}

void setUp(void) {
    ArduinoFakeReset();
    callbackCalled = false;
    receivedDetectorNum = 0;
    receivedAddress = 0;
}

void tearDown(void) {
    // Clean up
}

void testAddressReportingCallback() {
    // Arrange
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);
    bidib.onAddress(testAddressCallback);

    // This is a simulated MSG_BM_ADDRESS message for detector 23, address 1025
    // Structure: MAGIC | LEN | ADDR | MSG_NUM | MSG_TYPE | DATA...     | CRC   | MAGIC
    // Data:      DET_NUM | 0   | ADDR_L  | ADDR_H
    // ADDR 1025 = 0x0401 -> ADDR_L = 0x01, ADDR_H = 0x04
    uint8_t msg_data[] = { 0xFE, 0x07, 0x00, 0x00, MSG_BM_ADDRESS, 23, 0, 1, 4, 0x00, 0xFE };

    // Calculate CRC for the message content (from LEN to end of DATA)
    uint8_t crc = bidib.calculateCrc(&msg_data[1], 8);
    msg_data[9] = crc;

    // Act
    mockSerial.addIncoming(msg_data, sizeof(msg_data));
    bidib.update(); // This should read the message
    bidib.handleMessages(); // This should process the message and fire the callback

    // Assert
    TEST_ASSERT_TRUE(callbackCalled);
    TEST_ASSERT_EQUAL_UINT8(23, receivedDetectorNum);
    TEST_ASSERT_EQUAL_UINT16(1025, receivedAddress);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(testAddressReportingCallback);
    return UNITY_END();
}
