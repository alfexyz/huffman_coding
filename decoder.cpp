#include <unistd.h>
#include <cstdio>
#include <cstdint>
#include <sys/stat.h>

#include "include/BitReader.h"
#include "include/defines.h"
#include "include/HuffmanTree.h"
#include "include/Node.h"

int main(int argc, char *argv[]) {
    FILE *infile = NULL;
    FILE *outfile = stdout;
    bool stats = false;

    int opt;
    while ((opt = getopt(argc, argv, "hi:o:s")) != -1) {
        switch (opt) {
            case 'h':
                printf("Huffman Decoder: Decompresses a file compressed with Huffman coding.\n");
                printf("Options:\n");
                printf("  -h          Display this help message\n");
                printf("  -i infile   Specify the compressed input file to decode\n");
                printf("  -o outfile  Specify the output file for decompressed data (default: stdout)\n");
                printf("  -s          Print decompression statistics\n");
                return 0;
            case 'i':
                infile = fopen(optarg, "rb");
                if (infile == NULL) {
                    fprintf(stderr, "Error: could not open input file.\n");
                    return 1;
                }
                break;
            case 'o':
                outfile = fopen(optarg, "wb");
                break;
            case 's':
                stats = true;
                break;
        }
    }

    Header file_header;
    fread(&file_header, sizeof(Header), 1, infile);
    if (file_header.magic != MAGIC) {
        fprintf(stderr, "Error: invalid file. Bad magic number.\n");
        return 1;
    }
    HuffmanTree *tree = HuffmanTree::reconstructTree(infile, file_header.tree_size);

    BitReader reader(infile);
    Node *current = tree->getRoot();
    uint64_t decoded = 0;
    uint8_t bit;

    while (decoded < file_header.file_size) {
        if (!reader.readBit(&bit)) break;

        if (bit == 0) {
            current = current->getLeft();
        } else {
            current = current->getRight();
        }

        if (current->isLeaf()) {
            fputc(current->getSymbol(), outfile);
            decoded++;
            current = tree->getRoot();
        }
    }

    if (stats) {
        struct stat in_info;
        fstat(fileno(infile), &in_info);
        uint64_t compressed_size = in_info.st_size;
        fprintf(stderr, "Compressed file size: %lu bytes\n", compressed_size);
        fprintf(stderr, "Decompressed file size: %lu bytes\n", file_header.file_size);
        fprintf(stderr, "Space saving: %.2f%%\n", 100.0 * (1.0 - (double)compressed_size / file_header.file_size));
    }

    delete tree;
    fclose(infile);
    fclose(outfile);
    return 0;
}