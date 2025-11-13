#include <ArduinoFake.h>
#include <unity.h>

#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Global state for callbacks
static bool speedCallbackCalled = false;
static uint16_t receivedAddress = 0;
static uint16_t receivedSpeed = 0;

void speedCallback(uint16_t address, uint16_t speed) {
    speedCallbackCalled = true;
    receivedAddress = address;
    receivedSpeed = speed;
}

void setUp(void) {
    ArduinoFakeReset();
    speedCallbackCalled = false;
    receivedAddress = 0;
    receivedSpeed = 0;
}

void tearDown(void) {}

void test_speed_reporting() {
    MockStream mockStream;
    BiDiB bidib;
    bidib.begin(mockStream);
    bidib.onSpeedUpdate(speedCallback);

    // Create a MSG_BM_SPEED message
    uint8_t msg[] = {
        0xFE, // Start Magic
        0x07, // Length
        0x00, // Address
        0x00, // Msg Num
        MSG_BM_SPEED,
        0x04, 0x00, // Loco Address 4
        0x90, 0x01, // Speed 400
        0x00, // CRC (placeholder)
        0xFE  // End Magic
    };
    uint8_t crc = bidib.calculateCrc(&msg[1], 8);
    msg[9] = crc;

    // Simulate receiving the message
    mockStream.addIncoming(msg, sizeof(msg));
    bidib.update();
    bidib.handleMessages();

    // Verify callback was called with correct data
    TEST_ASSERT_TRUE(speedCallbackCalled);
    TEST_ASSERT_EQUAL(4, receivedAddress);
    TEST_ASSERT_EQUAL(400, receivedSpeed);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_speed_reporting);
    return UNITY_END();
}
