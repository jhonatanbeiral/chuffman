#define _GNU_SOURCE
#include "huffman.h"
#include "bitio.h"

#include <stdlib.h>
#include <string.h>   // strdup
#include <endian.h>   // htobe64, be64toh

// ----------------------------
// Árvore de Huffman
// ----------------------------

static Node* new_node(unsigned char symbol, uint64_t freq, Node* left, Node* right) {
    Node* n = malloc(sizeof(Node));
    if (!n) return NULL;
    n->symbol = symbol;
    n->freq = freq;
    n->left = left;
    n->right = right;
    return n;
}

static void free_node(Node* n) {
    if (!n) return;
    free_node(n->left);
    free_node(n->right);
    free(n);
}

Node* build_huffman_tree(uint64_t freq[ALPHABET]) {
    Node* nodes[ALPHABET];
    size_t count = 0;

    // cria nós folhas para cada símbolo existente
    for (int i = 0; i < ALPHABET; i++) {
        if (freq[i] > 0) {
            nodes[count++] = new_node((unsigned char)i, freq[i], NULL, NULL);
        }
    }

    if (count == 0) return NULL;
    if (count == 1) return new_node(0, 0, nodes[0], NULL);

    // combina nós até restar apenas a raiz
    while (count > 1) {
        // encontra os dois menores
        int min1 = -1, min2 = -1;
        for (size_t i = 0; i < count; i++) {
            if (min1 == -1 || nodes[i]->freq < nodes[min1]->freq) {
                min2 = min1;
                min1 = i;
            } else if (min2 == -1 || nodes[i]->freq < nodes[min2]->freq) {
                min2 = i;
            }
        }

        Node* a = nodes[min1];
        Node* b = nodes[min2];
        Node* merged = new_node(0, a->freq + b->freq, a, b);

        if (min1 > min2) {
            size_t tmp = min1; min1 = min2; min2 = tmp;
        }

        nodes[min1] = merged;
        nodes[min2] = nodes[count - 1];
        count--;
    }

    return nodes[0];
}

void free_huffman_tree(Node* root) {
    free_node(root);
}

// ----------------------------
// Gerar códigos
// ----------------------------

static void gen_codes(Node* n, char* buf, int depth, char* codes[ALPHABET]) {
    if (!n->left && !n->right) {
        buf[depth] = '\0';
        codes[n->symbol] = strdup(buf);
        return;
    }
    if (n->left) {
        buf[depth] = '0';
        gen_codes(n->left, buf, depth + 1, codes);
    }
    if (n->right) {
        buf[depth] = '1';
        gen_codes(n->right, buf, depth + 1, codes);
    }
}

// ----------------------------
// Compressão
// ----------------------------

int compress_file(const char* inpath, const char* outpath) {
    FILE* in = fopen(inpath, "rb");
    if (!in) return -1;

    uint64_t freq[ALPHABET] = {0};
    int c;
    uint64_t total = 0;

    // conta frequência dos símbolos
    while ((c = fgetc(in)) != EOF) {
        freq[(unsigned char)c]++;
        total++;
    }
    rewind(in);

    Node* root = build_huffman_tree(freq);
    if (!root) {
        fclose(in);
        return -2;
    }

    char* codes[ALPHABET] = {0};
    char tmp[ALPHABET];
    gen_codes(root, tmp, 0, codes);

    FILE* out = fopen(outpath, "wb");
    if (!out) {
        fclose(in);
        free_huffman_tree(root);
        return -3;
    }

    // escreve cabeçalho: total de símbolos
    uint64_t be_total = htobe64(total);
    fwrite(&be_total, sizeof(be_total), 1, out);

    // escreve tabela de frequências
    fwrite(freq, sizeof(uint64_t), ALPHABET, out);

    // escreve os bits comprimidos
    BitWriter bw;
    bw_init(&bw, out);
    while ((c = fgetc(in)) != EOF) {
        bw_write_bits(&bw, codes[(unsigned char)c]);
    }
    bw_flush(&bw);

    fclose(in);
    fclose(out);

    for (int i = 0; i < ALPHABET; i++) {
        free(codes[i]);
    }
    free_huffman_tree(root);

    return 0;
}

// ----------------------------
// Descompressão
// ----------------------------

int decompress_file(const char* inpath, const char* outpath) {
    FILE* in = fopen(inpath, "rb");
    if (!in) return -1;

    uint64_t be_total;
    if (fread(&be_total, sizeof(be_total), 1, in) != 1) {
        fclose(in);
        return -2;
    }
    uint64_t total = be64toh(be_total);

    uint64_t freq[ALPHABET];
    if (fread(freq, sizeof(uint64_t), ALPHABET, in) != ALPHABET) {
        fclose(in);
        return -3;
    }

    Node* root = build_huffman_tree(freq);
    if (!root) {
        fclose(in);
        return -4;
    }

    FILE* out = fopen(outpath, "wb");
    if (!out) {
        fclose(in);
        free_huffman_tree(root);
        return -5;
    }

    BitReader br;
    br_init(&br, in);

    Node* n = root;
    for (uint64_t i = 0; i < total; i++) {
        while (n->left || n->right) {
            int bit = br_read_bit(&br);
            if (bit < 0) break;
            n = bit ? n->right : n->left;
        }
        fputc(n->symbol, out);
        n = root;
    }

    fclose(in);
    fclose(out);
    free_huffman_tree(root);

    return 0;
}

