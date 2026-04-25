#include "../include/BitReader.h"

BitReader::BitReader(FILE *infile) {
    this->infile = infile;
    this->buffer = 0;
    this->bit_position = 8;
    this->eof = false;
}

bool BitReader::readBit(uint8_t *bit) {
    if (bit_position == 8) {
        int c = fgetc(infile);
        if (c == EOF) {
            eof = true;
            return false;
        }
        buffer = (uint8_t)c;
        bit_position = 0;
    }
    *bit = (buffer >> bit_position) & 1;
    bit_position++;
    return true;
}