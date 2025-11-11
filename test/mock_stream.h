#ifndef MOCK_STREAM_H
#define MOCK_STREAM_H

#include <Arduino.h>
#include <queue>

class MockStream : public Stream {
public:
    // Stream-Implementierung
    int available() override {
        return incoming.size();
    }

    int read() override {
        if (incoming.empty()) {
            return -1;
        }
        int val = incoming.front();
        incoming.pop();
        return val;
    }

    int peek() override {
        if (incoming.empty()) {
            return -1;
        }
        return incoming.front();
    }

    size_t write(uint8_t val) override {
        outgoing.push(val);
        return 1;
    }

    void flush() override {
        // Nichts zu tun für den Mock
    }

    // Test-Helfer
    void addIncoming(const uint8_t* data, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            incoming.push(data[i]);
        }
    }

    void clear() {
        while(!incoming.empty()) incoming.pop();
        while(!outgoing.empty()) outgoing.pop();
    }

    int available_outgoing() {
        return outgoing.size();
    }

    void read_outgoing(uint8_t* buffer, size_t length) {
        for(size_t i = 0; i < length; ++i) {
            if(outgoing.empty()) {
                break;
            }
            buffer[i] = outgoing.front();
            outgoing.pop();
        }
    }

    int read_outgoing() {
        if (outgoing.empty()) {
            return -1;
        }
        int val = outgoing.front();
        outgoing.pop();
        return val;
    }

private:
    std::queue<uint8_t> incoming;
    std::queue<uint8_t> outgoing;
};

#endif // MOCK_STREAM_H
