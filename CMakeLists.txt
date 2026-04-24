cmake_minimum_required(VERSION 4.0)
project(huffman_coding)

set(CMAKE_CXX_STANDARD 23)

add_executable(encoder encoder.cpp src/Node.cpp src/Code.cpp src/HuffmanTree.cpp)
add_executable(decoder decoder.cpp src/Node.cpp src/Code.cpp src/HuffmanTree.cpp)
