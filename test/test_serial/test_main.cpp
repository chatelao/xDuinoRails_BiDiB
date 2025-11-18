#include <ArduinoFake.h>
#include <unity.h>

#include "BiDiB_Serial.h"
#include "mock_stream.h"
#include "crc8.h"

using namespace fakeit;

void setUp(void) {
    ArduinoFakeReset();
}

void tearDown(void) {
    // Clean up
}

void testSendMessage_P2P() {
    // Arrange
    MockStream mockSerial;
    BiDiB_Serial bidib;
    bidib.begin(mockSerial);

    BiDiBMessage msg;
    // Length is the number of bytes after the length byte itself:
    // Address(1) + MsgNum(1) + MsgType(1) + Data(1) = 4
    msg.length = 4;
    msg.address[0] = 0; // Broadcast address
    msg.msg_num = 12;
    msg.msg_type = MSG_SYS_GET_MAGIC;
    msg.data[0] = 0xAF;

    // The CRC is calculated over the entire message content starting with the length byte
    uint8_t content_for_crc[] = { msg.length, msg.address[0], msg.msg_num, msg.msg_type, msg.data[0] };
    uint8_t expected_crc = bidib.calculateCrc(content_for_crc, sizeof(content_for_crc));

    uint8_t expected_output[] = {
        BIDIB_MAGIC,
        msg.length,
        msg.address[0],
        msg.msg_num,
        msg.msg_type,
        msg.data[0],
        expected_crc,
        BIDIB_MAGIC
    };

    // Act
    bidib.sendMessage(msg);

    // Assert
    TEST_ASSERT_EQUAL_UINT8(sizeof(expected_output), mockSerial.available_outgoing());
    uint8_t actual_output[sizeof(expected_output)];
    mockSerial.read_outgoing(actual_output, sizeof(actual_output));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_output, actual_output, sizeof(expected_output));
}

void testReceiveMessage_P2P() {
    // Arrange
    MockStream mockSerial;
    BiDiB_Serial bidib;
    bidib.begin(mockSerial);

    uint8_t msg_length = 4;
    uint8_t msg_num = 1;
    uint8_t msg_type = MSG_SYS_MAGIC;
    uint8_t msg_data = 0xAF;

    uint8_t content_for_crc[] = { msg_length, 0x00, msg_num, msg_type, msg_data };
    uint8_t crc = bidib.calculateCrc(content_for_crc, sizeof(content_for_crc));
    uint8_t incoming_msg[] = { BIDIB_MAGIC, msg_length, 0x00, msg_num, msg_type, msg_data, crc, BIDIB_MAGIC };

    // Act
    mockSerial.addIncoming(incoming_msg, sizeof(incoming_msg));
    bidib.update();

    // Assert
    TEST_ASSERT_TRUE(bidib.messageAvailable());
    BiDiBMessage received_msg = bidib.getLastMessage();
    TEST_ASSERT_EQUAL_UINT8(msg_length, received_msg.length);
    TEST_ASSERT_EQUAL_UINT8(0, received_msg.address[0]);
    TEST_ASSERT_EQUAL_UINT8(msg_num, received_msg.msg_num);
    TEST_ASSERT_EQUAL_UINT8(msg_type, received_msg.msg_type);
    TEST_ASSERT_EQUAL_UINT8(msg_data, received_msg.data[0]);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(testSendMessage_P2P);
    RUN_TEST(testReceiveMessage_P2P);
    return UNITY_END();
}
