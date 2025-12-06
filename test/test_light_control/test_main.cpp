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

uint8_t lcStatType, lcStatNum, lcStatState;
bool lcStatReceived = false;

uint8_t lcConfigXType, lcConfigXNum, lcConfigXEnum, lcConfigXVal;
bool lcConfigXReceived = false;

uint8_t lcWaitType, lcWaitNum, lcWaitTime;
bool lcWaitReceived = false;

//================================================================================
// Mocks
//================================================================================

void lcStatCallback(uint8_t portType, uint8_t portNum, uint8_t state) {
    lcStatType = portType;
    lcStatNum = portNum;
    lcStatState = state;
    lcStatReceived = true;
}

void lcConfigXCallback(uint8_t portType, uint8_t portNum, uint8_t p_enum, uint8_t p_val) {
    lcConfigXType = portType;
    lcConfigXNum = portNum;
    lcConfigXEnum = p_enum;
    lcConfigXVal = p_val;
    lcConfigXReceived = true;
}

void lcWaitCallback(uint8_t portType, uint8_t portNum, uint8_t time) {
    lcWaitType = portType;
    lcWaitNum = portNum;
    lcWaitTime = time;
    lcWaitReceived = true;
}

//================================================================================
// Setup and Teardown
//================================================================================

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib = BiDiB(); // Reset state
    bidib.begin(mockStream);

    lcStatReceived = false;
    lcConfigXReceived = false;
    lcWaitReceived = false;

    bidib.onLcStat(lcStatCallback);
    bidib.onLcConfigX(lcConfigXCallback);
    bidib.onLcWait(lcWaitCallback);
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

void test_setLcOutput() {
    bidib.setLcOutput(BIDIB_PORTTYPE_SWITCH, 5, 1);

    // MSG_LC_OUTPUT = 0x40
    // Payload: Type (0), Num (5), State (1)
    const uint8_t payload[] = { 0x06, 0x00, 0x00, MSG_LC_OUTPUT, 0x00, 0x05, 0x01 };
    uint8_t expected[sizeof(payload) + 3]; // MAGIC, CRC, MAGIC
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_setLcConfigX() {
    bidib.setLcConfigX(BIDIB_PORTTYPE_LIGHT, 10, BIDIB_PCFG_DIMM_UP, 50);

    // MSG_LC_CONFIGX_SET = 0x46
    // Payload: Type (1), Num (10), Enum (3), Val (50)
    const uint8_t payload[] = { 0x07, 0x00, 0x00, MSG_LC_CONFIGX_SET, 0x01, 0x0A, 0x03, 50 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_getLcConfigX() {
    bidib.getLcConfigX(BIDIB_PORTTYPE_SERVO, 2);

    // MSG_LC_CONFIGX_GET = 0x47
    // Payload: Type (2), Num (2)
    const uint8_t payload[] = { 0x05, 0x00, 0x00, MSG_LC_CONFIGX_GET, 0x02, 0x02 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_handleLcStat() {
    // MSG_LC_STAT = 0xC0
    // Payload: Type (0), Num (5), State (1)
    const uint8_t payload[] = { 0x06, 0x00, 0x00, MSG_LC_STAT, 0x00, 0x05, 0x01 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(lcStatReceived);
    TEST_ASSERT_EQUAL(BIDIB_PORTTYPE_SWITCH, lcStatType);
    TEST_ASSERT_EQUAL(5, lcStatNum);
    TEST_ASSERT_EQUAL(1, lcStatState);
}

void test_handleLcWait() {
    // MSG_LC_WAIT = 0xC4
    // Payload: Type (1), Num (3), Time (100)
    const uint8_t payload[] = { 0x06, 0x00, 0x00, MSG_LC_WAIT, 0x01, 0x03, 100 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(lcWaitReceived);
    TEST_ASSERT_EQUAL(BIDIB_PORTTYPE_LIGHT, lcWaitType);
    TEST_ASSERT_EQUAL(3, lcWaitNum);
    TEST_ASSERT_EQUAL(100, lcWaitTime);
}

void test_handleLcConfigX() {
    // MSG_LC_CONFIGX = 0xC6
    // Payload: Type (2), Num (4), Enum (7), Val (10)
    const uint8_t payload[] = { 0x07, 0x00, 0x00, MSG_LC_CONFIGX, 0x02, 0x04, 0x07, 10 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(lcConfigXReceived);
    TEST_ASSERT_EQUAL(BIDIB_PORTTYPE_SERVO, lcConfigXType);
    TEST_ASSERT_EQUAL(4, lcConfigXNum);
    TEST_ASSERT_EQUAL(7, lcConfigXEnum);
    TEST_ASSERT_EQUAL(10, lcConfigXVal);
}

void test_handleLcConfigX_Multiple() {
    // MSG_LC_CONFIGX = 0xC6
    // Payload: Type (2), Num (4), Enum1(7), Val1(10), Enum2(8), Val2(20)
    const uint8_t payload[] = { 0x09, 0x00, 0x00, MSG_LC_CONFIGX, 0x02, 0x04, 0x07, 10, 0x08, 20 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    // Reset flag
    lcConfigXReceived = false;

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(lcConfigXReceived);
    // Since callback overwrites global vars, we only see the last one.
    // Ideally we should use a mock list, but this checks that parsing loop runs.
    TEST_ASSERT_EQUAL(BIDIB_PORTTYPE_SERVO, lcConfigXType);
    TEST_ASSERT_EQUAL(4, lcConfigXNum);
    TEST_ASSERT_EQUAL(8, lcConfigXEnum);
    TEST_ASSERT_EQUAL(20, lcConfigXVal);
}

//================================================================================
// Main
//================================================================================

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_setLcOutput);
    RUN_TEST(test_setLcConfigX);
    RUN_TEST(test_getLcConfigX);
    RUN_TEST(test_handleLcStat);
    RUN_TEST(test_handleLcWait);
    RUN_TEST(test_handleLcConfigX);
    RUN_TEST(test_handleLcConfigX_Multiple);
    UNITY_END();
    return 0;
}
