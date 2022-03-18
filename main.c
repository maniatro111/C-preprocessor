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

	return_value = ht_create(&map);
	for (i = 1; i < argc && return_value == 0; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'D')
				return_value = add_argument_mapping(argv, &i, map);
			else if (argv[i][1] == 'I')
			{
				i++;
				return_value = add_directory_path(&directory_list, &list_capacity, &list_entries, argv[i]);
			}
			else if (argv[i][1] == 'o')
			{
			}
			else
				return 1;
		}
		else if (infile == NULL)
		{
			return_value = get_relative_path(argv[i], &relative_path);

			if (return_value == 0)
				return_value = copy_file_name(&infile, argv[i]);
		}
		else if (outfile == NULL)
		{
			return_value = copy_file_name(&outfile, argv[i]);
		}
		else
			return 1;
	}

	if (infile != NULL && return_value == 0)
	{
		infd = fopen(infile, "r");
		if (infd == NULL)
			return 1;
	}
	if (outfile != NULL && return_value == 0)
	{
		outfd = fopen(outfile, "w+");
		if (outfd == NULL)
			return 1;
	}

	if (return_value == 0)
		return_value = read_file(map, infd, outfd, directory_list, list_entries, relative_path);
	if (infile != NULL && return_value == 0)
	{
		free(infile);
		free(relative_path);
		fclose(infd);
	}
	if (outfile != NULL && return_value == 0)
	{
		free(outfile);
		fclose(outfd);
	}

	if (list_capacity != 0 && return_value == 0)
	{
		int i;
		for (i = 0; i < list_entries; i++)
		{
			free(directory_list[i]);
		}
		free(directory_list);
	}

	if (return_value == 0)
		ht_destroy(map);

	return return_value;
}
