#pragma once
#include <cstdio>
#include "Node.h"
#include "Code.h"

class HuffmanTree {
    private:
        Node *root;
        void buildHelper(Node *node, Code &c, Code table[]);
        void dumpHelper(Node *node, FILE* outfile);
    public:
        HuffmanTree(Node *root);
        Node* getRoot() const;
        void buildCodeTable(Code table[]);
        void dumpTree(FILE *outfile);
        static HuffmanTree* reconstructTree(FILE *infile, uint16_t tree_size);
        ~HuffmanTree();
};