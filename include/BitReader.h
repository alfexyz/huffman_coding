#pragma once
#include <cstdio>
#include <cstdint>

class BitReader {
    private:
        FILE *infile;
        uint8_t buffer;
        uint8_t bit_position;
        bool eof;
    public:
        BitReader(FILE *infile);
        bool readBit(uint8_t *bit);
};