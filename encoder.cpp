#include <unistd.h>
#include <cstdio>
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>
#include <sys/stat.h>

#include "include/defines.h"
#include "include/HuffmanTree.h"
#include "include/Node.h"
#include "include/BitWriter.h"

void compute_histogram(FILE* infile, uint64_t histogram[]) {
    int c;
    while ((c = fgetc(infile)) != EOF)
        histogram[c] ++;
    histogram[0] ++;
    histogram[ALPHABET - 1] ++;
}

Node* create_tree(uint64_t histogram[]) {
    struct NodeCmp {
        bool operator()(const Node* lhs, const Node* rhs) {
            return lhs->getFrequency() > rhs->getFrequency();
        }
    };
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;
    for (int i = 0; i < ALPHABET; i++) {
        if (histogram[i] > 0) {
            Node* node = new Node(i, histogram[i]);
            pq.push(node);
        }
    }
    while (pq.size() > 1) {
        Node* node1 = pq.top();
        pq.pop();
        Node* node2 = pq.top();
        pq.pop();
        Node* newNode = Node::join(node1, node2);
        pq.push(newNode);
    }
    return pq.top();
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
    Node* root = create_tree(histogram);
    HuffmanTree tree(root);
    Code table[ALPHABET] = {};
    tree.buildCodeTable(table);
    uint16_t unique = 0;
    for (int i = 0; i < ALPHABET; i++)
        if (histogram[i] > 0)
            unique ++;
    struct stat file_info;
    fstat(fileno(infile), &file_info);
    uint64_t file_size = file_info.st_size;
    if (outfile == NULL) {
        fprintf(stderr, "Error: no output file specified.\n");
        return 1;
    }
    Header file_header;
    file_header.magic = MAGIC;
    file_header.file_size = file_size;
    file_header.tree_size = 3 * unique - 1;
    fwrite(&file_header, sizeof(Header), 1, outfile);
    tree.dumpTree(outfile);
    rewind(infile);
    BitWriter writer(outfile);
    int c;
    while ((c = fgetc(infile)) != EOF) {
        writer.writeCode(table[c]);
    }
    writer.flush();
    fflush(outfile);
    if (stats) {
        struct stat out_info;
        fstat(fileno(outfile), &out_info);
        uint64_t compressed_size = out_info.st_size;
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", file_size);
        fprintf(stderr, "Compressed file size: %lu bytes\n", compressed_size);
        fprintf(stderr, "Space saving: %.2f%%\n", 100.0 * (1.0 - (double)compressed_size / file_size));
    }

    fclose(infile);
    fclose(outfile);

}