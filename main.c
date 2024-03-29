#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_map.h"
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

	/* Create the map for storing macros */
	return_value = map_create(&map);
	/* Iterate through all arguments given */
	for (i = 1; i < argc && return_value == 0; i++) {
		/* If the current argument is an option */
		if (argv[i][0] == '-') {
			/* If the argument is D */
			if (argv[i][1] == 'D')
				/* We need to add the macro */
				return_value =
				    add_argument_mapping(argv, &i, map);
			/* If the argument is I */
			else if (argv[i++][1] == 'I')
				/* Add the path to the directory path array */
				return_value = add_directory_path(
				    &directory_list, &list_capacity,
				    &list_entries, argv[i]);
			/* If the argument is o and we don't already have an */
			/* outfile */
			else if (argv[i][1] == 'o' && outfile == NULL)
				/* Save the path to the outfile */
				return_value =
				    copy_file_name(&outfile, argv[i]);
			/* Else return an error code, because we don't have */
			/* other options */
			else
				return 1;
			/* If the argument isn't an option, check if we have a
			 */
			/* path to an input file.  We can add a possible input
			 */
			/* file, only if we don't already have one. */
		} else if (infile == NULL) {
			/* Get the relative path to the infile */
			return_value =
			    get_relative_path(argv[i], &relative_path);
			/* Save the path to the infile */
			if (return_value == 0)
				return_value = copy_file_name(&infile, argv[i]);
			/* If the argument isn't an option and we already have
			 */
			/* an input file check if we have a path to an output */
			/* file.  We can add a possible input file, only if we
			 */
			/* don't already have one. */
		} else if (outfile == NULL)
			return_value = copy_file_name(&outfile, argv[i]);
		/* We don't have any types of arguments that haven't been */
		/* treated. Return 1 so that the program finishes. */
		else
			return 1;
	}

	/* After we analyzed all arguments, try to open the infile and outfile
	 */
	if (return_value == 0)
		return_value = open_file(infile, &infd, "r");
	if (return_value == 0)
		return_value = open_file(outfile, &outfd, "w+");

	/* Start to analyze the infile */
	if (return_value == 0)
		return_value = read_file(map, infd, outfd, directory_list,
					 list_entries, relative_path);

	/* Close the input file and output file */
	if (return_value == 0)
		close_file(infile, infd, relative_path);
	if (return_value == 0)
		close_file(outfile, outfd, NULL);

	/* Free the memory allocated for the directory path list */
	if (return_value == 0)
		free_directory_list(directory_list, list_entries);

	/* Free the memory allocated for the map */
	if (return_value == 0)
		map_destroy(map);

	/* Return the exit code */
	return return_value;
}
