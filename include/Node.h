#pragma once
#include <cstdint>

class Node {
    private:
        Node *left;
        Node *right;
        uint8_t symbol;
        uint64_t frequency;
    public:
        Node(uint8_t symbol, uint64_t frequency);
        static Node* join(Node *left, Node *right);
        uint8_t getSymbol() const;
        uint64_t getFrequency() const;
        Node* getLeft() const;
        Node* getRight() const;
        bool isLeaf();
        ~Node();
};
