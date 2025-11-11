#include <Arduino.h>
#include <unity.h>
#include "BiDiB.h"
#include "ArduinoFake.h"

using namespace fakeit;

// Mock-Container für die gesendeten Nachrichten
std::vector<BiDiBMessage> sentMessages;

// Fake-Implementierung von sendMessage, die Nachrichten in unserem Container speichert
void fakeSendMessage(const BiDiBMessage& msg) {
    sentMessages.push_back(msg);
}

void setUp(void) {
    ArduinoFakeReset();
    sentMessages.clear();
}

void tearDown(void) {}

// Eigene BiDiB-Testklasse, um sendMessage zu "überschreiben" und Nachrichten zu injizieren
class BiDiBTestable : public BiDiB {
public:
    // Diese Methode "überschreibt" die der Basisklasse, da sie virtual ist
    void sendMessage(const BiDiBMessage& msg) override {
        fakeSendMessage(msg);
    }

    // Test-spezifische Methode, um den internen Zustand zu manipulieren
    void injectMessage(const BiDiBMessage& msg) {
        // Da _lastMessage und _messageAvailable private sind, können wir nicht direkt darauf zugreifen.
        // Für einen sauberen Test bräuchte die BiDiB-Klasse `protected`-Member
        // oder eine `friend`-Deklaration für die Testklasse.
        // Als Workaround ändern wir die Sichtbarkeit in der Header-Datei für die Tests.
        // (Dies wird im nächsten Schritt erledigt)
        _lastMessage = msg;
        _messageAvailable = true;
    }
};


void test_handle_feature_getall() {
    BiDiBTestable bidib;

    BiDiBMessage msg;
    msg.msg_type = MSG_FEATURE_GETALL;
    msg.msg_num = 12;

    bidib.injectMessage(msg);
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(1, sentMessages.size());
    BiDiBMessage response = sentMessages[0];
    TEST_ASSERT_EQUAL(MSG_FEATURE_COUNT, response.msg_type);
    TEST_ASSERT_EQUAL(12, response.msg_num);
    TEST_ASSERT_EQUAL(3, response.data[0]); // 3 default features
}

void test_handle_feature_getnext_sequence() {
    BiDiBTestable bidib;

    // 1. Get count
    BiDiBMessage msg_getall;
    msg_getall.msg_type = MSG_FEATURE_GETALL;
    msg_getall.msg_num = 1;
    bidib.injectMessage(msg_getall);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(1, sentMessages.size());
    sentMessages.clear();

    // 2. Get first feature
    BiDiBMessage msg_getnext1;
    msg_getnext1.msg_type = MSG_FEATURE_GETNEXT;
    msg_getnext1.msg_num = 2;
    bidib.injectMessage(msg_getnext1);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(1, sentMessages.size());
    TEST_ASSERT_EQUAL(MSG_FEATURE, sentMessages[0].msg_type);
    TEST_ASSERT_EQUAL(BIDIB_FEATURE_FW_UPDATE_SUPPORT, sentMessages[0].data[0]);
    TEST_ASSERT_EQUAL(1, sentMessages[0].data[1]);
    sentMessages.clear();

    // 3. Get second feature
    BiDiBMessage msg_getnext2;
    msg_getnext2.msg_type = MSG_FEATURE_GETNEXT;
    msg_getnext2.msg_num = 3;
    bidib.injectMessage(msg_getnext2);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(1, sentMessages.size());
    TEST_ASSERT_EQUAL(MSG_FEATURE, sentMessages[0].msg_type);
    TEST_ASSERT_EQUAL(BIDIB_FEATURE_STRING_SIZE, sentMessages[0].data[0]);
    TEST_ASSERT_EQUAL(32, sentMessages[0].data[1]);
    sentMessages.clear();

    // 4. Get third feature
    BiDiBMessage msg_getnext3;
    msg_getnext3.msg_type = MSG_FEATURE_GETNEXT;
    msg_getnext3.msg_num = 4;
    bidib.injectMessage(msg_getnext3);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(1, sentMessages.size());
    TEST_ASSERT_EQUAL(MSG_FEATURE, sentMessages[0].msg_type);
    TEST_ASSERT_EQUAL(BIDIB_FEATURE_MSG_RECEIVE_COUNT, sentMessages[0].data[0]);
    TEST_ASSERT_EQUAL(4, sentMessages[0].data[1]);
    sentMessages.clear();

    // 5. Get end of list
    BiDiBMessage msg_getnext4;
    msg_getnext4.msg_type = MSG_FEATURE_GETNEXT;
    msg_getnext4.msg_num = 5;
    bidib.injectMessage(msg_getnext4);
    bidib.handleMessages();
    TEST_ASSERT_EQUAL(1, sentMessages.size());
    TEST_ASSERT_EQUAL(MSG_FEATURE_NA, sentMessages[0].msg_type);
    TEST_ASSERT_EQUAL(255, sentMessages[0].data[0]);
}

void test_handle_feature_get() {
    BiDiBTestable bidib;

    BiDiBMessage msg;
    msg.msg_type = MSG_FEATURE_GET;
    msg.msg_num = 20;
    msg.data[0] = BIDIB_FEATURE_STRING_SIZE;

    bidib.injectMessage(msg);
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(1, sentMessages.size());
    BiDiBMessage response = sentMessages[0];
    TEST_ASSERT_EQUAL(MSG_FEATURE, response.msg_type);
    TEST_ASSERT_EQUAL(20, response.msg_num);
    TEST_ASSERT_EQUAL(BIDIB_FEATURE_STRING_SIZE, response.data[0]);
    TEST_ASSERT_EQUAL(32, response.data[1]);
}

void test_handle_feature_get_na() {
    BiDiBTestable bidib;

    BiDiBMessage msg;
    msg.msg_type = MSG_FEATURE_GET;
    msg.msg_num = 21;
    msg.data[0] = 99; // Non-existent feature

    bidib.injectMessage(msg);
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(1, sentMessages.size());
    BiDiBMessage response = sentMessages[0];
    TEST_ASSERT_EQUAL(MSG_FEATURE_NA, response.msg_type);
    TEST_ASSERT_EQUAL(21, response.msg_num);
    TEST_ASSERT_EQUAL(99, response.data[0]);
}

void test_handle_feature_set() {
    BiDiBTestable bidib;

    BiDiBMessage msg;
    msg.msg_type = MSG_FEATURE_SET;
    msg.msg_num = 30;
    msg.data[0] = BIDIB_FEATURE_STRING_SIZE;
    msg.data[1] = 64;

    bidib.injectMessage(msg);
    bidib.handleMessages();

    TEST_ASSERT_EQUAL(64, bidib.getFeature(BIDIB_FEATURE_STRING_SIZE));

    TEST_ASSERT_EQUAL(1, sentMessages.size());
    BiDiBMessage response = sentMessages[0];
    TEST_ASSERT_EQUAL(MSG_FEATURE, response.msg_type);
    TEST_ASSERT_EQUAL(30, response.msg_num);
    TEST_ASSERT_EQUAL(BIDIB_FEATURE_STRING_SIZE, response.data[0]);
    TEST_ASSERT_EQUAL(64, response.data[1]);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_handle_feature_getall);
    RUN_TEST(test_handle_feature_getnext_sequence);
    RUN_TEST(test_handle_feature_get);
    RUN_TEST(test_handle_feature_get_na);
    RUN_TEST(test_handle_feature_set);
    UNITY_END();
    return 0;
}
