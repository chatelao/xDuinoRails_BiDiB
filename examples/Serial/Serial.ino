#include <Arduino.h>
#include <BiDiB_Serial.h>

// Create a BiDiB_Serial instance for point-to-point communication.
BiDiB_Serial bidib;

// --- Mock Node (simulating the other end of the communication) ---
// In a real setup, this would be a separate device.
// For this example, we'll use a second serial port (Serial1) to talk to ourselves.
// You can test this by connecting the RX1 and TX1 pins on a board like an Arduino Mega.
// Or, by using two USB-to-Serial converters connected to each other (RX->TX, TX->RX).

void setup() {
  // Start the serial communication with your PC for debugging and output.
  Serial.begin(115200);
  while (!Serial); // Wait for the serial port to be ready
  Serial.println("BiDiB Serial P2P Node Example");

  // --- Initialize BiDiB ---
  // Use Serial1 for the BiDiB bus communication.
  // This could be any hardware or software serial port.
  Serial1.begin(115200);
  bidib.begin(Serial1);

  // Set this node's Unique ID.
  // IMPORTANT: For a real application, you must change this to a unique value.
  // You can find your microcontroller's unique ID using various libraries.
  uint8_t myUniqueID[7] = {0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07};
  memcpy(bidib.unique_id, myUniqueID, 7);

  // Set a feature to demonstrate communication.
  // Let's say this node has 4 native accessories.
  bidib.setFeature(12, 4); // FEATURE_ACCESSORY_COUNT

  // --- Logon to the "Bus" ---
  // In a P2P setup, this just announces our presence to the other node.
  bidib.logon();
  Serial.println("Logon message sent.");
}

void loop() {
  // The update() function must be called in the main loop.
  // It handles all incoming communication.
  bidib.update();

  // If a full message has been received by update(), handleMessages()
  // will process its content and trigger any registered callbacks.
  if (bidib.messageAvailable()) {
    Serial.println("Message received, handling...");
    bidib.handleMessages();

    // For debugging, let's print the received message type.
    BiDiBMessage msg = bidib.getLastMessage();
    Serial.print("  -> Message Type: 0x");
    Serial.println(msg.msg_type, HEX);

    // --- Example Response Logic ---
    // If the other node asks for our features, our handleMessages() will
    // respond automatically. Let's add a response for a specific query.
    if (msg.msg_type == MSG_BOOST_QUERY) {
        Serial.println("  -> Received Booster Query. Sending status back.");
        BiDiBMessage response;
        response.length = 4;
        response.address[0] = 0; // The master will fill in our address
        response.msg_num = msg.msg_num;
        response.msg_type = MSG_BOOST_STAT;
        response.data[0] = BIDIB_BST_STATE_ON;
        bidib.sendMessage(response);
    }
  }


  // --- Example: Sending a Message Periodically ---
  // Let's send a query to the other node every 5 seconds.
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 5000) {
    lastSendTime = millis();

    Serial.println("Sending a booster query to the other node...");
    bidib.queryBooster(1); // Query the node with address 1
  }
}
