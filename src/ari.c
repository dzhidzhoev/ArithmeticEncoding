#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "ari.h"
#include "utils.h"

// #define DEBUG_PRINT
// #define DEBUG_DATA

static inline long long max(long long a, long long b) {
    return a < b ? b : a;
}

static inline long long min(long long a, long long b) {
    return a < b ? a : b;
}

static int prev_bits = 0;
static void output_bit(FILE *out, struct bit_rw_buf *buf, unsigned val) {
    write_bit(out, buf, val);
    while (prev_bits) {
        write_bit(out, buf, !val);
        prev_bits--;
    }
}

static unsigned read_bytes = 0;
enum { N = 1 << CHAR_BIT };

struct freq_info {
    unsigned freq;
    int sym;
};

static int cmp_freqs(const void *_f1, const void *_f2) {
    const struct freq_info *f1 = _f1;
    const struct freq_info *f2 = _f2;
    if (f1->freq > f2->freq) {
        return 1;
    } else if (f1->freq == f2->freq) {
        return 0;
    }
    return -1;
}

static struct freq_info freqs[N];
static unsigned freq_ranges[N + 1];
static int total_count;
static int counts[N];
static int global_counts[N];
static unsigned long long freqs_summ;
static const unsigned long long MAX_FREQS_SUMM = (1 << 24) - 1;
static const int WINDOW_LEN = 256;
static const unsigned long long FREQ_ADD_COEFF = (1 << 15);
static const int FREQS_SCALE_COEFF = 2;

typedef uint64_t value_type;
value_type top_val;
value_type half_val;
value_type quart1_val;
value_type quart3_val;

static void clear_counts_table() {
    read_bytes = 0;
    for (int i = 0; i < N; i++) {
        counts[i] = 0;
    }
}

enum { BITS_USED = 40 };
static void init_tables() {
    top_val = (1ull << BITS_USED) - 1;
    half_val = top_val / 2 + 1;
    quart1_val = top_val / 4 + 1;
    quart3_val = half_val + quart1_val;

    freqs_summ = 0;
    for (int i = 0; i < N; i++) {
        freqs[i].freq = 1;
        freqs[i].sym = i;
        freqs_summ++;

        counts[i] = 0;
        global_counts[i] = 0;
    }

    total_count = 0;
}

static void prepare_table_with_stat() {
    for (int i = 0; i < N; i++) {
        if (!global_counts[i]) {
            freqs[i].freq = 0;
        }
    }
}

static void modify_table(int ch) {
    freqs[ch].freq += FREQ_ADD_COEFF;
    counts[ch]++;
    ++read_bytes;
    if (read_bytes % WINDOW_LEN == 0) {
    }

    for (int i = 0; i < N; i++) {
        if (counts[i] == global_counts[i]) {
            freqs[i].freq = 0;
        }
    }
}

static void update_tables() {
    freqs_summ = 0;
    for (int i = 0; i < N; i++) {
        freqs_summ += freqs[i].freq;
    }
    if (freqs_summ >= MAX_FREQS_SUMM) {
        for (int i = 0; i < N; i++) {
            if (freqs[i].freq) {
                freqs[i].freq /= FREQS_SCALE_COEFF;
                freqs[i].freq = max(freqs[i].freq, 1);
            }
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
        freq_ranges[i] = freq_ranges[i - 1] + freqs[i - 1].freq;
    }
}

static void print_table() {
    int summ = 0;
    printf("[");
    for (int i = 0; i < N; i++) {
        summ += freqs[i].freq;
        printf("%d, ", summ);
    }
    printf("]\n");
}

static void write_counts_table(FILE *ofp) {
    int cnt = 0;
    for (int i = 0; i < N; i++) {
        if (global_counts[i]) {
            cnt++;
        }
    }
    fwrite(&cnt, sizeof(cnt), 1, ofp);
    for (int i = 0; i < N; i++) {
        if (global_counts[i]) {
            fputc(i, ofp);
            fwrite(global_counts + i, sizeof(global_counts[i]), 1, ofp);
        }
    }
}

static void read_counts_table(FILE *ifp) {
    int cnt;
    fread(&cnt, sizeof(cnt), 1, ifp);
    for (int i = 0; i < N; i++) {
        global_counts[i] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        int c = fgetc(ifp);
        fread(global_counts + c, sizeof(global_counts[c]), 1, ifp);
    }
}

static void count_stat(FILE *ifp) {
    int ch;
    while ((ch = fgetc(ifp)) != -1) {
        global_counts[ch]++;
        total_count++;
    }
}

void compress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    struct bit_rw_buf rbuf = {}, wbuf = {};
    init_tables();

    count_stat(ifp);
    int32_t input_size = total_count;
    fseek(ifp, 0, SEEK_SET);

    fwrite(&input_size, sizeof(input_size), 1, ofp);

    write_counts_table(ofp);

    prepare_table_with_stat();

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
            left &= top_val;
            right &= top_val;
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

    read_counts_table(ifp);

    prepare_table_with_stat();

    value_type value = 0;
    for (int i = 0; i < BITS_USED; i++) {
        value <<= 1;
        value |= read_bit(ifp, &rbuf);
    }

    value_type left = 0, right = top_val;
    
    int wrote_bytes = 0;
    for (int wrote_bytes = 0; wrote_bytes < file_size; wrote_bytes++) {
        update_tables();
        update_freqs_ranges();
    
        // printf("NEXT LEFT %d RIGHT %d\n", left, right);
        long long cur_point = (((unsigned long long)value - left + 1) * (freqs_summ) - 1) / ((unsigned long long)right - left + 1);
#ifdef DEBUG_PRINT
        printf("CUR POINT %ld FREQS_SUMM %llu VALUE 0x%x %d\n", cur_point, freqs_summ, value, value);
#endif
        int i;
        for (i = 1; i < N; i++) {
            if (freq_ranges[i] > cur_point) {
                break;
            }
        }
        int symbol = freqs[i - 1].sym;
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
            left &= top_val;
            right &= top_val;
            value &= top_val;
#ifdef DEBUG_PRINT
            printf("[%u %u]\n", left, right);
#endif
        }
    }
    fclose(ifp);
    fclose(ofp);
}
