#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include <vector>
#include <string>

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

// Helper function to compare two vectors of uint8_t
void ASSERT_EQUAL_VECTOR(const std::vector<uint8_t>& expected, const std::vector<uint8_t>& actual, const char* message) {
    TEST_ASSERT_EQUAL_size_t_MESSAGE(expected.size(), actual.size(), message);
    for (size_t i = 0; i < expected.size(); ++i) {
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i], actual[i], (std::string(message) + " - Mismatch at index " + std::to_string(i)).c_str());
    }
}

void construct_message_stream(const std::vector<uint8_t>& content, std::vector<uint8_t>& stream) {
    stream.push_back(BIDIB_MAGIC);
    for(uint8_t byte : content) {
        stream.push_back(byte);
    }
    stream.push_back(calculate_expected_crc(content));
    stream.push_back(BIDIB_MAGIC);
}

void setUp(void) {
    mockSerial.clear();
    bidib.begin(mockSerial);
}

void tearDown(void) {}

void test_handle_get_magic(void) {
    std::vector<uint8_t> content = {0x03, 0x00, 0x01, 0x01};
    construct_message_stream(content, mockSerial.input_buffer);

    bidib.update();
    bidib.handleMessages();

    std::vector<uint8_t> expected_content = {0x04, 0x00, 0x01, 0x81, 0xAF};
    std::vector<uint8_t> expected_output;
    construct_message_stream(expected_content, expected_output);

    ASSERT_EQUAL_VECTOR(expected_output, mockSerial.output_buffer, "test_handle_get_magic");
}

void test_handle_get_p_version(void) {
    std::vector<uint8_t> content = {0x03, 0x00, 0x02, 0x02};
    construct_message_stream(content, mockSerial.input_buffer);

    bidib.update();
    bidib.handleMessages();

    std::vector<uint8_t> expected_content = {0x05, 0x00, 0x02, 0x82, 0x01, 0x00};
    std::vector<uint8_t> expected_output;
    construct_message_stream(expected_content, expected_output);

    ASSERT_EQUAL_VECTOR(expected_output, mockSerial.output_buffer, "test_handle_get_p_version");
}

void test_handle_get_unique_id(void) {
    std::vector<uint8_t> content = {0x03, 0x00, 0x03, 0x03};
    construct_message_stream(content, mockSerial.input_buffer);

    bidib.update();
    bidib.handleMessages();

    std::vector<uint8_t> expected_content = {0x0A, 0x00, 0x03, 0x83, 0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    std::vector<uint8_t> expected_output;
    construct_message_stream(expected_content, expected_output);

    ASSERT_EQUAL_VECTOR(expected_output, mockSerial.output_buffer, "test_handle_get_unique_id");
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(test_handle_get_magic);
    RUN_TEST(test_handle_get_p_version);
    RUN_TEST(test_handle_get_unique_id);
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
