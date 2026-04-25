#pragma once

#include <cstdint>
#include "defines.h"

class Code {
    private:
        uint32_t top;
        uint8_t bits[MAX_CODE_SIZE];
    public:
        Code();
        bool push(uint8_t bit);
        bool pop(uint8_t *bit);
        bool isEmpty() const;
        bool isFull() const;
        uint32_t size() const;
        uint8_t getBit(uint32_t index) const;
};