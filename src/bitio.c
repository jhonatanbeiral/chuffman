#include "bitio.h"
#include <stdlib.h>

// ----------------------------
// Escrita de bits (BitWriter)
// ----------------------------

void bw_init(BitWriter *bw, FILE *f) {
    bw->f = f;
    bw->buf = 0;
    bw->idx = 0;
}

void bw_write_bit(BitWriter *bw, int bit) {
    if (bit) {
        // coloca um 1 no bit certo dentro do buffer
        bw->buf |= (1 << (7 - bw->idx));
    }
    bw->idx++;

    // se completou 8 bits, escreve o byte no arquivo
    if (bw->idx == 8) {
        fputc(bw->buf, bw->f);
        bw->buf = 0;
        bw->idx = 0;
    }
}

void bw_write_bits(BitWriter *bw, const char *bits) {
    for (; *bits; ++bits) {
        bw_write_bit(bw, (*bits == '1'));
    }
}

void bw_flush(BitWriter *bw) {
    // escreve qualquer resto que não completou 8 bits
    if (bw->idx != 0) {
        fputc(bw->buf, bw->f);
        bw->buf = 0;
        bw->idx = 0;
    }
}

// ----------------------------
// Leitura de bits (BitReader)
// ----------------------------

void br_init(BitReader *br, FILE *f) {
    br->f = f;
    br->buf = 0;
    br->idx = 8; // força a leitura de um novo byte na primeira vez
    br->last_read = 0;
}

int br_read_bit(BitReader *br) {
    if (br->idx == 8) {
        int c = fgetc(br->f);
        if (c == EOF) {
            br->last_read = 1;
            return -1; // fim de arquivo
        }
        br->buf = (uint8_t)c;
        br->idx = 0;
    }

    // extrai o próximo bit do buffer
    int bit = (br->buf >> (7 - br->idx)) & 1;
    br->idx++;
    return bit;
}

void br_close(BitReader *br) {
    (void)br; // nada pra liberar (só evita warning do compilador)
}

