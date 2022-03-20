#include <stdio.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

#define MAX_CMD_BUF_SIZE 256

/*****************************************
 *  ALL THE BRIEF EXPLANATIONS OF EACH   *
 * FUNCTION ARE IN THE FUNCTIONS.C FILE. *
 *****************************************/

int add_argument_mapping(char **argv, int *line, map *map);

int add_directory_path(char ***list, int *capacity, int *size, char *path);

int copy_file_name(char **infile_name, char *buf);

int get_relative_path(char *buf, char **relative_path);

int read_file(map *map, FILE *infd, FILE *outfd, char **directory_list,
		  int directory_list_size, char *relative_path);

void free_directory_list(char **directory_list, int size);

int open_file(char *name, FILE **fd, char *mode);

void close_file(char *name, FILE *fd, char *relative_path);
