#include <ArduinoFake.h>
#include <unity.h>
#include "BiDiB.h"
#include "mock_stream.h"

using namespace fakeit;

// Mock-Stream für die Simulation der seriellen Kommunikation
MockStream mockSerial;

// Zähler für gesendete Nachrichten
int sendMessageCount = 0;
BiDiBMessage lastSentMessage;

// Eine abgeleitete Test-Klasse, um auf geschützte Member zuzugreifen und sendMessage zu überschreiben
class BiDiBTest : public BiDiB {
public:
    // Überschreiben von sendMessage, um ausgehende Nachrichten abzufangen
    void sendMessage(const BiDiBMessage &msg) override {
        sendMessageCount++;
        lastSentMessage = msg;
    }

    // Zugriff auf die interne Verarbeitung von eingehenden Nachrichten
    void processIncoming(const uint8_t* data, size_t size) {
        mockSerial.addIncoming(data, size);
        update(); // Liest die Daten ein
        handleMessages(); // Verarbeitet die eingelesene Nachricht
    }
};

BiDiBTest bidib;

void setUp(void) {
    ArduinoFakeReset();
    mockSerial.clear();
    sendMessageCount = 0;

    // Beginne bei einer Startzeit von 10000ms
    When(Method(ArduinoFake(), millis)).Return(10000);

    bidib.begin(mockSerial);
    // Secure-ACK Feature für die Tests aktivieren
    bidib.setFeature(FEATURE_BM_SECACK_AVAILABLE, 1);
    bidib.setFeature(FEATURE_BM_SECACK_ON, 1);
}

void tearDown(void) {
    // Aufräumen nach jedem Test
}

// Test 1: Erfolgreicher Sende- und Bestätigungszyklus für MSG_BM_OCC
void test_secure_ack_occurs_and_is_confirmed(void) {
    // 1. Sende eine Belegtmeldung
    bidib.sendOccupancySingle(5, true);

    // 2. Überprüfe, ob die MSG_BM_OCC gesendet wurde
    TEST_ASSERT_EQUAL(1, sendMessageCount);
    TEST_ASSERT_EQUAL(MSG_BM_OCC, lastSentMessage.msg_type);
    TEST_ASSERT_EQUAL(5, lastSentMessage.data[0]);

    // 3. Simuliere den Empfang der korrekten MIRROR-Nachricht
    // Manuelle Berechnung des CRC: {0x04, 0x00, 0x00, 0xA8, 0x05} -> CRC = 0x0C
    uint8_t mirror_msg[] = {BIDIB_MAGIC, 0x04, 0x00, 0x00, MSG_BM_MIRROR_OCC, 0x05, 0x0C, BIDIB_MAGIC};
    bidib.processIncoming(mirror_msg, sizeof(mirror_msg));

    // 4. Simuliere einen Timeout - es sollte keine neue Nachricht gesendet werden
    sendMessageCount = 0;
    When(Method(ArduinoFake(), millis)).Return(10000 + SECURE_ACK_TIMEOUT + 100);
    bidib.update(); // Sollte Timeout-Logik auslösen

    TEST_ASSERT_EQUAL(0, sendMessageCount); // Es sollte keine erneute Sendung stattfinden
}

// Test 2: Timeout führt zu erneutem Senden
void test_secure_ack_timeout_triggers_resend(void) {
    // 1. Sende eine Belegtmeldung
    bidib.sendOccupancySingle(8, false); // MSG_BM_FREE

    // 2. Überprüfe, ob die erste Nachricht gesendet wurde
    TEST_ASSERT_EQUAL(1, sendMessageCount);
    TEST_ASSERT_EQUAL(MSG_BM_FREE, lastSentMessage.msg_type);
    TEST_ASSERT_EQUAL(8, lastSentMessage.data[0]);

    // 3. Simuliere einen Timeout
    sendMessageCount = 0;
    When(Method(ArduinoFake(), millis)).Return(10000 + SECURE_ACK_TIMEOUT + 100);
    bidib.update(); // Sollte Timeout-Logik auslösen

    // 4. Überprüfe, ob die Nachricht erneut gesendet wurde
    TEST_ASSERT_EQUAL(1, sendMessageCount);
    TEST_ASSERT_EQUAL(MSG_BM_FREE, lastSentMessage.msg_type);
    TEST_ASSERT_EQUAL(8, lastSentMessage.data[0]);
}

// Test 3: Mehrfache Timeouts und endgültiges Aufgeben
void test_secure_ack_gives_up_after_max_retries(void) {
    bidib.sendOccupancySingle(10, true);
    TEST_ASSERT_EQUAL(1, sendMessageCount);

    // Simuliere (SECURE_ACK_RETRIES) Timeouts
    for (int i = 0; i < SECURE_ACK_RETRIES; ++i) {
        sendMessageCount = 0;
        When(Method(ArduinoFake(), millis)).Return(10000 + (SECURE_ACK_TIMEOUT + 100) * (i + 1));
        bidib.update();
        TEST_ASSERT_EQUAL(1, sendMessageCount); // Erneutes Senden
    }

    // Simuliere einen weiteren Timeout
    sendMessageCount = 0;
    When(Method(ArduinoFake(), millis)).Return(10000 + (SECURE_ACK_TIMEOUT + 100) * (SECURE_ACK_RETRIES + 1));
    bidib.update();

    // Jetzt sollte nicht mehr gesendet werden
    TEST_ASSERT_EQUAL(0, sendMessageCount);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_secure_ack_occurs_and_is_confirmed);
    RUN_TEST(test_secure_ack_timeout_triggers_resend);
    RUN_TEST(test_secure_ack_gives_up_after_max_retries);
    UNITY_END();
    return 0;
}
