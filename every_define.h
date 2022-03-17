#include <stdio.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

void insert_define_from_file(ht *tabel, char *buf, FILE *infd);

void get_apostrophes(char *string, int **vect, int *el_number);

int check_not_in_between(int pos, int nr_el, int *vec);

void analyze_and_print(ht *map, char *buf, FILE *outfd);

void add_argument_mapping(char **argv, int *line, ht *map);

int undefine_key(ht *map, char *key);
