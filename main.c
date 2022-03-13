#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *infile;
char *outfile;
FILE *infd, *outfd;

int main(int argc, char** argv)
{
int i;
infile = NULL;
infd = stdin;
outfile = (char *) malloc(6 * sizeof(char));
strcpy(outfile, "a.out");
for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
		if (argv[i][1] == 'D') {
			
		}
		if (argv[i][1] == 'I') {

		}
		if (argv[i][1] == 'o') {

		} else {
			return 1;
		}
	} else {
		if (infile == NULL) {
			int len = strlen(argv[i]);

			infile = (char *) malloc((len + 1) * sizeof(char));
			strcpy(infile, argv[i]);
			infd = fopen(infile, "r");
			if (infd == NULL)
				return 1;
		} else {
			if (strcmp(outfile, "a.out") == 0) {
				free(outfile);
				outfile = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
				strcpy(outfile, argv[i]);
				outfd = fopen(outfile, "w+");
				if (outfd == NULL)
					return 1;
				} else {
					return 1;
			}
		}
	}
}



char buf[256];

while (fgets(buf, 256, infd)) {
	printf("%s", buf);
}


return 0;
}
