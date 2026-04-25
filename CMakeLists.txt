cmake_minimum_required(VERSION 3.28)
project(huffman_coding)

set(CMAKE_CXX_STANDARD 23)

add_executable(encoder encoder.cpp src/Node.cpp src/Code.cpp src/HuffmanTree.cpp src/BitWriter.cpp)
add_executable(decoder decoder.cpp src/Node.cpp src/Code.cpp src/HuffmanTree.cpp src/BitWriter.cpp src/BitReader.cpp)
