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
	char **directory_list;
	map *map;
	int i;
	char *infile = NULL;
	char *outfile = NULL;
	char *relative_path = NULL;
	int list_entries = 0;
	int list_capacity = 0;
	int return_value = 0;
	FILE *infd = stdin;
	FILE *outfd = stdout;

	return_value = map_create(&map);
	for (i = 1; i < argc && return_value == 0; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'D')
				return_value =
				    add_argument_mapping(argv, &i, map);
			else if (argv[i++][1] == 'I')
				return_value = add_directory_path(
				    &directory_list, &list_capacity,
				    &list_entries, argv[i]);
			else if (argv[i][1] == 'o' && outfile == NULL)
				return_value =
				    copy_file_name(&outfile, argv[i]);
			else
				return 1;
		} else if (infile == NULL) {
			return_value =
			    get_relative_path(argv[i], &relative_path);

			if (return_value == 0)
				return_value = copy_file_name(&infile, argv[i]);
		} else if (outfile == NULL)
			return_value = copy_file_name(&outfile, argv[i]);
		else
			return 1;
	}

	if (return_value == 0)
		return_value = open_file(infile, &infd, "r");
	if (return_value == 0)
		return_value = open_file(outfile, &outfd, "w+");

	if (return_value == 0)
		return_value = read_file(map, infd, outfd, directory_list,
					 list_entries, relative_path);

	if (return_value == 0)
		return_value = close_file(infile, infd, relative_path);

	if (return_value == 0)
		return_value = close_file(outfile, outfd, NULL);

	if (return_value == 0)
		free_directory_list(directory_list, list_entries);

	if (return_value == 0)
		map_destroy(map);

	return return_value;
}
