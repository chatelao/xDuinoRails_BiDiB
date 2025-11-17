#include "BiDiB_RS485.h"
#include <Arduino.h>

BiDiB_RS485::BiDiB_RS485(int8_t de_pin) : BiDiB()
{
    _de_pin = de_pin;
    _queue_head = 0;
    _queue_tail = 0;
}

void BiDiB_RS485::begin(Stream &serial)
{
    BiDiB::begin(serial);
    if (_de_pin >= 0)
    {
        pinMode(_de_pin, OUTPUT);
        digitalWrite(_de_pin, LOW);
    }
}

void BiDiB_RS485::sendMessage(const BiDiBMessage &msg)
{
    // Add message to the send queue
    uint8_t next_head = (_queue_head + 1) % BIDIB_RS485_MAX_QUEUE_SIZE;
    if (next_head != _queue_tail)
    {
        _send_queue[_queue_head] = msg;
        _queue_head = next_head;
    }
    else
    {
        // Queue is full, handle error (e.g., drop message, log error)
    }
}

void BiDiB_RS485::update()
{
    if (!bidib_serial) return;

    while (bidib_serial->available() > 0)
    {
        uint8_t b = bidib_serial->read();

        // The poll is a single MAGIC byte. If we are in FSM_IDLE and receive it,
        // it's our cue to send.
        if (_rx_state == FSM_IDLE && b == BIDIB_MAGIC)
        {
            sendFromQueue();
            // A poll also serves as the start of a potential incoming message,
            // so we let the base class process it as well.
            if (BiDiB::processByte(b)) {
                // A full message was received (unlikely, but possible)
                 _messageAvailable = true;
            }
        }
        else
        {
            if (BiDiB::processByte(b)) {
                // A full message has been received.
                 _messageAvailable = true;
            }
        }
    }
}

void BiDiB_RS485::sendFromQueue()
{
    if (_queue_head != _queue_tail)
    {
        if (_de_pin >= 0)
        {
            digitalWrite(_de_pin, HIGH);
        }

        // Use the base class's original sendMessage to actually send the data
        BiDiB::sendMessage(_send_queue[_queue_tail]);

        if (bidib_serial) {
            bidib_serial->flush();
        }

        if (_de_pin >= 0)
        {
            digitalWrite(_de_pin, LOW);
        }

        _queue_tail = (_queue_tail + 1) % BIDIB_RS485_MAX_QUEUE_SIZE;
    }
}
