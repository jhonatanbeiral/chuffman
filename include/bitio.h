#ifndef BITIO_H
#define BITIO_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  FILE *f;
  uint8_t buf;
  int idx;
} BitWriter;

typedef struct {
  FILE *f;
  uint8_t buf;
  int idx;
  int last_read;
} BitReader;

void bw_init (BitWriter *bw, FILE *f);
void bw_write_bit(BitWriter *bw, int bit);
void bw_write_bits(BitWriter *bw, const char *bits);
void bw_flush(BitWriter *bw);

void br_init(BitReader *br, FILE *f);
int br_read_bit(BitReader *br);
void br_close(BitReader *br);

#endif
