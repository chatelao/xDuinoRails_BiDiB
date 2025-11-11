#include <iostream>
#include <iomanip>
#include <stdint.h>
#include "src/BiDiB.cpp"

int main() {
    BiDiB bidib;
    uint8_t data[] = {3, 0, 0, 4};
    std::cout << (int)bidib.calculateCrc(data, sizeof(data)) << std::endl;
    return 0;
}