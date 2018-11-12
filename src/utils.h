#pragma once

#include <limits.h>
#include <stdint.h>
#include <stdio.h>

enum {
    ARI,
    PPM,
    BWT
};

typedef struct CompressOptions {
    char *ifile;
    char *ofile;
    char mode;
    int  method;
} CompressOptions;

CompressOptions *parse_args(int argc, char **argv);

CompressOptions *default_config(void);

void print_config(CompressOptions *opts);

void free_compress_opts(CompressOptions *opts);

int can_open_file(const char *filename);

struct bit_rw_buf {
    int wait;
    uint8_t data;
};

void write_bit(FILE *out, struct bit_rw_buf *buf, unsigned val);

int read_bit(FILE *in, struct bit_rw_buf *buf);

void write_buf(FILE *out, struct bit_rw_buf *buf);

void read_buf(FILE *in, struct bit_rw_buf *buf);