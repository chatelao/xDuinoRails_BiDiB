#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

MockStream mockStream;
BiDiB bidib;

// Placeholder for booster status callback
uint8_t received_booster_status = 0xFF;
void boosterStatusCallback(uint8_t status) {
    received_booster_status = status;
}

// Placeholder for booster diagnostic callback
uint8_t received_diagnostic_type = 0xFF;
uint16_t received_diagnostic_value = 0xFFFF;
void boosterDiagnosticCallback(uint8_t type, uint16_t value) {
    received_diagnostic_type = type;
    received_diagnostic_value = value;
}

void setUp(void) {
    ArduinoFakeReset();
    mockStream.clear();
    bidib.begin(mockStream);
    received_booster_status = 0xFF;
    received_diagnostic_type = 0xFF;
    received_diagnostic_value = 0xFFFF;
    bidib.onBoosterStatus(boosterStatusCallback);
    bidib.onBoosterDiagnostic(boosterDiagnosticCallback);
}

void tearDown(void) {}

// =================================================================================
// Test Cases
// =================================================================================

void test_setBoosterState_On_Broadcast() {
    bidib.setBoosterState(true, 0);
    uint8_t payload[] = { 0x03, 0x00, 0x00, MSG_BOOST_ON };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t expected[] = { BIDIB_MAGIC, 0x03, 0x00, 0x00, MSG_BOOST_ON, crc, BIDIB_MAGIC };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_setBoosterState_Off_Unicast() {
    bidib.setBoosterState(false, 1);
    uint8_t payload[] = { 0x04, 0x01, 0x00, 0x00, MSG_BOOST_OFF };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t expected[] = { BIDIB_MAGIC, 0x04, 0x01, 0x00, 0x00, MSG_BOOST_OFF, crc, BIDIB_MAGIC };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_queryBooster() {
    bidib.queryBooster(1);
    uint8_t payload[] = { 0x04, 0x01, 0x00, 0x00, MSG_BOOST_QUERY };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t expected[] = { BIDIB_MAGIC, 0x04, 0x01, 0x00, 0x00, MSG_BOOST_QUERY, crc, BIDIB_MAGIC };
    uint8_t actual[sizeof(expected)];
    mockStream.read_outgoing(actual, sizeof(expected));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, sizeof(expected));
}

void test_handleBoosterStatus() {
    uint8_t payload[] = { 0x04, 0x00, 0x00, MSG_BOOST_STAT, BIDIB_BST_STATE_ON_LIMIT };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t incoming[] = { BIDIB_MAGIC, payload[0], payload[1], payload[2], payload[3], payload[4], crc, BIDIB_MAGIC };
    mockStream.addIncoming(incoming, sizeof(incoming));
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL_UINT8(BIDIB_BST_STATE_ON_LIMIT, received_booster_status);
}

void test_handleBoosterDiagnostic() {
    uint16_t current_value = 1500; // 1.5A
    uint8_t payload[] = { 0x06, 0x00, 0x00, MSG_BOOST_DIAGNOSTIC, BIDIB_BST_DIAG_CURRENT, (uint8_t)(current_value & 0xFF), (uint8_t)(current_value >> 8) };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t incoming[] = { BIDIB_MAGIC, payload[0], payload[1], payload[2], payload[3], payload[4], payload[5], payload[6], crc, BIDIB_MAGIC };
    mockStream.addIncoming(incoming, sizeof(incoming));
    bidib.update();
    bidib.handleMessages();
    TEST_ASSERT_EQUAL_UINT8(BIDIB_BST_DIAG_CURRENT, received_diagnostic_type);
    TEST_ASSERT_EQUAL_UINT16(current_value, received_diagnostic_value);
}

void test_handleMultiBoosterDiagnostic() {
    uint16_t current_value = 2100; // 2.1A
    uint16_t voltage_value = 18;   // 18V
    uint8_t payload[] = { 0x08, 0x00, 0x00, MSG_BOOST_DIAGNOSTIC,
                          BIDIB_BST_DIAG_CURRENT, (uint8_t)(current_value & 0xFF), (uint8_t)(current_value >> 8),
                          BIDIB_BST_DIAG_VOLTAGE, (uint8_t)(voltage_value & 0xFF) };
    uint8_t crc = bidib.calculateCrc(payload, sizeof(payload));
    uint8_t incoming[] = { BIDIB_MAGIC, 0x08, 0x00, 0x00, MSG_BOOST_DIAGNOSTIC,
                           payload[4], payload[5], payload[6], payload[7], payload[8], crc, BIDIB_MAGIC };

    mockStream.addIncoming(incoming, sizeof(incoming));
    bidib.update();
    bidib.handleMessages();

    // Note: This test only verifies the LAST diagnostic value received.
    // A more robust test would require storing all received values.
    TEST_ASSERT_EQUAL_UINT8(BIDIB_BST_DIAG_VOLTAGE, received_diagnostic_type);
    TEST_ASSERT_EQUAL_UINT16(voltage_value, received_diagnostic_value);
}


// =================================================================================
// Main
// =================================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_setBoosterState_On_Broadcast);
    RUN_TEST(test_setBoosterState_Off_Unicast);
    RUN_TEST(test_queryBooster);
    RUN_TEST(test_handleBoosterStatus);
    RUN_TEST(test_handleBoosterDiagnostic);
    RUN_TEST(test_handleMultiBoosterDiagnostic);
    UNITY_END();
    return 0;
}
