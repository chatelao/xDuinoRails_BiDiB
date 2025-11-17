#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include <string>
#include <vector>
#include <numeric>

// Mock Stream class to capture output
class MockStream : public Stream {
public:
    std::vector<uint8_t> output_buffer;
    std::vector<uint8_t> input_buffer;
    size_t read_pos = 0;

    virtual int available() { return input_buffer.size() - read_pos; }
    virtual int read() { return available() > 0 ? input_buffer[read_pos++] : -1; }
    virtual int peek() { return available() > 0 ? input_buffer[read_pos] : -1; }
    virtual void flush() {}
    virtual size_t write(uint8_t c) {
        output_buffer.push_back(c);
        return 1;
    }

    void clear() {
        output_buffer.clear();
        input_buffer.clear();
        read_pos = 0;
    }
};

BiDiB bidib;
MockStream mockSerial;

// --- CRC Calculation Logic (copied from BiDiB.cpp for test accuracy) ---
#include "crc8.h"

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

// Helper function to compare two vectors of uint8_t
void ASSERT_EQUAL_VECTOR(const std::vector<uint8_t>& expected, const std::vector<uint8_t>& actual, const char* message) {
    TEST_ASSERT_EQUAL_size_t_MESSAGE(expected.size(), actual.size(), message);
    for (size_t i = 0; i < expected.size(); ++i) {
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i], actual[i], (std::string(message) + " - Mismatch at index " + std::to_string(i)).c_str());
    }
}

void setUp(void) {
    mockSerial.clear();
    bidib.begin(mockSerial);
}

void tearDown(void) {}

void test_send_simple_message(void) {
    BiDiBMessage msg;
    msg.length = 4; // 1 (addr) + 1 (data) + 2 (num, type)
    msg.address[0] = 0;
    msg.msg_num = 10;
    msg.msg_type = 20;
    msg.data[0] = 30;

    bidib.sendMessage(msg);

    std::vector<uint8_t> crc_data = {4, 0, 10, 20, 30};
    uint8_t expected_crc = calculate_expected_crc(crc_data);

    std::vector<uint8_t> expected_output = { 0xFE, 4, 0, 10, 20, 30, expected_crc, 0xFE };

    ASSERT_EQUAL_VECTOR(expected_output, mockSerial.output_buffer, "test_send_simple_message (broadcast)");
}

void test_send_message_with_escaping(void) {
    BiDiBMessage msg;
    msg.length = 5; // 2 (addr) + 1 (data) + 2 (num, type)
    msg.address[0] = 1;
    msg.address[1] = 0;
    msg.msg_num = 0xFE; // This needs escaping
    msg.msg_type = 20;
    msg.data[0] = 0xFD; // This needs escaping

    bidib.sendMessage(msg);

    std::vector<uint8_t> crc_data = {5, 1, 0, 0xFE, 20, 0xFD};
    uint8_t expected_crc = calculate_expected_crc(crc_data);

    std::vector<uint8_t> expected_output = {
        0xFE, 5, 1, 0,
        0xFD, (uint8_t)(0xFE ^ 0x20),
        20,
        0xFD, (uint8_t)(0xFD ^ 0x20),
        expected_crc,
        0xFE
    };

    ASSERT_EQUAL_VECTOR(expected_output, mockSerial.output_buffer, "test_send_message_with_escaping");
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(test_send_simple_message);
    RUN_TEST(test_send_message_with_escaping);
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
