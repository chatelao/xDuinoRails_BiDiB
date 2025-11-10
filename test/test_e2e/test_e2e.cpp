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

// --- CRC Calculation Logic ---
static const uint8_t crc8_table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 234,
    50, 108, 142, 208, 83, 13, 235, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 238, 121, 39, 197, 155,
    244, 170, 72, 22, 87, 9, 239, 181, 41, 119, 149, 203, 14, 80, 176, 232,
    107, 53, 215, 137, 182, 232, 10, 84, 21, 75, 169, 247, 116, 42, 200, 150,
    52, 106, 136, 214, 246, 168, 74, 20, 85, 11, 233, 183, 151, 201, 43, 117
};

void updateCrc(uint8_t byte, uint8_t &crc) {
    crc = crc8_table[crc ^ byte];
}

uint8_t calculate_expected_crc(const std::vector<uint8_t>& data) {
    uint8_t crc = 0;
    for (uint8_t byte : data) {
        updateCrc(byte, crc);
    }
    return crc;
}
// --- End of CRC Calculation Logic ---

void construct_message_stream(const std::vector<uint8_t>& content, std::vector<uint8_t>& stream) {
    stream.push_back(BIDIB_MAGIC);
    for(uint8_t byte : content) {
        stream.push_back(byte);
    }
    stream.push_back(calculate_expected_crc(content));
    stream.push_back(BIDIB_MAGIC);
}

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

    receiver.update();

    TEST_ASSERT_TRUE(receiver.messageAvailable());

    BiDiBMessage received_msg = receiver.getLastMessage();

    TEST_ASSERT_EQUAL(sent_msg.length, received_msg.length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sent_msg.address, received_msg.address, 1);
    TEST_ASSERT_EQUAL(sent_msg.msg_num, received_msg.msg_num);
    TEST_ASSERT_EQUAL(sent_msg.msg_type, received_msg.msg_type);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(sent_msg.data, received_msg.data, 2);
}

void test_e2e_system_message_exchange(void) {
    // Sender asks for the receiver's unique ID
    BiDiBMessage get_uid_msg;
    get_uid_msg.length = 3; // 1 (addr) + 2 (num, type)
    get_uid_msg.address[0] = 0;
    get_uid_msg.msg_num = 10;
    get_uid_msg.msg_type = MSG_SYS_GET_UNIQUE_ID;

    sender.sendMessage(get_uid_msg);

    // The receiver processes the request and sends back the UID
    receiver.update();
    receiver.handleMessages();

    // Now the sender needs to process the response
    sender.update();

    TEST_ASSERT_TRUE(sender.messageAvailable());
    BiDiBMessage uid_response = sender.getLastMessage();

    TEST_ASSERT_EQUAL(MSG_SYS_UNIQUE_ID, uid_response.msg_type);
    TEST_ASSERT_EQUAL(10, uid_response.msg_num); // Should echo the message number
    TEST_ASSERT_EQUAL_UINT8_ARRAY(receiver.unique_id, uid_response.data, 7);
}


void runTests() {
    UNITY_BEGIN();
    RUN_TEST(test_e2e_simple_message_transfer);
    RUN_TEST(test_e2e_system_message_exchange);
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
