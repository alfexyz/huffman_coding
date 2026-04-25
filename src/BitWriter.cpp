#include "../include/BitWriter.h"

BitWriter::BitWriter(FILE *outfile) {
    this->outfile = outfile;
    this->buffer = 0;
    this->bit_position = 0;
}

void BitWriter::writeBit(uint8_t bit) {
    if (bit == 1) {
        buffer |= (1 << bit_position);
    }
    bit_position++;
    if (bit_position == 8) {
        fputc(buffer, outfile);
        buffer = 0;
        bit_position = 0;
    }
}

void BitWriter::writeCode(Code &code) {
    for (uint32_t i = 0; i < code.size(); i++) {
        writeBit((code.getBit(i)));
    }
}

void BitWriter::flush() {
    if (bit_position > 0) {
        fputc(buffer, outfile);
    }
}