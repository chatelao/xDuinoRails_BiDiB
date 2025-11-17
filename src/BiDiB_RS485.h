#ifndef BiDiB_RS485_h
#define BiDiB_RS485_h

#include "BiDiB.h"
#include <Arduino.h>

const uint8_t BIDIB_RS485_MAX_QUEUE_SIZE = 8;

class BiDiB_RS485 : public BiDiB
{
public:
    BiDiB_RS485(int8_t de_pin);
    void begin(Stream &serial);
    void update();
    virtual void sendMessage(const BiDiBMessage &msg);


private:
    int8_t _de_pin;
    BiDiBMessage _send_queue[BIDIB_RS485_MAX_QUEUE_SIZE];
    uint8_t _queue_head;
    uint8_t _queue_tail;

    void sendFromQueue();
};

#endif
