#include <stdlib.h>
#include <stdio.h>

#include "ari.h"

void compress_ari(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}

void decompress_ari(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}
