# Huffman Coding — Lossless File Compressor

A command-line lossless file compression tool built in C++23, implementing Huffman coding from scratch.
The project was developed as part of an Object-Oriented Programming lab, and covers the full pipeline:
frequency analysis → tree construction → code generation → binary serialization → bitstream encoding/decoding.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build](#build)
- [Usage](#usage)
  - [Encoder](#encoder)
  - [Decoder](#decoder)
- [How It Works](#how-it-works)
  - [Step 1 — Frequency Analysis](#step-1--frequency-analysis)
  - [Step 2 — Huffman Tree Construction](#step-2--huffman-tree-construction)
  - [Step 3 — Code Table Generation](#step-3--code-table-generation)
  - [Step 4 — File Header & Tree Serialization](#step-4--file-header--tree-serialization)
  - [Step 5 — Bitstream Encoding](#step-5--bitstream-encoding)
  - [Step 6 — Decoding](#step-6--decoding)
- [Compressed File Format](#compressed-file-format)
- [Class Design](#class-design)
- [Compression Results](#compression-results)

---

## Overview

Huffman coding is a well-known greedy algorithm that assigns shorter binary codes to more frequent symbols,
producing an optimal prefix-free code for any given frequency distribution.
This implementation supports encoding and decoding arbitrary binary files (not just text),
operates entirely at the bit level, and self-contains the tree inside the compressed file so no external dictionary is needed.

---

## Features

- Encodes **any file** — works at the byte level (256-symbol alphabet)
- Self-describing compressed format — tree is embedded in the output file alongside a magic number and original file size
- Bit-level I/O via custom `BitWriter` and `BitReader` classes — no wasted padding overhead beyond the last byte
- Priority-queue-based tree construction using the STL `priority_queue`
- Stack-based tree reconstruction on the decoder side — no recursion needed
- Optional `-s` flag on both tools to print compression statistics
- Builds two independent executables: `encoder` and `decoder`
- Zero external dependencies — only the C++ standard library and POSIX APIs

---

## Project Structure

```
huffman_coding/
├── CMakeLists.txt        # Build configuration (C++23, two executables)
├── encoder.cpp           # Encoder entry point
├── decoder.cpp           # Decoder entry point
├── include/
│   ├── defines.h         # Shared constants and the Header struct
│   ├── Node.h            # Binary tree node
│   ├── Code.h            # Variable-length bitstring (one Huffman code)
│   ├── HuffmanTree.h     # Tree: build, serialize, reconstruct
│   ├── BitWriter.h       # Bit-level output stream
│   └── BitReader.h       # Bit-level input stream
├── src/
│   ├── Node.cpp
│   ├── Code.cpp
│   ├── HuffmanTree.cpp
│   ├── BitWriter.cpp
│   └── BitReader.cpp
├── test.txt              # "hello"
├── test2.txt             # "this is an example of a huffman tree"
└── big_test.txt          # Stress test (~37 KB, repeated content)
```

---

## Build

Requires CMake ≥ 3.28 and a C++23-capable compiler.

```bash
cmake -S . -B build
cmake --build build
```

This produces two executables inside `build/`: `encoder` and `decoder`.

---

## Usage

### Encoder

```
./encoder -i <input_file> -o <output_file> [-s] [-h]

Options:
  -h          Display help
  -i infile   Input file to compress
  -o outfile  Output file for compressed data
  -s          Print compression statistics
```

**Example:**

```bash
./encoder -i test2.txt -o test2.huff -s
```

```
Original file size:    38 bytes
Compressed file size:  30 bytes
Space saving:          21.05%
```

---

### Decoder

```
./decoder -i <input_file> [-o <output_file>] [-s] [-h]

Options:
  -h          Display help
  -i infile   Compressed input file
  -o outfile  Output file (default: stdout)
  -s          Print decompression statistics
```

**Example:**

```bash
./decoder -i test2.huff -o test2_decoded.txt -s
```

---

## How It Works

### Step 1 — Frequency Analysis

The encoder reads through the entire input file and counts how many times each byte value (0–255) appears,
building a frequency histogram of 256 entries.

Two sentinel entries (`histogram[0]` and `histogram[255]`) are always incremented before tree construction.
This guarantees that even a completely homogeneous file (e.g., one made up of a single repeated byte)
still produces a valid two-node Huffman tree rather than a degenerate single-node structure.

---

### Step 2 — Huffman Tree Construction

All bytes with non-zero frequency are inserted into a **min-heap priority queue** as leaf `Node` objects,
ordered by ascending frequency.

The standard Huffman greedy algorithm then runs:

```
while queue.size() > 1:
    left  = queue.pop()   // lowest frequency
    right = queue.pop()   // second lowest
    parent = Node::join(left, right)   // frequency = left + right
    queue.push(parent)
```

When the loop ends the single remaining element is the tree root.
Frequently occurring symbols end up close to the root, giving them shorter codes.

---

### Step 3 — Code Table Generation

`HuffmanTree::buildCodeTable()` performs a recursive depth-first traversal of the tree,
maintaining a `Code` object that accumulates bits as it descends:

- Going **left** → push bit `0`
- Going **right** → push bit `1`
- Reaching a **leaf** → copy the accumulated `Code` into the code table at index `leaf->symbol`
- **Backtracking** → pop the last bit

The result is a 256-entry table where `table[byte_value]` holds the variable-length Huffman code for that byte.

---

### Step 4 — File Header & Tree Serialization

Before any encoded data, the encoder writes a **12-byte header**:

| Field       | Type       | Description                               |
|-------------|------------|-------------------------------------------|
| `magic`     | `uint32_t` | `0xDEADEAEF` — validates the file format  |
| `tree_size` | `uint16_t` | Byte length of the serialized tree        |
| `file_size` | `uint64_t` | Original (uncompressed) file size         |

Immediately after the header, the tree is serialized via a **pre-order traversal**:

- For each **leaf** node: emit byte `'L'` (0x4C) followed by the symbol byte
- For each **internal** node: recurse into children first, then emit byte `'I'` (0x49)

The resulting flat byte stream encodes the full tree shape and all leaf symbols.
The decoder rebuilds the tree from this stream using a stack:

- On `'L'`: create a leaf node and push it onto the stack
- On `'I'`: pop two nodes, join them into a parent, push the parent
- Final stack element is the reconstructed root

The serialized tree is always at most `3 × unique_symbols − 1` bytes long.

---

### Step 5 — Bitstream Encoding

With the tree serialized, the encoder rewinds to the beginning of the input and processes each byte:

1. Look up the byte in the code table
2. Feed every bit of its code to `BitWriter::writeBit()`
3. `BitWriter` accumulates bits into a byte buffer and flushes to disk when 8 bits are ready
4. After the last byte, `BitWriter::flush()` writes any partial byte with zero-padding

The encoded payload follows immediately after the serialized tree in the output file.

---

### Step 6 — Decoding

The decoder reads the header, validates the magic number, and reconstructs the Huffman tree.
It then traverses the tree bit by bit using `BitReader`:

```
current = root
while decoded < file_size:
    bit = BitReader::readBit()
    if bit == 0: current = current->left
    if bit == 1: current = current->right
    if current is a leaf:
        write current->symbol to output
        decoded++
        current = root   // reset for next symbol
```

The `file_size` stored in the header tells the decoder exactly how many symbols to emit,
preventing any confusion caused by the padding bits at the end of the last byte.

---

## Compressed File Format

```
┌──────────────────────────────────────────┐
│  Header (12 bytes)                       │
│    magic     : 4 bytes  (0xDEADEAEF)     │
│    tree_size : 2 bytes                   │
│    file_size : 8 bytes (original size)   │  ← needed by decoder to stop at exact symbol count
├──────────────────────────────────────────┤
│  Serialized Huffman Tree (variable)      │
│    pre-order: 'L'<byte> for leaves,      │
│               'I' for internal nodes     │
│    max size: 3 × unique_symbols − 1      │
├──────────────────────────────────────────┤
│  Encoded Bitstream (variable)            │
│    variable-length codes packed into     │
│    bytes, LSB-first per byte             │
│    last byte zero-padded if needed       │
└──────────────────────────────────────────┘
```

---

## Class Design

| Class         | Responsibility                                                                 |
|---------------|--------------------------------------------------------------------------------|
| `Node`        | Single node in the Huffman tree; leaf or internal; owns its children           |
| `Code`        | Stores one variable-length Huffman code as a packed bit array (up to 32 bits) |
| `HuffmanTree` | Wraps the root; builds code table, serializes/deserializes the tree            |
| `BitWriter`   | Buffers individual bits and flushes complete bytes to a `FILE*` stream         |
| `BitReader`   | Reads complete bytes from a `FILE*` stream and exposes them bit by bit         |

---

## Compression Results

| File          | Original | Compressed | Saving   |
|---------------|----------|------------|----------|
| `test.txt`    | 6 B      | ~18 B      | overhead (file too small) |
| `test2.txt`   | 38 B     | ~30 B      | ~21%     |
| `big_test.txt`| ~37 KB   | ~25 KB     | ~32%     |

For small files the header and serialized tree dominate, so compression is only effective once the payload
is large enough for the frequency-optimized codes to offset the fixed overhead.
The `big_test.txt` result demonstrates the expected behaviour on repetitive natural-language input.
