#include <iostream>
#include <iomanip>
#include <stdint.h>
#include "src/BiDiB.cpp"

int main() {
    BiDiB bidib;
    uint8_t data[] = {0x04, 0x00, 0x00, 0x48, 0x00};
    std::cout << (int)bidib.calculateCrc(data, sizeof(data)) << std::endl;
    return 0;
}
