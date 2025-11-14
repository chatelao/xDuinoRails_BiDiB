#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

BiDiB bidib;
MockStream mockStream;

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib.begin(mockStream);
}

void tearDown(void) {}

void test_crc_calculation(void) {
    uint8_t data[] = {0x0C, 0x01, 0x00, 0x00, 0x10, 0x00, 0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t crc = bidib.calculateCrc(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT8(0x1C, crc);
}

void test_enterFirmwareUpdateMode(void) {
    uint8_t node_addr = 1;
    uint8_t unique_id[7] = {0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    bidib.enterFirmwareUpdateMode(node_addr, unique_id);

    uint8_t expected[] = {
        0xFE, // START
        0x0C, // LENGTH
        0x01, 0x00, // ADDRESS
        0x00, // MSG_NUM
        0x10, // MSG_FW_UPDATE_OP
        0x00, // OP_ENTER
        0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // UID
        0x1C, // CRC
        0xFE  // END
    };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_exitFirmwareUpdateMode(void) {
    bidib.exitFirmwareUpdateMode(1);

    uint8_t expected[] = {
        0xFE, 0x05, 0x01, 0x00, 0x00, 0x10, 0x01, 0x0F, 0xFE
    };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_setFirmwareUpdateDestination(void) {
    bidib.setFirmwareUpdateDestination(1, 0); // Destination: Flash

    uint8_t expected[] = {
        0xFE, 0x06, 0x01, 0x00, 0x00, 0x10, 0x02, 0x00, 0x29, 0xFE
    };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_sendFirmwareData(void) {
    uint8_t data[] = {':', '1', '0', '0', '0', '0', '0', '0', '0'};
    bidib.sendFirmwareData(1, data, sizeof(data));

    uint8_t expected[] = {
        0xFE, 0x0E, 0x01, 0x00, 0x00, 0x10, 0x03,
        ':', '1', '0', '0', '0', '0', '0', '0', '0',
        0x10, 0xFE
    };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_signalFirmwareUpdateDone(void) {
    bidib.signalFirmwareUpdateDone(1);

    uint8_t expected[] = {
        0xFE, 0x05, 0x01, 0x00, 0x00, 0x10, 0x04, 0x30, 0xFE
    };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}
// =================================================================================
// Main
// =================================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_crc_calculation);
    RUN_TEST(test_enterFirmwareUpdateMode);
    RUN_TEST(test_exitFirmwareUpdateMode);
    RUN_TEST(test_setFirmwareUpdateDestination);
    RUN_TEST(test_sendFirmwareData);
    RUN_TEST(test_signalFirmwareUpdateDone);
    UNITY_END();
    return 0;
}
