#include <stdlib.h>
#include <stdio.h>

#include "bwt.h"

void compress_bwt(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}

void decompress_bwt(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}
