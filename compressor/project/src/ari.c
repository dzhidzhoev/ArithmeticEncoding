#include <stdlib.h>
#include <stdio.h>

#include "ari.h"

void compress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    /** PUT YOUR CODE HERE
      * implement an arithmetic encoding algorithm for compression
      * don't forget to change header file `ari.h`
    */

    // This is an implementation of simple copying
    size_t n, m;
    unsigned char buff[8192];

    do {
        n = fread(buff, 1, sizeof buff, ifp);
        if (n)
            m = fwrite(buff, 1, n, ofp);
        else 
            m = 0;
    } while ((n > 0) && (n == m));

    fclose(ifp);
    fclose(ofp);
}

void decompress_ari(char *ifile, char *ofile) {
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");

    /** PUT YOUR CODE HERE
      * implement an arithmetic encoding algorithm for decompression
      * don't forget to change header file `ari.h`
    */

    // This is an implementation of simple copying
    size_t n, m;
    unsigned char buff[8192];

    do {
        n = fread(buff, 1, sizeof buff, ifp);
        if (n)
            m = fwrite(buff, 1, n, ofp);
        else 
            m = 0;
    } while ((n > 0) && (n == m));

    fclose(ifp);
    fclose(ofp);
}
