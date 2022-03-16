#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "every_define.h"
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
			{
				if (strlen(argv[i]) > 2)
				{
					argv[i] = argv[i] + 2;
				}
				else
					i++;

				if (strchr(argv[i], '=') != NULL)
				{
					char *aux;
					aux = strtok(argv[i], "=");
					ht_set(map, argv[i], argv[i] + strlen(argv[i]) + 1);
				}
				else
				{
					ht_set(map, argv[i], "");
				}
			}
			else if (argv[i][1] == 'I')
			{
			}
			else if (argv[i][1] == 'o')
			{
			}
			else
			{
				return 1;
			}
		}
		else
		{
			if (infile == NULL)
			{
				infile = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
				strcpy(infile, argv[i]);
				infd = fopen(infile, "r");
				if (infd == NULL)
					return 1;
			}
			else
			{
				if (outfile == NULL)
				{
					outfile = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char));
					strcpy(outfile, argv[i]);
					outfd = fopen(outfile, "w+");
					if (outfd == NULL)
					{
						return 1;
					}
				}
				else
				{
					return 1;
				}
			}
		}
	}

	int j;
	for (j = 0; j < (int)map->capacity; j++)
		if (map->entries[j].key != NULL)
		{
			printf("%s %s\n", map->entries[j].key, map->entries[j].value);
			// printf("%s\n", ht_get(map, map->entries[j].key));
		}

	while (fgets(buf, 256, infd))
	{
		if (buf[0] == '#')
		{
			if (strncmp(buf + 1, "define", 6) == 0)
			{
				insert_define_from_file(map, buf + 7, infd);
			}
		}
		else
		{
			if (strlen(buf) == 1 && buf[0] == '\n')
				;
			else
				fprintf(outfd, "%s", buf);
		}
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
