#pragma once
#include <cstdio>
#include <cstdint>
#include "defines.h"
#include "Code.h"

class BitWriter {
private:
    FILE *outfile;
    uint8_t buffer;
    uint8_t bit_position;
public:
    BitWriter(FILE *outfile);
    void writeBit(uint8_t bit);
    void writeCode(Code &code);
    void flush();
};