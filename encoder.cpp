#include <unistd.h>
#include <cstdio>
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

#include "include/defines.h"
#include "include/Node.h"

void compute_histogram(FILE* infile, uint64_t histogram[]) {
    int c;
    while ((c = fgetc(infile)) != EOF)
        histogram[c] ++;
    histogram[0] ++;
    histogram[ALPHABET - 1] ++;
}

void create_tree(uint64_t histogram[]) {
    struct NodeCmp {
        bool operator()(const Node* lhs, const Node* rhs) {
            return lhs->getFrequency() > rhs->getFrequency();
        }
    };
    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> pq;
    for (auto x : histogram) {
        Node* node = new Node(x, histogram[x]);
        pq.push(node);
    }
    while (!pq.empty()) {
        Node* node1 = pq.top();
        pq.pop();
        Node* node2 = pq.top();
        pq.pop();
        Node* newNode = Node::join(node1, node2);
        pq.push(newNode);
    }
}

int main(int argc, char *argv[]) {
    FILE *infile = stdin;
    FILE *outfile = NULL;
    bool stats = false;

    int opt;
    while ((opt = getopt(argc, argv, "hi:o:s")) != -1) {
        switch (opt) {
            case 'h':
                printf("Huffman Encoder: Compresses a file using Huffman coding.\n");
                printf("Options:\n");
                printf("  -h          Display this help message\n");
                printf("  -i infile   Specify the input file to encode (default: stdin)\n");
                printf("  -o outfile  Specify the output file for compressed data\n");
                printf("  -s          Print compression statistics\n");
                return 0;
            case 'i':
                infile = fopen( optarg, "rb");
                if (infile == NULL) {
                    fprintf(stderr, "Error: could not open input file.\n");
                    return 1;
                }
                break;
            case 'o':
                outfile = fopen( optarg, "wb");
                break;
            case 's':
                stats = true;
                break;
        }

    }
    uint64_t histogram[ALPHABET] = {};
    compute_histogram(infile, histogram);
    create_tree(histogram);
}