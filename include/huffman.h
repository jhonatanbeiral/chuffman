#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>

#define ALPHABET 256

typedef struct Node {
    unsigned char symbol;
    uint64_t freq;
    struct Node *left, *right;
} Node;

Node* build_huffman_tree(uint64_t freq[ALPHABET]);
void free_huffman_tree(Node* root);

int compress_file(const char *inpath, const char *outpath);
int decompress_file(const char *inpath, const char *outpath);

#endif
