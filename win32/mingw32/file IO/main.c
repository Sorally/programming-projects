#include <stdio.h>   /* required for file operations */

FILE *fp;            /* declare the file pointer */

int main(int argc, char *argv[]) {
	
	int n;
	long elapsed_seconds;
	char line[80];

	if (argc != 2) {
		printf("Usage:\t%s FILENAME\n", argv[0]);
		return 0;
	}

	
	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "error opening file");
		return 0;
	}
	
	while(fgets(line, 80, fp) != NULL) {
		if (strcmp(line, "id: en, index: 2\n") == 0) {
			fgets(line, 80, fp);
			fgets(line, 80, fp);
			fgets(line, 80, fp);
			fgets(line, 80, fp);
			fgets(line, 80, fp);
			printf ("%s", line);
		}
	}

	fclose(fp);  /* close the file prior to exiting the routine */

	return 0;
}

