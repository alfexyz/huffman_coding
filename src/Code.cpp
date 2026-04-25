#include "../include/Code.h"
#include <cstring>

Code::Code() {
    top = 0;
    memset(bits, 0, MAX_CODE_SIZE);
}

bool Code::push(uint8_t bit) {
    if (isFull()) return false;
    if (bit == 1) {
        bits[top / 8] |= (1 << (top % 8));
    } else {
        bits[top / 8] &= ~(1 << (top % 8));
    }
    top++;
    return true;
}

bool Code::pop(uint8_t *bit) {
    if (isEmpty()) return false;
    top--;
    *bit = (bits[top / 8] >> (top % 8)) & 1;
    return true;
}

bool Code::isEmpty() const {
    return top == 0;
}

bool Code::isFull() const {
    return top == MAX_CODE_SIZE * 8;
}

uint32_t Code::size() const {
    return top;
}

uint8_t Code::getBit(uint32_t index) const {
    return (bits[index / 8] >> (index % 8)) & 1;
}