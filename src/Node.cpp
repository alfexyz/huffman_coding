#include "../include/Node.h"

Node::Node(uint8_t symbol, uint64_t frequency) {
    this->symbol = symbol;
    this->frequency = frequency;
    this->left = nullptr;
    this->right = nullptr;
}

Node* Node::getLeft() const {
    return this->left;
}

Node* Node::getRight() const {
    return this->right;
}

uint64_t Node::getFrequency() const {
    return this->frequency;
}

uint8_t Node::getSymbol() const {
    return this->symbol;
}

bool Node::isLeaf() {
    return this->left == nullptr && this->right == nullptr;
}

Node* Node::join(Node* left, Node* right) {
    Node* parent = new Node('$', left->getFrequency() + right->getFrequency());
    parent->left = left;
    parent->right = right;
    return parent;
}

Node::~Node() {
    delete left;
    delete right;
}
