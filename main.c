#include "every_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

char *infile;
char *outfile;
FILE *infd, *outfd;
ht *map;

int main(int argc, char **argv)
{
	int i;
	char buf[256];

	infile = NULL;
	infd = stdin;
	outfd = stdout;
	outfile = NULL;

	map = ht_create();
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'D')
				add_argument_mapping(argv, &i, map);
			else if (argv[i][1] == 'I')
			{
			}
			else if (argv[i][1] == 'o')
			{
			}
			else
				return 1;
		}
		else if (infile == NULL)
		{
			infile = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
			strcpy(infile, argv[i]);
		}
		else if (outfile == NULL)
		{
			outfile = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
			strcpy(outfile, argv[i]);
		}
		else
			return 1;
	}

	if (infile != NULL)
	{
		infd = fopen(infile, "r");
		if (infd == NULL)
			return 1;
	}
	if (outfile != NULL)
	{
		outfd = fopen(outfile, "w+");
		if (outfd == NULL)
			return 1;
	}

	while (fgets(buf, 256, infd))
	{
		if (buf[0] == '#')
		{
			if (strncmp(buf + 1, "define", 6) == 0)
				insert_define_from_file(map, buf + 8, infd);
			else if (strncmp(buf + 1, "undef", 5) == 0)
				undefine_key(map, buf);
			else if (strncmp(buf + 1, "if", 2) == 0)
			{
				int result;
				result = evaluate_if_condition(map, buf + 4);
				if (result)
				{
					fgets(buf, 256, infd);
					while (strncmp(buf, "#else", 5) && strncmp(buf, "#endif", 6))
					{
						analyze_and_print(map, buf, outfd);
						fgets(buf, 256, infd);
					}
					if (strncmp(buf, "#else", 5) == 0)
					{
						while (strncmp(buf, "#endif", 6))
						{
							fgets(buf, 256, infd);
						}
					}
				}
				else
				{
					fgets(buf, 256, infd);
					while (strncmp(buf, "#else", 5) && strncmp(buf, "#endif", 6))
					{
						fgets(buf, 256, infd);
					}
					if (strncmp(buf, "#else", 5) == 0)
					{
						fgets(buf, 256, infd);
						while (strncmp(buf, "#endif", 6))
						{
							analyze_and_print(map, buf, outfd);
							fgets(buf, 256, infd);
						}
					}
				}
			}
		}
		else if (strlen(buf) == 1 && buf[0] == '\n')
			;
		else
			analyze_and_print(map, buf, outfd);
	}

	if (infile != NULL)
	{
		free(infile);
		fclose(infd);
	}
	if (outfile != NULL)
	{
		free(outfile);
		fclose(outfd);
	}

	ht_destroy(map);

	return 0;
}
