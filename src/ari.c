#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "ari.h"

// #define DEBUG_PRINT
// #define DEBUG_DATA

static inline long long max(long long a, long long b) {
    return a < b ? b : a;
}

static inline long long min(long long a, long long b) {
    return a < b ? a : b;
}

struct bit_rw_buf {
    int wait;
    uint8_t data;
};


void write_buf(FILE *out, struct bit_rw_buf *buf) {
    if (buf->wait) {
        buf->data <<= CHAR_BIT - buf->wait;
        fwrite(&buf->data, sizeof(buf->data), 1, out);
        buf->wait = 0;
#ifdef DEBUG_PRINT
        // printf("BUF FLUSH %x\n", buf->data);
#endif
        buf->data = 0;
    }
}

void read_buf(FILE *in, struct bit_rw_buf *buf) {
    buf->wait += CHAR_BIT * fread(&buf->data, 1, sizeof(buf->data), in);
#ifdef DEBUG_PRINT
    // printf("BUF GRAB %x\n", buf->data);
#endif
}

#ifndef DEBUG_DATA

void write_bit(FILE *out, struct bit_rw_buf *buf, unsigned val) {
    if (buf->wait == CHAR_BIT * sizeof(buf->data)) {
        write_buf(out, buf);
    }
    buf->data <<= 1;
    buf->data |= val;
    buf->wait++;
}

int read_bit(FILE *in, struct bit_rw_buf *buf) {
    while (1) {
        if (buf->wait) {
            int res = (buf->data >> (buf->wait - 1)) & 1;
            buf->wait--;
            return res;
        } 
        if (feof(in)) {
            return 0;
        }
        read_buf(in, buf);
        if (ferror(in)) {
            return 0;
        }
    }
}

#else

void write_bit(FILE *out, struct bit_rw_buf *buf, unsigned val) {
    fputc(val + '0', out);
#ifdef DEBUG_PRINT
    printf("\t\t\t\t\tWRITE BIT %d\n", val);
#endif
}

int read_bit(FILE *in, struct bit_rw_buf *buf) {
    int ch = fgetc(in);
    if (ch > 0) {
        // printf("READ BIT %d\n", ch - '0');
        return ch - '0';
    } else {
        // printf("READ BIT 0\n");
        return 0;
    }
}

#endif

static int prev_bits = 0;
static void output_bit(FILE *out, struct bit_rw_buf *buf, unsigned val) {
    write_bit(out, buf, val);
    while (prev_bits) {
        write_bit(out, buf, !val);
        prev_bits--;
    }
}

static unsigned read_bytes = 0;
static unsigned freqs[1 << CHAR_BIT];
enum { N = sizeof(freqs) / sizeof(*freqs) };
static unsigned freq_ranges[N + 1];
static unsigned counts[N];
static unsigned long long freqs_summ;
const unsigned long long MAX_FREQS_SUMM = (1 << 26) - 1;
const int COEFFS_CNT = 4;
const unsigned long long COEFF[COEFFS_CNT] = {
    1000000,
    5000000,
    10000000,
    50000000,
};
static unsigned long long freqs_coeffs[COEFFS_CNT][N];
const int WINDOW_LEN = 256;
unsigned long long FREQ_ADD_COEFF = (1 << 24);
const int FREQS_SCALE_COEFF = 2;

typedef uint32_t value_type;
const value_type top_val = ~(value_type)0;
const value_type half_val = top_val / 2 + 1;
const value_type quart1_val = top_val / 4 + 1;
const value_type quart3_val = half_val + quart1_val;

static void clear_counts_table() {
    read_bytes = 0;
    for (int i = 0; i < N; i++) {
        counts[i] = 0;
    }
}

static void init_tables() {
    freqs_summ = 0;
    for (int i = 0; i < N; i++) {
        freqs[i] = 1;
        freqs_summ++;
    }

    for (int j = 0; j < COEFFS_CNT; j++) {
        for (int i = 0; i < N; i++) {
            freqs_coeffs[j][i] = 1;
        }
    }
}

static void modify_table(int ch) {
    // freqs[ch] += max(counts[ch], 1) * FREQ_ADD_COEFF;
    // counts[ch]++;
    ++read_bytes;
    // if (read_bytes % WINDOW_LEN == 0) {
    if (read_bytes == WINDOW_LEN) {
        for (int i = 0; i <= 10; i++) {
            freqs[i] = MAX_FREQS_SUMM / 11 - 30;
        }
        for (int i = 11; i <= N; i++) {
            freqs[i] = 1;
        }
        // clear_counts_table();
    }
}

static void update_tables() {
    freqs_summ = 0;
    for (int i = 0; i < N; i++) {
        freqs_summ += freqs[i];
    }
    if (freqs_summ >= MAX_FREQS_SUMM) {
        for (int i = 0; i < N; i++) {
            freqs[i] /= FREQS_SCALE_COEFF;
            freqs[i] = max(freqs[i], 1);
        }
        update_tables();
        return;
    }

#ifdef DEBUG_PRINT
    // printf("FREQS SUMM %d\n", freqs_summ);
#endif
}

static void update_freqs_ranges() {
    freq_ranges[0] = 0;
    for (int i = 1; i <= N; i++) {
        freq_ranges[i] = freq_ranges[i - 1] + freqs[i - 1];
    }
}

static void print_table() {
    int summ = 0;
    printf("[");
    for (int i = 0; i < N; i++) {
        summ += freqs[i];
        printf("%d, ", summ);
    }
    printf("]\n");
}

void compress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    struct bit_rw_buf rbuf = {}, wbuf = {};
    init_tables();

    fseek(ifp, 0, SEEK_END);
    int32_t input_size = ftell(ifp);
    fseek(ifp, 0, SEEK_SET);

    fwrite(&input_size, sizeof(input_size), 1, ofp);

    value_type left = 0, right = top_val;

    int ch;
    while ((ch = fgetc(ifp)) != -1) {
        update_tables();
        update_freqs_ranges();

        unsigned long long delta = (unsigned long long)right - left + 1;
        right = left + freq_ranges[ch + 1] * delta / freqs_summ - 1;
        left = left + freq_ranges[ch] * delta / freqs_summ;
#ifdef DEBUG_PRINT
        printf("WRITE %d\n", ch);
        printf("[%u %u]\n", left, right);
#endif

        while (1) {
            if (right < half_val) {
                output_bit(ofp, &wbuf, 0);
            } else if (left >= half_val) {
                left -= half_val;
                right -= half_val;
                output_bit(ofp, &wbuf, 1);
            } else if (quart1_val <= left && right < quart3_val) {
                left -= quart1_val;
                right -= quart1_val;
                prev_bits++;
            } else 
                break;
            left <<= 1;
            right <<= 1;
            right++;
#ifdef DEBUG_PRINT            
            printf("[%u %u]\n", left, right);
#endif
        }

        modify_table(ch);
    }

    // last interval position encoding
    ++prev_bits;
    if (left < quart1_val) {
        output_bit(ofp, &wbuf, 0);
    } else {
        output_bit(ofp, &wbuf, 1);
    }

    write_buf(ofp, &wbuf);

    fclose(ifp);
    fclose(ofp);
}

void decompress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    struct bit_rw_buf rbuf = {}, wbuf = {};
    init_tables();

    int32_t file_size;
    fread(&file_size, sizeof(file_size), 1, ifp);

    value_type value = 0;
    for (int i = 0; i < CHAR_BIT * sizeof(value_type); i++) {
        value <<= 1;
        value |= read_bit(ifp, &rbuf);
    }

    value_type left = 0, right = top_val;
    
    int wrote_bytes = 0;
    for (int wrote_bytes = 0; wrote_bytes < file_size; wrote_bytes++) {
        update_tables();
        update_freqs_ranges();
    
        // printf("NEXT LEFT %d RIGHT %d\n", left, right);
        long cur_point = (((unsigned long long)value - left + 1) * (freqs_summ) - 1) / ((unsigned long long)right - left + 1);
#ifdef DEBUG_PRINT
        printf("CUR POINT %ld FREQS_SUMM %llu VALUE 0x%x %d\n", cur_point, freqs_summ, value, value);
#endif
        int i;
        for (i = 1; i < N; i++) {
            if (freq_ranges[i] > cur_point) {
                break;
            }
        }
        int symbol = i - 1;
        fputc(symbol, ofp);
#ifdef DEBUG_PRINT
        printf("WRITE %d\n", symbol);
#endif

        unsigned long long delta = (unsigned long long)right - left + 1;
        right = left + freq_ranges[i] * delta / freqs_summ - 1;
        left = left + freq_ranges[i - 1] * delta / freqs_summ;
#ifdef DEBUG_PRINT
        printf("[%u %u]\n", left, right);
#endif

        modify_table(symbol);

        while (1) {
            if (right < half_val) {
            } else if (left >= half_val) {
                value -= half_val;
                right -= half_val;
                left -= half_val;
            } else if (quart1_val <= left && right < quart3_val) {
                value -= quart1_val;
                left -= quart1_val;
                right -= quart1_val;
            } else 
                break;
            value <<= 1;
            value |= read_bit(ifp, &rbuf);
            left <<= 1;
            right <<= 1;
            right++;
#ifdef DEBUG_PRINT
            printf("[%u %u]\n", left, right);
#endif
        }
    }
    fclose(ifp);
    fclose(ofp);
}
