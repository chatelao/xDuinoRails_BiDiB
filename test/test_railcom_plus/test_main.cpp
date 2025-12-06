#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"
#include "crc8.h"

using namespace fakeit;

// Global variables for verification
bool rcPlusCallbackCalled = false;
uint8_t lastOpcode = 0;
uint8_t lastData[64];
uint8_t lastDataLen = 0;

void testRcPlusCallback(uint8_t opcode, const uint8_t* data, uint8_t len) {
    rcPlusCallbackCalled = true;
    lastOpcode = opcode;
    lastDataLen = len;
    if (len > 0 && len <= 64) {
        memcpy(lastData, data, len);
    }
}

void setUp(void) {
    ArduinoFakeReset();
    rcPlusCallbackCalled = false;
    lastOpcode = 0;
    lastDataLen = 0;
    memset(lastData, 0, sizeof(lastData));
}

void tearDown(void) {}

void test_handle_msg_cs_rcplus() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);
    bidib.onRcPlus(testRcPlusCallback);

    // MSG_CS_RCPLUS: Length, Addr(0), MsgNum, Type, Opcode, Data...
    // Let's simulate a RC_BIND command: Opcode=0x05, Data=0xAA 0xBB (dummy)
    uint8_t msg_data[] = { 0xAA, 0xBB };
    uint8_t opcode = 0x05; // BIDIB_CS_RCPLUS_BIND

    // Construct message
    // Length = 1(addr) + 1(num) + 1(type) + 1(opcode) + 2(data) = 6
    uint8_t msg_core[] = { 0x06, 0x00, 0x01, MSG_CS_RCPLUS, opcode, msg_data[0], msg_data[1] };

    uint8_t crc = bidib.calculateCrc(msg_core, sizeof(msg_core));
    uint8_t msg_payload[sizeof(msg_core) + 3];
    msg_payload[0] = BIDIB_MAGIC;
    memcpy(&msg_payload[1], msg_core, sizeof(msg_core));
    msg_payload[sizeof(msg_core) + 1] = crc;
    msg_payload[sizeof(msg_core) + 2] = BIDIB_MAGIC;

    mockSerial.addIncoming(msg_payload, sizeof(msg_payload));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(rcPlusCallbackCalled);
    TEST_ASSERT_EQUAL(opcode, lastOpcode);
    TEST_ASSERT_EQUAL(2, lastDataLen);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_data, lastData, 2);
}

void test_send_rc_plus_ack() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);

    uint8_t opcode = 0x05; // BIDIB_CS_RCPLUS_ACK_BIND
    uint8_t data[] = { 0x11, 0x22 };
    bidib.sendRcPlusAck(opcode, data, 2);

    // Verify output
    // Length = 4 + 2 = 6.
    // Bytes: MAGIC, Length(6), Addr(0), MsgNum(0), MsgType(0xE8), Opcode(0x05), Data(0x11, 0x22), CRC, MAGIC

    uint8_t buffer[64];
    mockSerial.read_outgoing(buffer, 64);

    TEST_ASSERT_EQUAL(BIDIB_MAGIC, buffer[0]);
    TEST_ASSERT_EQUAL(0x06, buffer[1]); // Length
    TEST_ASSERT_EQUAL(0x00, buffer[2]); // Address
    TEST_ASSERT_EQUAL(0x00, buffer[3]); // MsgNum
    TEST_ASSERT_EQUAL(MSG_CS_RCPLUS_ACK, buffer[4]); // Type
    TEST_ASSERT_EQUAL(opcode, buffer[5]); // Opcode
    TEST_ASSERT_EQUAL(0x11, buffer[6]);
    TEST_ASSERT_EQUAL(0x22, buffer[7]);

    // Check CRC
    uint8_t expected_core[] = { 0x06, 0x00, 0x00, MSG_CS_RCPLUS_ACK, opcode, 0x11, 0x22 };
    uint8_t expected_crc = bidib.calculateCrc(expected_core, sizeof(expected_core));

    TEST_ASSERT_EQUAL(expected_crc, buffer[8]);
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, buffer[9]);
}

void test_send_bm_rc_plus() {
    MockStream mockSerial;
    BiDiB bidib;
    bidib.begin(mockSerial);

    uint8_t data[] = { 0x33, 0x44, 0x55 };
    bidib.sendBmRcPlus(data, 3);

    // Verify output
    // Length = 3 + 3 = 6.
    // Bytes: MAGIC, Length(6), Addr(0), MsgNum(0), MsgType(0xAB), Data(0x33...), CRC, MAGIC

    uint8_t buffer[64];
    mockSerial.read_outgoing(buffer, 64);

    TEST_ASSERT_EQUAL(BIDIB_MAGIC, buffer[0]);
    TEST_ASSERT_EQUAL(0x06, buffer[1]); // Length
    TEST_ASSERT_EQUAL(0x00, buffer[2]); // Address
    TEST_ASSERT_EQUAL(0x00, buffer[3]); // MsgNum
    TEST_ASSERT_EQUAL(MSG_BM_RCPLUS, buffer[4]); // Type
    TEST_ASSERT_EQUAL(0x33, buffer[5]);
    TEST_ASSERT_EQUAL(0x44, buffer[6]);
    TEST_ASSERT_EQUAL(0x55, buffer[7]);

    // Check CRC
    uint8_t expected_core[] = { 0x06, 0x00, 0x00, MSG_BM_RCPLUS, 0x33, 0x44, 0x55 };
    uint8_t expected_crc = bidib.calculateCrc(expected_core, sizeof(expected_core));

    TEST_ASSERT_EQUAL(expected_crc, buffer[8]);
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, buffer[9]);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_handle_msg_cs_rcplus);
    RUN_TEST(test_send_rc_plus_ack);
    RUN_TEST(test_send_bm_rc_plus);
    UNITY_END();
    return 0;
}
