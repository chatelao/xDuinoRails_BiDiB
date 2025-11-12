#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

// =============================================================================
// Mock BiDiB class for testing protected/private methods
// =============================================================================
class TestableBiDiB : public BiDiB {
public:
    // Expose protected members for testing
    using BiDiB::_lastMessage;

    void sendMessage(const BiDiBMessage& msg) override {
        // Intercept sent messages and store them for inspection
        _lastSentMessage = msg;
    }

    BiDiBMessage getLastSentMessage() {
        return _lastSentMessage;
    }

private:
    BiDiBMessage _lastSentMessage;
};

// =============================================================================
// Test Globals
// =============================================================================
TestableBiDiB bidib;
MockStream mockSerial;
bool pomAckReceived = false;
uint16_t receivedPomAddress = 0;
uint8_t receivedPomStatus = 0;

// =============================================================================
// Test Callbacks
// =============================================================================
void pomAckCallback(uint16_t address, uint8_t status) {
    pomAckReceived = true;
    receivedPomAddress = address;
    receivedPomStatus = status;
}

// =============================================================================
// Test Fixture
// =============================================================================
void setUp(void) {
    pomAckReceived = false;
    receivedPomAddress = 0;
    receivedPomStatus = 0;
    bidib.begin(mockSerial);
    bidib.onPomAck(pomAckCallback);
}

void tearDown(void) {
    // clean up after each test
}

// =============================================================================
// Unit Tests
// =============================================================================

void test_pomWriteByte_message_structure() {
    bidib.pomWriteByte(1234, 56, 78);
    BiDiBMessage sentMsg = bidib.getLastSentMessage();

    TEST_ASSERT_EQUAL(MSG_CS_POM, sentMsg.msg_type);
    TEST_ASSERT_EQUAL(15, sentMsg.length);
    TEST_ASSERT_EQUAL(0, sentMsg.address[0]);
    TEST_ASSERT_EQUAL(1234 & 0xFF, sentMsg.data[0]);
    TEST_ASSERT_EQUAL((1234 >> 8) & 0xFF, sentMsg.data[1]);
    TEST_ASSERT_EQUAL(BIDIB_CS_POM_WR_BYTE, sentMsg.data[5]);
    TEST_ASSERT_EQUAL(55, sentMsg.data[6]); // CV is 1-based, so 56-1=55
    TEST_ASSERT_EQUAL(0, sentMsg.data[7]);
    TEST_ASSERT_EQUAL(78, sentMsg.data[9]);
}

// =============================================================================
// End-to-End (TX/RX) Tests
// =============================================================================

void test_receive_pom_ack() {
    // Simulate receiving a POM ACK message
    // MAGIC | LEN=9 | ADDR=0 | NUM=0 | TYPE | ADDR_L=D2 | ADDR_H=04 | ... | ACK=1 | CRC | MAGIC
    uint8_t ack_msg[] = {BIDIB_MAGIC, 9, 0, 0, MSG_CS_POM_ACK, 0xD2, 0x04, 0, 0, 0, 1, 0, BIDIB_MAGIC};
    // Calculate CRC over the message content (from length byte)
    uint8_t crc = bidib.calculateCrc(ack_msg + 1, 10);
    ack_msg[11] = crc;

    mockSerial.addIncoming(ack_msg, sizeof(ack_msg));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(pomAckReceived);
    TEST_ASSERT_EQUAL(1234, receivedPomAddress);
    TEST_ASSERT_EQUAL(1, receivedPomStatus);
}


// =============================================================================
// Main
// =============================================================================
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pomWriteByte_message_structure);
    RUN_TEST(test_receive_pom_ack);
    UNITY_END();
    return 0;
}
