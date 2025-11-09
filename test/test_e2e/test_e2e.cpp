#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include <string>
#include <vector>

// Mock Stream class to simulate a connection between two BiDiB instances
class MockStream : public Stream {
public:
    std::vector<uint8_t> buffer;
    size_t read_pos = 0;

    virtual int available() { return buffer.size() - read_pos; }
    virtual int read() { return available() > 0 ? buffer[read_pos++] : -1; }
    virtual int peek() { return available() > 0 ? buffer[read_pos] : -1; }
    virtual void flush() {}
    virtual size_t write(uint8_t c) {
        buffer.push_back(c);
        return 1;
    }

    // Custom method for this test to reset the stream and prepare for a new transfer
    void reset() {
        buffer.clear();
        read_pos = 0;
    }
};

BiDiB sender;
BiDiB receiver;
MockStream mockConnection;

void setUp(void) {
    mockConnection.reset();
    sender.begin(mockConnection);
    receiver.begin(mockConnection);
}

void tearDown(void) {}

void test_e2e_simple_message_transfer(void) {
    BiDiBMessage sent_msg;
    sent_msg.length = 5; // 1 (addr) + 2 (data) + 2 (num, type)
    sent_msg.address[0] = 0;
    sent_msg.msg_num = 123;
    sent_msg.msg_type = 45;
    sent_msg.data[0] = 67;
    sent_msg.data[1] = 89;

    sender.sendMessage(sent_msg);

    // The receiver needs to be updated to process the incoming bytes
    // In a real scenario, this would happen in the main loop
    while(mockConnection.available()) {
        receiver.update();
    }

    TEST_ASSERT_TRUE(receiver.messageAvailable());

    BiDiBMessage received_msg = receiver.getLastMessage();

    TEST_ASSERT_EQUAL(sent_msg.length, received_msg.length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sent_msg.address, received_msg.address, 1);
    TEST_ASSERT_EQUAL(sent_msg.msg_num, received_msg.msg_num);
    TEST_ASSERT_EQUAL(sent_msg.msg_type, received_msg.msg_type);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sent_msg.data, received_msg.data, 2);
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(test_e2e_simple_message_transfer);
    UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
    delay(2000);
    runTests();
}
void loop() {}
#else
int main(int argc, char **argv) {
    runTests();
    return 0;
}
#endif
