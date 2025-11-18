#include "BiDiB_Serial.h"
#include <Arduino.h>

BiDiB_Serial::BiDiB_Serial() : BiDiB()
{
}

void BiDiB_Serial::begin(Stream &serial)
{
    BiDiB::begin(serial);
}

void BiDiB_Serial::sendMessage(const BiDiBMessage &msg)
{
    // For simple point-to-point serial, we can send the message directly
    // without worrying about bus contention or polling, so we call the
    // base class's implementation which handles the framing and CRC.
    BiDiB::sendMessage(msg);
}

void BiDiB_Serial::update()
{
    // The base class update() handles receiving messages, which is all
    // that is needed for simple serial.
    BiDiB::update();
}
