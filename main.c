#include "every_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

int main(int argc, char **argv)
{
	char *infile;
	char *outfile;
	char *relative_path;
	char **directory_list;
	int list_entries;
	int list_capacity;
	int return_value;
	FILE *infd, *outfd;
	ht *map;
	int i;
	list_entries = 0;
	list_capacity = 0;
	infile = NULL;
	infd = stdin;
	outfd = stdout;
	outfile = NULL;
	return_value = 0;

	map = ht_create();
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'D')
				add_argument_mapping(argv, &i, map);
			else if (argv[i][1] == 'I')
			{
				i++;
				add_directory_path(&directory_list, &list_capacity, &list_entries, argv[i]);
			}
			else if (argv[i][1] == 'o')
			{
			}
			else
				return 1;
		}
		else if (infile == NULL)
		{
			int k = strlen(argv[i]);
			k--;
			for (; k >= 0 && argv[i][k] != '/'; k--)
				;
			if (k > 0)
			{
				relative_path = (char *)calloc((k + 2), sizeof(char));
				strncpy(relative_path, argv[i], k + 1);
				strcat(relative_path, "\0");
			}

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

	return_value = read_file(map, infd, outfd, directory_list, list_entries, relative_path);
	if (infile != NULL)
	{
		free(infile);
		free(relative_path);
		fclose(infd);
	}
	if (outfile != NULL)
	{
		free(outfile);
		fclose(outfd);
	}

	if (list_capacity != 0)
	{
		int i;
		for (i = 0; i < list_entries; i++)
		{
			free(directory_list[i]);
		}
		free(directory_list);
	}

	ht_destroy(map);

	return return_value;
}
