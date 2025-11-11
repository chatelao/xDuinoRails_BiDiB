#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

// Test-spezifische Klasse, um auf interne Zustände zugreifen zu können
class TestBiDiB : public BiDiB {
public:
    // Expose protected members for testing
    using BiDiB::_lastMessage;
    using BiDiB::_messageAvailable;

    bool getSystemEnabled() { return _system_enabled; }

    void setTestLastMessage(const BiDiBMessage& msg) {
        _lastMessage = msg;
        _messageAvailable = true;
    }
};

TestBiDiB bidib;
MockStream mockSerial;

void setUp(void) {
    bidib.begin(mockSerial);
    mockSerial.clear();
}

void tearDown(void) {
    // clean up after test
}

void test_send_enable(void) {
    bidib.enable();

    // FE 03 00 00 04 05 FE
    TEST_ASSERT_EQUAL(7, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(3, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_SYS_ENABLE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(108, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_send_disable(void) {
    bidib.disable();

    // FE 03 00 00 05 50 FE
    TEST_ASSERT_EQUAL(7, mockSerial.available_outgoing());
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(3, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(0, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(MSG_SYS_DISABLE, mockSerial.read_outgoing());
    TEST_ASSERT_EQUAL(50, mockSerial.read_outgoing()); // CRC
    TEST_ASSERT_EQUAL(BIDIB_MAGIC, mockSerial.read_outgoing());
}

void test_receive_disable(void) {
    TEST_ASSERT_TRUE(bidib.getSystemEnabled());

    BiDiBMessage msg;
    msg.msg_type = MSG_SYS_DISABLE;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();

    TEST_ASSERT_FALSE(bidib.getSystemEnabled());
}

void test_receive_enable(void) {
    // First, disable the system
    BiDiBMessage msg;
    msg.msg_type = MSG_SYS_DISABLE;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();
    TEST_ASSERT_FALSE(bidib.getSystemEnabled());

    // Then, re-enable it
    msg.msg_type = MSG_SYS_ENABLE;
    bidib.setTestLastMessage(msg);
    bidib.handleMessages();
    TEST_ASSERT_TRUE(bidib.getSystemEnabled());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_send_enable);
    RUN_TEST(test_send_disable);
    RUN_TEST(test_receive_disable);
    RUN_TEST(test_receive_enable);
    UNITY_END();
    return 0;
}
