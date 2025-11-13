#include <ArduinoFake.h>
#include <unity.h>

#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Global state for callbacks
static bool cvCallbackCalled = false;
static uint16_t receivedAddress = 0;
static uint16_t receivedCv = 0;
static uint8_t receivedValue = 0;

void cvCallback(uint16_t address, uint16_t cv, uint8_t value) {
    cvCallbackCalled = true;
    receivedAddress = address;
    receivedCv = cv;
    receivedValue = value;
}

void setUp(void) {
    ArduinoFakeReset();
    cvCallbackCalled = false;
    receivedAddress = 0;
    receivedCv = 0;
    receivedValue = 0;
}

void tearDown(void) {}

void test_cv_reporting() {
    MockStream mockStream;
    BiDiB bidib;
    bidib.begin(mockStream);
    bidib.onCvUpdate(cvCallback);

    // Create a MSG_BM_CV message
    uint8_t msg[] = {
        0xFE, // Start Magic
        0x09, // Length
        0x00, // Address
        0x00, // Msg Num
        MSG_BM_CV,
        0x03, 0x00, // Loco Address 3
        0x00, // Decoder Type
        0x05, 0x00, // CV 5
        0xAB, // Value
        0x00, // CRC (placeholder)
        0xFE  // End Magic
    };
    uint8_t crc = bidib.calculateCrc(&msg[1], 10);
    msg[11] = crc;

    // Simulate receiving the message
    mockStream.addIncoming(msg, sizeof(msg));
    bidib.update();
    bidib.handleMessages();

    // Verify callback was called with correct data
    TEST_ASSERT_TRUE(cvCallbackCalled);
    TEST_ASSERT_EQUAL(3, receivedAddress);
    TEST_ASSERT_EQUAL(5, receivedCv);
    TEST_ASSERT_EQUAL(0xAB, receivedValue);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_cv_reporting);
    return UNITY_END();
}
