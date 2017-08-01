#include <stdlib.h>
#include <stdio.h>

#include "ppm.h"

void compress_ppm(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}

void decompress_ppm(char *ifile, char *ofile) {
	FILE *ifp = (FILE *)fopen(ifile, "r");
	FILE *ofp = (FILE *)fopen(ofile, "w");

	char c;

	while (fscanf(ifp, "%c", &c) != EOF) {
		fprintf(ofp, "%c", c);
	}

	fclose(ifp);
	fclose(ofp);
}
