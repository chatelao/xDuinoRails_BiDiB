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

uint8_t macroStateNum, macroStateVal;
bool macroStateReceived = false;

uint8_t macroStepNum, macroStepIdx, macroStepDelay, macroStepV1, macroStepV2, macroStepV3;
bool macroStepReceived = false;

uint8_t macroParaNum, macroParaIdx;
uint32_t macroParaVal;
bool macroParaReceived = false;

//================================================================================
// Callbacks
//================================================================================

void lcMacroStateCallback(uint8_t macroNum, uint8_t state) {
    macroStateNum = macroNum;
    macroStateVal = state;
    macroStateReceived = true;
}

void lcMacroCallback(uint8_t macroNum, uint8_t stepNum, uint8_t delay, uint8_t val1, uint8_t val2, uint8_t val3) {
    macroStepNum = macroNum;
    macroStepIdx = stepNum;
    macroStepDelay = delay;
    macroStepV1 = val1;
    macroStepV2 = val2;
    macroStepV3 = val3;
    macroStepReceived = true;
}

void lcMacroParaCallback(uint8_t macroNum, uint8_t paramNum, uint32_t value) {
    macroParaNum = macroNum;
    macroParaIdx = paramNum;
    macroParaVal = value;
    macroParaReceived = true;
}

//================================================================================
// Setup and Teardown
//================================================================================

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib = BiDiB(); // Reset state
    bidib.begin(mockStream);

    macroStateReceived = false;
    macroStepReceived = false;
    macroParaReceived = false;

    bidib.onLcMacroState(lcMacroStateCallback);
    bidib.onLcMacro(lcMacroCallback);
    bidib.onLcMacroPara(lcMacroParaCallback);
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

void test_handleMacro_Off() {
    bidib.handleMacro(5, BIDIB_MACRO_OFF);

    // MSG_LC_MACRO_HANDLE = 0x48
    // Payload: MacroNum(5), OpCode(0)
    // Length: 1(Addr) + 1(Num) + 1(Type) + 2(Data) = 5
    const uint8_t payload[] = { 0x05, 0x00, 0x00, MSG_LC_MACRO_HANDLE, 0x05, 0x00 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_setMacroStep() {
    // macro 1, step 2, delay 10, val1=0xAA, val2=0xBB, val3=0xCC
    bidib.setMacroStep(1, 2, 10, 0xAA, 0xBB, 0xCC);

    // MSG_LC_MACRO_SET = 0x49
    // Payload: Mac(1), Step(2), Del(10), AA, BB, CC
    // Length: 1(Addr) + 1(Num) + 1(Type) + 6(Data) = 9
    const uint8_t payload[] = { 0x09, 0x00, 0x00, MSG_LC_MACRO_SET, 0x01, 0x02, 10, 0xAA, 0xBB, 0xCC };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_getMacroStep() {
    bidib.getMacroStep(3, 4);

    // MSG_LC_MACRO_GET = 0x4A
    // Length: 1(Addr) + 1(Num) + 1(Type) + 2(Data) = 5
    const uint8_t payload[] = { 0x05, 0x00, 0x00, MSG_LC_MACRO_GET, 0x03, 0x04 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_setMacroParameter() {
    // macro 2, param 1, value 0x12345678
    bidib.setMacroParameter(2, 1, 0x12345678);

    // MSG_LC_MACRO_PARA_SET = 0x4B
    // Payload: Mac(2), Para(1), Val(78, 56, 34, 12) Little Endian
    // Length: 1(Addr) + 1(Num) + 1(Type) + 6(Data) = 9
    const uint8_t payload[] = { 0x09, 0x00, 0x00, MSG_LC_MACRO_PARA_SET, 0x02, 0x01, 0x78, 0x56, 0x34, 0x12 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_getMacroParameter() {
    bidib.getMacroParameter(5, 2);

    // MSG_LC_MACRO_PARA_GET = 0x4C
    // Length: 1(Addr) + 1(Num) + 1(Type) + 2(Data) = 5
    const uint8_t payload[] = { 0x05, 0x00, 0x00, MSG_LC_MACRO_PARA_GET, 0x05, 0x02 };
    uint8_t expected[sizeof(payload) + 3];
    build_and_escape(expected, payload, sizeof(payload));

    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(actual));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
}

void test_handleLcMacroState() {
    // MSG_LC_MACRO_STATE = 0xC8
    // Payload: Mac(10), State(2)
    // Length: 1(Addr) + 1(Num) + 1(Type) + 2(Data) = 5
    const uint8_t payload[] = { 0x05, 0x00, 0x00, MSG_LC_MACRO_STATE, 10, 2 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(macroStateReceived);
    TEST_ASSERT_EQUAL(10, macroStateNum);
    TEST_ASSERT_EQUAL(2, macroStateVal);
}

void test_handleLcMacro() {
    // MSG_LC_MACRO = 0xC9
    // Payload: Mac(1), Step(0), Del(5), 1, 2, 3
    // Length: 1(Addr) + 1(Num) + 1(Type) + 6(Data) = 9
    const uint8_t payload[] = { 0x09, 0x00, 0x00, MSG_LC_MACRO, 1, 0, 5, 1, 2, 3 };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(macroStepReceived);
    TEST_ASSERT_EQUAL(1, macroStepNum);
    TEST_ASSERT_EQUAL(0, macroStepIdx);
    TEST_ASSERT_EQUAL(5, macroStepDelay);
    TEST_ASSERT_EQUAL(1, macroStepV1);
    TEST_ASSERT_EQUAL(2, macroStepV2);
    TEST_ASSERT_EQUAL(3, macroStepV3);
}

void test_handleLcMacroPara() {
    // MSG_LC_MACRO_PARA = 0xCA
    // Payload: Mac(3), Para(1), Val(0xDEADBEEF) -> EF BE AD DE
    // Length: 1(Addr) + 1(Num) + 1(Type) + 6(Data) = 9
    const uint8_t payload[] = { 0x09, 0x00, 0x00, MSG_LC_MACRO_PARA, 3, 1, 0xEF, 0xBE, 0xAD, 0xDE };
    uint8_t incoming[sizeof(payload) + 3];
    build_and_escape(incoming, payload, sizeof(payload));
    mockStream.addIncoming(incoming, sizeof(incoming));

    bidib.update();
    bidib.handleMessages();

    TEST_ASSERT_TRUE(macroParaReceived);
    TEST_ASSERT_EQUAL(3, macroParaNum);
    TEST_ASSERT_EQUAL(1, macroParaIdx);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, macroParaVal);
}

//================================================================================
// Main
//================================================================================

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_handleMacro_Off);
    RUN_TEST(test_setMacroStep);
    RUN_TEST(test_getMacroStep);
    RUN_TEST(test_setMacroParameter);
    RUN_TEST(test_getMacroParameter);
    RUN_TEST(test_handleLcMacroState);
    RUN_TEST(test_handleLcMacro);
    RUN_TEST(test_handleLcMacroPara);
    UNITY_END();
    return 0;
}
