#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif
#include "every_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check_not_in_between(int pos, int nr_el, int *vec)
{
	int i;

	if (!nr_el)
		return 1;
	for (i = 0; i < nr_el; i += 2)
		if (pos > vec[i] && pos < vec[i + 1])
			return 0;
	return 1;
}

static int get_apostrophes(char *string, int **vect, int *el_number)
{
	*el_number = 0;
	if (strchr(string, '\"')) {
		int initial_size;

		initial_size = 10;
		*vect = (int *)malloc(initial_size * sizeof(int));
		if ((*vect) == NULL)
			return 12;
		(*vect)[*el_number] = strchr(string, '\"') - string;
		(*el_number)++;
		while (1 + (*vect)[(*el_number) - 1] < strlen(string) &&
		       strchr(string + 1 + (*vect)[(*el_number) - 1], '\"')) {
			(*vect)[*el_number] =
			    strchr(string + 1 + (*vect)[(*el_number) - 1],
				   '\"') -
			    string;
			(*el_number)++;
		}
	}
	return 0;
}
static int search_for_key_and_copy(map *tabel, char **value, int *start,
				   int *end)
{
	char *aux;

	aux = (char *)calloc((*end - *start + 1), sizeof(char));
	if (aux == NULL)
		return 12;
	strncpy(aux, (*value) + *start, *end - *start);
	strcat(aux, "\0");

	if (map_get(tabel, aux)) {
		char *valoare;
		char *copie;

		valoare = map_get(tabel, aux);
		copie = (char *)calloc(
		    (strlen(*value) - strlen(aux) + strlen(valoare) + 1),
		    sizeof(char));
		if (copie == NULL) {
			free(aux);
			return 12;
		}
		strncpy(copie, *value, *start);
		strcat(copie, valoare);
		strcat(copie, (*value) + *end);
		strcat(copie, "\0");
		*end = *start + strlen(valoare);
		free(*value);
		*value = copie;
	} else
		*end = *start + strlen(aux);
	free(aux);
	*start = *end + 1;
	return 0;
}

static int check_define_in_define(map *tabel, char **value)
{
	int start = 0;
	int end = 0;
	int return_value = 0;

	for (; end < strlen(*value) && return_value == 0; end++)
		if (strchr("\t []{}<>=+-*/%!&|^.,:;()\\", (*value)[end]))
			return_value =
			    search_for_key_and_copy(tabel, value, &start, &end);
	if (return_value == 0)
		return_value =
		    search_for_key_and_copy(tabel, value, &start, &end);
	return return_value;
}

static void eliminate_tabs_and_add_space(char *dest, char *source)
{
	int i = 0;

	while (i < strlen(source) && (source[i] == ' ' || source[i] == '\t'))
		i++;
	strcat(dest, " ");
	strcat(dest, source + i);
}

static int insert_define_from_file(map *tabel, char *buf, FILE *infd)
{
	char *argumente;
	char *aux;
	char *key;
	int return_value = 0;

	argumente = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	if (argumente == NULL)
		return 12;
	strcpy(argumente, buf);
	aux = strtok(argumente, "\n ");
	key = (char *)malloc((strlen(aux) + 1) * sizeof(char));
	if (key == NULL) {
		free(argumente);
		return 12;
	}
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	if (aux == NULL)
		return_value = map_set(tabel, key, "");
	else if (aux[strlen(aux) - 1] == '\\') {
		char *argumente_concatenate;
		int i;

		argumente_concatenate = (char *)calloc(250, sizeof(char));
		if (argumente_concatenate != NULL)
			strcat(argumente_concatenate, aux);
		argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';
		for (i = strlen(argumente_concatenate) - 1;
		     i >= 0 && (argumente_concatenate[i] == ' ' ||
				argumente_concatenate[i] == '\t');
		     i--)
			argumente_concatenate[i] = '\0';
		fgets(buf, 256, infd);
		buf[strlen(buf) - 1] = '\0';
		while (buf[strlen(buf) - 1] == '\\') {
			eliminate_tabs_and_add_space(argumente_concatenate,
						     buf);
			argumente_concatenate[strlen(argumente_concatenate) -
					      1] = '\0';
			for (i = strlen(argumente_concatenate);
			     i >= 1 && (argumente_concatenate[i] == ' ' &&
					argumente_concatenate[i - 1] == ' ');
			     i--)
				argumente_concatenate[i] = '\0';
			fgets(buf, 256, infd);
			buf[strlen(buf) - 1] = '\0';
		}
		eliminate_tabs_and_add_space(argumente_concatenate, buf);

		char *mimi;

		mimi = (char *)malloc((strlen(aux) + 1) * sizeof(char));
		if (mimi == NULL)
			return_value = 12;
		if (return_value == 0) {
			strcpy(mimi, aux);
			return_value = check_define_in_define(tabel, &mimi);
		}
		if (return_value == 0)
			return_value =
			    map_set(tabel, key, argumente_concatenate);
		if (return_value == 0)
			free(mimi);
		free(argumente_concatenate);
	} else {
		char *mimi;

		mimi = (char *)malloc((strlen(aux) + 1) * sizeof(char));
		if (mimi == NULL)
			return_value = 12;
		if (return_value == 0) {
			strcpy(mimi, aux);
			return_value = check_define_in_define(tabel, &mimi);
		}
		if (return_value == 0)
			return_value = map_set(tabel, key, mimi);
		if (return_value == 0)
			free(mimi);
	}
	free(key);
	free(argumente);
	return return_value;
}

static int check_and_copy(int pos, int dimension, int *v, char *buf, map *map,
			  int map_index)
{
	if (check_not_in_between(pos, dimension, v)) {
		char *aux;

		aux = (char *)calloc(256, sizeof(char));
		if (aux == NULL)
			return 12;
		strcpy(aux, buf);
		strncpy(buf, aux, pos);
		strcpy(buf + pos, map->entries[map_index].value);
		strcpy(buf + pos + strlen(map->entries[map_index].value),
		       aux + pos + strlen(map->entries[map_index].key));
		strcat(buf, "\0");
		free(aux);
	}
	return 0;
}

static int analyze_and_print(map *map, char *buf, FILE *outfd)
{
	int *v;
	int n;
	char *copie;
	int start = 0;
	int end = 0;
	int return_value = 0;

	copie = (char *)calloc(256, sizeof(char));
	if (copie == NULL)
		return 12;
	strcpy(copie, buf);
	return_value = get_apostrophes(buf, &v, &n);
	for (; end < strlen(copie) && return_value == 0; end++)
		if (strchr("\t []{}<>=+-*/%!&|^.,:;()\\", (copie)[end]))
			return_value =
			    search_for_key_and_copy(map, &copie, &start, &end);
	if (return_value == 0)
		return_value =
		    search_for_key_and_copy(map, &copie, &start, &end);
	if (return_value == 0) {
		if (copie[0] == '\t')
			copie[0] = ' ';
		fprintf(outfd, "%s", copie);
		if (n)
			free(v);
	}
	free(copie);
	return return_value;
}

int add_argument_mapping(char **argv, int *line, map *map)
{
	int return_value = 0;

	if (strlen(argv[*line]) > 2)
		argv[*line] = argv[*line] + 2;
	else
		(*line)++;

	if (strchr(argv[*line], '=') != NULL) {
		char *aux;

		aux = strtok(argv[*line], "=");
		return_value = map_set(map, argv[*line],
				       argv[*line] + strlen(argv[*line]) + 1);
	} else
		return_value = map_set(map, argv[*line], "");
}

static int undefine_key(map *map, char *key)
{
	key[strlen(key) - 1] = '\0';
	delete_entry(map, key);
}

static int turn_to_int_and_check(char *eval)
{
	int a;

	a = atoi(eval);
	if (a == 0)
		return 0;
	else
		return 1;
}

static int evaluate_if_condition(map *map, char *key)
{
	char *aux;

	key[strlen(key) - 1] = '\0';
	aux = map_get(map, key);
	if (aux == NULL)
		return turn_to_int_and_check(key);
	strcpy(key, aux);
	return turn_to_int_and_check(key);
}

static int check_if_defined(map *map, char *key)
{
	key[strlen(key) - 1] = '\0';
	return macro_defined(map, key);
}

static FILE *check_if_file_in_dir(char **directory_list,
				  int directory_list_size, char *file)
{
	int i;

	for (i = 0; i < directory_list_size; i++) {
		char *path;
		FILE *fd;

		path = (char *)calloc(
		    (strlen(directory_list[i]) + strlen(file) + 2),
		    sizeof(char));
		if (path == NULL)
			return NULL;
		strcpy(path, directory_list[i]);
		strcat(path, "/");
		strcat(path, file);
		strcat(path, "\0");
		fd = fopen(path, "r");
		free(path);
		if (fd)
			return fd;
	}
	return NULL;
}

static int add_header_file(char *buf, char **directory_list, FILE *outfd,
			   map *map, int directory_list_size,
			   char *relative_path)
{
	char *total_path;
	FILE *infd;
	FILE *fd;
	int return_value = 0;

	buf[strlen(buf) - 1] = '\0';
	buf[strlen(buf) - 1] = '\0';

	total_path =
	    (char *)calloc((strlen(relative_path) + strlen(buf)), sizeof(char));
	if (total_path == NULL)
		return 12;
	strcpy(total_path, relative_path);
	strcat(total_path, buf + 1);
	infd = fopen(total_path, "r");
	if (infd) {
		return_value = read_file(map, infd, outfd, directory_list,
					 directory_list_size, relative_path);
		fclose(infd);
		free(total_path);
		return return_value;
	}
	free(total_path);
	fd = check_if_file_in_dir(directory_list, directory_list_size, buf + 1);
	if (fd) {
		return_value = read_file(map, fd, outfd, directory_list,
					 directory_list_size, relative_path);
		fclose(fd);
		return return_value;
	}
	return 12;
}

int read_file(map *map, FILE *infd, FILE *outfd, char **directory_list,
	      int directory_list_size, char *relative_path)
{
	char buf[256];
	int return_value = 0;
	int allowed_to_interpret = 1;

	while (fgets(buf, 256, infd) && return_value == 0) {
		if (allowed_to_interpret == 1 && return_value == 0) {
			if (strncmp(buf, "#define", 7) == 0)
				return_value =
				    insert_define_from_file(map, buf + 8, infd);
			else if (strncmp(buf, "#undef", 6) == 0)
				undefine_key(map, buf + 7);
			else if (strncmp(buf, "#ifdef", 6) == 0)
				allowed_to_interpret =
				    check_if_defined(map, buf + 7);
			else if (strncmp(buf, "#ifndef", 7) == 0)
				allowed_to_interpret =
				    (check_if_defined(map, buf + 8) + 1) % 2;
			else if (strncmp(buf, "#if", 3) == 0)
				allowed_to_interpret =
				    evaluate_if_condition(map, buf + 4);
			else if (strncmp(buf, "#else", 5) == 0)
				allowed_to_interpret = 0;
			else if (strncmp(buf, "#include", 8) == 0)
				return_value = add_header_file(
				    buf + 9, directory_list, outfd, map,
				    directory_list_size, relative_path);
			else if (strncmp(buf, "#endif", 6) == 0)
				;
			else if (strlen(buf) == 1 && buf[0] == '\n')
				;
			else
				return_value =
				    analyze_and_print(map, buf, outfd);
		} else if (strncmp(buf, "#else", 5) == 0)
			allowed_to_interpret = 1;
		else if (strncmp(buf, "#elif", 5) == 0)
			allowed_to_interpret =
			    evaluate_if_condition(map, buf + 6);
		else if (strncmp(buf, "#endif", 6) == 0)
			allowed_to_interpret = 1;
	}
	return return_value;
}

int add_directory_path(char ***list, int *capacity, int *size, char *path)
{
	if ((*capacity) == 0) {
		*capacity = 10;
		(*list) = (char **)malloc((*capacity) * sizeof(char *));
		if ((*list) == NULL)
			return 12;
	} else if (*size == *capacity) {
		*capacity += 10;
		(*list) = (char **)realloc(*list, (*capacity) * sizeof(char *));
		if ((*list) == NULL)
			return 12;
	}
	(*list)[*size] = (char *)malloc((strlen(path) + 1) * sizeof(char));
	if ((*list)[*size] == NULL) {
		free((*list));
		return 12;
	}
	strcpy((*list)[*size], path);
	(*size)++;

	return 0;
}

int get_relative_path(char *buf, char **relative_path)
{
	int k = strlen(buf);

	k--;
	for (; k >= 0 && buf[k] != '/'; k--)
		;
	if (k > 0) {
		(*relative_path) = (char *)calloc((k + 2), sizeof(char));
		if ((*relative_path) == NULL)
			return 12;
		strncpy((*relative_path), buf, k + 1);
		strcat((*relative_path), "\0");
	}
	return 0;
}

int copy_file_name(char **infile_name, char *buf)
{
	(*infile_name) = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	if ((*infile_name) == NULL)
		return 12;
	strcpy((*infile_name), buf);
	return 0;
}

void free_directory_list(char **directory_list, int size)
{
	if (size != 0) {
		int i;

		for (i = 0; i < size; i++)
			free(directory_list[i]);
		free(directory_list);
	}
}

int open_file(char *name, FILE **fd, char *mode)
{
	if (name != NULL) {
		*fd = fopen(name, mode);
		if (*fd == NULL)
			return -1;
	}
	return 0;
}

int close_file(char *name, FILE *fd, char *relative_path)
{
	if (name != NULL) {
		if (relative_path != NULL)
			free(relative_path);
		free(name);
		fclose(fd);
	}
	return 0;
}
