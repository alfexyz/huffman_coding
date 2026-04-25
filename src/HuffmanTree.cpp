#include "../include/HuffmanTree.h"
#include "../include/Node.h"
#include "../include/Code.h"
#include <stack>

HuffmanTree::HuffmanTree(Node *root) {
    this->root = root;
}

Node* HuffmanTree::getRoot() const {
    return this->root;
}

HuffmanTree::~HuffmanTree() {
    delete this->root;
}

void HuffmanTree::buildCodeTable(Code table[]) {
    Code c;
    this->buildHelper(this->root, c, table);
}

void HuffmanTree::buildHelper(Node *node, Code &c, Code table[]) {
    if (node->isLeaf()) {
        table[node->getSymbol()] = c;
        return;
    }
    if (node->getLeft()) {
        c.push(0);
        buildHelper(node->getLeft(), c, table);
        uint8_t bit;
        c.pop(&bit);
    }
    if (node->getRight()) {
        c.push(1);
        buildHelper(node->getRight(), c, table);
        uint8_t bit;
        c.pop(&bit);
    }
}

void HuffmanTree::dumpTree(FILE *outfile) {
    dumpHelper(root, outfile);
}

void HuffmanTree::dumpHelper(Node *node, FILE *outfile) {
    if (node->isLeaf()) {
        fputc('L', outfile);
        fputc(node->getSymbol(), outfile);
        return;
    }
    dumpHelper(node->getLeft(), outfile);
    dumpHelper(node->getRight(), outfile);
    fputc('I', outfile);
}

HuffmanTree* HuffmanTree::reconstructTree(FILE *infile, uint16_t tree_size) {
    std::stack<Node*> node_stack;
    uint8_t *tree_dump = new uint8_t[tree_size];
    fread(tree_dump, sizeof(uint8_t), tree_size, infile);

    for (int i = 0; i < tree_size; i++) {
        if (tree_dump[i] == 'L') {
            i++;
            Node *leaf = new Node(tree_dump[i], 0);
            node_stack.push(leaf);
        } else if (tree_dump[i] == 'I') {
            Node *right = node_stack.top();
            node_stack.pop();
            Node *left = node_stack.top();
            node_stack.pop();
            Node *parent = Node::join(left, right);
            node_stack.push(parent);
        }
    }

    Node *root = node_stack.top();
    delete[] tree_dump;
    return new HuffmanTree(root);
}
