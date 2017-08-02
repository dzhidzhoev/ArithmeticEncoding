#include <stdlib.h>
#include <stdio.h>

#include "bwt.h"

void compress_bwt(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "rb");
	FILE *ofp = (FILE *)fopen(ofile, "wb");

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

void decompress_bwt(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "rb");
	FILE *ofp = (FILE *)fopen(ofile, "wb");

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
