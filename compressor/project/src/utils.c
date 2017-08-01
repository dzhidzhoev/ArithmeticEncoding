#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

CompressOptions *parse_args(int argc, char **argv) {
    char help[] = 
"Usage\n\n\
  compress.exe [options]\n\n\
Options\n\n\
  --help                     = Print usage information and exit.\n\
  --input  <file>            = Specify input file to compress/decompress.\n\
  --output <file>            = Specify output file to write a result.\n\
  --mode   {c | d}           = Use specified mode, `c` to compress and `d` to decompress.\n\
  --method {ari | ppm | bwt} = Use specified method of data compression/decompression.\n\n";

    CompressOptions *opts = default_config();

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            printf(help);
            free_compress_opts(opts);
            return NULL;
        }
        else if (!strcmp(argv[i], "--input")) {
            if (++i < argc) {
                opts->ifile = realloc(opts->ifile, sizeof(char) * sizeof(*(argv[i])));
                strcpy(opts->ifile, argv[i]);
                continue;
            }
            else {
                printf("Error: too few arguments\n");
                free_compress_opts(opts);
                return NULL;
            }
        }
        else if (!strcmp(argv[i], "--output")) {
            if (++i < argc) {
                opts->ofile = realloc(opts->ofile, sizeof(char) * sizeof(*(argv[i])));
                strcpy(opts->ofile, argv[i]);
                continue;
            }
            else {
                printf("Error: too few arguments\n");
                free_compress_opts(opts);
                return NULL;
            }
        }
        else if (!strcmp(argv[i], "--mode")) {
            if (++i < argc) {
                if (!strcmp(argv[i], "c")) {
                    opts->mode = 'c';
                    continue;
                }
                else if (!strcmp(argv[i], "d")) {
                    opts->mode = 'd';
                    continue;
                }
                else {
                    printf("Error: unknown mode `%s`\n", argv[i]);
                    free_compress_opts(opts);
                    return NULL;
                }
            }
            else {
                printf("Error: too few arguments\n");
                free_compress_opts(opts);
                return NULL;
            }
        }
        else if (!strcmp(argv[i], "--method")) {
            if (++i < argc) {
                if (!strcmp(argv[i], "ari")) {
                    opts->method = ARI;
                    continue;
                }
                else if (!strcmp(argv[i], "ppm")) {
                    opts->method = PPM;
                    continue;
                }
                else if (!strcmp(argv[i], "bwt")) {
                    opts->method = BWT;
                    continue;
                }
                else {
                    printf("Error: unknown method `%s`\n", argv[i]);
                    free_compress_opts(opts);
                    return NULL;
                }
            }
            else {
                printf("Error: too few arguments\n");
                free_compress_opts(opts);
                return NULL;
            }
        }
        else {
            printf("Error: unexpected option `%s`\n", argv[i]);
            free_compress_opts(opts);
            return NULL;
        }
    }
    print_config(opts);
    return opts;
}

CompressOptions *default_config(void) {
    CompressOptions *default_opts = calloc(1, sizeof(*default_opts));

    default_opts->ifile = calloc(sizeof(char), 10);
    strcpy(default_opts->ifile, "input.txt");

    default_opts->ofile = calloc(sizeof(char), 11);
    strcpy(default_opts->ofile, "output.txt");

    default_opts->mode = 'c';
    default_opts->method = ARI;

    return default_opts;
}

void print_config(CompressOptions *opts) {
    if (opts != NULL) {
        if (opts->ifile != NULL) {
            printf("Input file: `%s`\n", opts->ifile);
        }

        if (opts->ofile != NULL) {
            printf("Output file: `%s`\n", opts->ofile);
        }

        if (opts->mode == 'c') {
            printf("Mode: compression\n");
        }
        else if (opts->mode == 'd') {
            printf("Mode: decompression\n");
        }

        if (opts->method == ARI) {
            printf("Compression: arithmetic encoding\n");
        }
        else if (opts->method == PPM) {
            printf("Compression: PPM\n");
        }
        else if (opts->method == BWT) {
            printf("Compression: arithmetic encoding with Burrows-Wheeler transform\n");
        }
    }
}

void free_compress_opts(CompressOptions *opts) {
    if (opts != NULL) {
        if (opts->ifile != NULL) {
            free(opts->ifile);
        }
        if (opts->ofile != NULL) {
            free(opts->ofile);
        }
        free(opts);
    }
}
