#pragma once

#include <cstdint>
#define ALPHABET 256
#define MAX_CODE_SIZE 32
#define MAGIC 0xDEADEAEF
#define BLOCK 4096
#define MAX_TREE_SIZE (3 * ALPHABET - 1)

struct Header {
    uint32_t magic;
    uint16_t tree_size;
    uint64_t file_size;
};