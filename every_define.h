#include <stdio.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

int check_not_in_between(int pos, int nr_el, int *vec);

int add_argument_mapping(char **argv, int *line, ht *map);

int undefine_key(ht *map, char *key);

void solve_ifs(ht *map, char *buf, FILE *infd, FILE *outfd);

int evaluate_if_condition(ht *map, char *key);

int check_if_defined(ht *map, char *key);

int add_header_file(char *buf, char **directory_list, FILE *outfd, ht *map, int directory_list_size, char *relative_path);

int read_file(ht *map, FILE *infd, FILE *outfd, char **directory_list, int directory_list_size, char *relative_path);

int add_directory_path(char ***list, int *capacity, int *size, char *path);

int get_relative_path(char *buf, char **relative_path);

int copy_file_name(char **infile_name, char *buf);
