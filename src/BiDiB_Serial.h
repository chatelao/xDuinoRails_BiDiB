#ifndef BiDiB_Serial_h
#define BiDiB_Serial_h

#include "BiDiB.h"
#include <Arduino.h>

class BiDiB_Serial : public BiDiB
{
public:
    BiDiB_Serial();
    void begin(Stream &serial);
    void update();
    virtual void sendMessage(const BiDiBMessage &msg);
};

#endif
