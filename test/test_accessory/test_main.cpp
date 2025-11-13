#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

MockStream mockSerial;
BiDiB bidib;

// Callback flags and variables
bool accessoryStateCallbackCalled = false;
uint8_t callbackAccessoryNum = 0;
uint8_t callbackAspect = 0;

void accessoryStateCallback(uint8_t accessoryNum, uint8_t aspect) {
    accessoryStateCallbackCalled = true;
    callbackAccessoryNum = accessoryNum;
    callbackAspect = aspect;
}

void setUp(void) {
    ArduinoFakeReset();
    mockSerial.clear();
    bidib.begin(mockSerial);
    accessoryStateCallbackCalled = false;
    callbackAccessoryNum = 0;
    callbackAspect = 0;
    bidib.onAccessoryState(accessoryStateCallback);
}

void tearDown(void) {
    // clean up after each test
}

void test_setAccessory() {
    bidib.setAccessory(5, 1); // Set accessory 5 to aspect 1 (e.g., "thrown")

    // Expected message: MAGIC | LEN | ADDR | MSG_NUM | MSG_ACCESSORY_SET | ANUM | ASPECT | CRC | MAGIC
    // LEN = 5, ADDR = 0, MSG_NUM = 0, ANUM = 5, ASPECT = 1
    uint8_t expected_msg[] = {0xFE, 0x05, 0x00, 0x00, 0x38, 0x05, 0x01, 0x00, 0xFE};
    uint8_t crc_payload[] = {0x05, 0x00, 0x00, 0x38, 0x05, 0x01};
    expected_msg[7] = bidib.calculateCrc(crc_payload, sizeof(crc_payload));

    uint8_t actual_msg[sizeof(expected_msg)];
    mockSerial.read_outgoing(actual_msg, sizeof(actual_msg));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_msg, actual_msg, sizeof(expected_msg));
}

void test_getAccessory() {
    bidib.getAccessory(10); // Get state of accessory 10

    // Expected message: MAGIC | LEN | ADDR | MSG_NUM | MSG_ACCESSORY_GET | ANUM | CRC | MAGIC
    // LEN = 4, ADDR = 0, MSG_NUM = 0, ANUM = 10
    uint8_t expected_msg[] = {0xFE, 0x04, 0x00, 0x00, 0x39, 0x0A, 0x00, 0xFE};
    uint8_t crc_payload[] = {0x04, 0x00, 0x00, 0x39, 0x0A};
    uint8_t crc = bidib.calculateCrc(crc_payload, sizeof(crc_payload));

    if (crc == 0xFE || crc == 0xFD) {
        uint8_t expected_msg_escaped[] = {0xFE, 0x04, 0x00, 0x00, 0x39, 0x0A, 0xFD, (uint8_t)(crc ^ 0x20), 0xFE};
        uint8_t actual_msg[sizeof(expected_msg_escaped)];
        mockSerial.read_outgoing(actual_msg, sizeof(actual_msg));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_msg_escaped, actual_msg, sizeof(expected_msg_escaped));
    } else {
        expected_msg[6] = crc;
        uint8_t actual_msg[sizeof(expected_msg)];
        mockSerial.read_outgoing(actual_msg, sizeof(actual_msg));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_msg, actual_msg, sizeof(expected_msg));
    }
}

void test_handleAccessoryState() {
    // Simulate receiving MSG_ACCESSORY_STATE
    // LEN = 5, ADDR = 0, MSG_NUM = 0, MSG_ACCESSORY_STATE, ANUM=7, ASPECT=0
    uint8_t crc_payload[] = {0x05, 0x00, 0x00, 0xB8, 0x07, 0x00};
    uint8_t crc = bidib.calculateCrc(crc_payload, sizeof(crc_payload));
    uint8_t incoming_msg[] = {0xFE, 0x05, 0x00, 0x00, 0xB8, 0x07, 0x00, crc, 0xFE};

    mockSerial.addIncoming(incoming_msg, sizeof(incoming_msg));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(accessoryStateCallbackCalled);
    TEST_ASSERT_EQUAL(7, callbackAccessoryNum);
    TEST_ASSERT_EQUAL(0, callbackAspect);
}

void test_handleAccessoryNotify() {
    // Simulate receiving MSG_ACCESSORY_NOTIFY
    // LEN = 5, ADDR = 0, MSG_NUM = 0, MSG_ACCESSORY_NOTIFY, ANUM=12, ASPECT=1
    uint8_t crc_payload[] = {0x05, 0x00, 0x00, 0xB9, 0x0C, 0x01};
    uint8_t crc = bidib.calculateCrc(crc_payload, sizeof(crc_payload));
    uint8_t incoming_msg[] = {0xFE, 0x05, 0x00, 0x00, 0xB9, 0x0C, 0x01, crc, 0xFE};

    mockSerial.addIncoming(incoming_msg, sizeof(incoming_msg));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(accessoryStateCallbackCalled);
    TEST_ASSERT_EQUAL(12, callbackAccessoryNum);
    TEST_ASSERT_EQUAL(1, callbackAspect);
}


int main() {
    UNITY_BEGIN();
    RUN_TEST(test_setAccessory);
    RUN_TEST(test_getAccessory);
    RUN_TEST(test_handleAccessoryState);
    RUN_TEST(test_handleAccessoryNotify);
    UNITY_END();
    return 0;
}
