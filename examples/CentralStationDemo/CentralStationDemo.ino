#include <BiDiB.h>

// Use HardwareSerial for communication.
// On Uno, Nano, Mini: Serial is used for USB communication
// On Mega: Serial1, Serial2, Serial3 are available
#define BIDIB_SERIAL Serial1

BiDiB bidib;

// --- Callback Functions ---

void driveAckCallback(uint16_t address, uint8_t status) {
    Serial.print("Drive ACK for address ");
    Serial.print(address);
    Serial.print(", Status: ");
    Serial.println(status);
}

void accessoryAckCallback(uint16_t address, uint8_t status) {
    Serial.print("Accessory ACK for address ");
    Serial.print(address);
    Serial.print(", Status: ");
    Serial.println(status);
}

void boosterStatusCallback(uint8_t status) {
    Serial.print("Booster Status: ");
    switch (status) {
        case BIDIB_BST_STATE_OFF:
            Serial.println("OFF");
            break;
        case BIDIB_BST_STATE_OFF_SHORT:
            Serial.println("OFF (Short Circuit)");
            break;
        case BIDIB_BST_STATE_ON:
            Serial.println("ON");
            break;
        case BIDIB_BST_STATE_ON_LIMIT:
            Serial.println("ON (Current Limited)");
            break;
        default:
            Serial.println(status, HEX);
    }
}

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("BiDiB Central Station Demo");

    // Initialize BiDiB library
    BIDIB_SERIAL.begin(115200);
    bidib.begin(BIDIB_SERIAL);

    // Register callbacks
    bidib.onDriveAck(driveAckCallback);
    bidib.onAccessoryAck(accessoryAckCallback);
    bidib.onBoosterStatus(boosterStatusCallback);

    // Optional: Logon to the bus if you are not the master
    // bidib.logon();
}

void loop() {
    // Update BiDiB library state
    bidib.update();
    bidib.handleMessages();

    // Example commands are sent every 5 seconds
    static unsigned long lastAction = 0;
    if (millis() - lastAction > 5000) {
        lastAction = millis();

        Serial.println("\n--- Sending new commands ---");

        // 1. Drive a loco (Address 3, Speed 100, F1 on)
        Serial.println("Driving loco 3 at speed 100 with F1 ON");
        bidib.drive(3, 100, 0x02); // F1 is bit 1

        // 2. Switch a turnout (Address 10, Output 1, State ON)
        Serial.println("Switching turnout 10 ON");
        bidib.accessory(10, 1, 1);

        // 3. Control a Booster node (Broadcast to all boosters)
        Serial.println("Turning boosters ON");
        bidib.setBoosterState(true, 0); // true = ON, 0 = broadcast

        // Wait a bit, then query the booster
        delay(500);
        Serial.println("Querying boosters");
        bidib.queryBooster(0); // 0 = broadcast

        // Wait a bit, then turn the booster off
        delay(1000);
        Serial.println("Turning boosters OFF");
        bidib.setBoosterState(false, 0);
    }
}
