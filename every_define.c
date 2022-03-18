#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "every_define.h"

int check_not_in_between(int pos, int nr_el, int *vec)
{
	int i;

	if (!nr_el)
		return 1;

	for (i = 0; i < nr_el; i += 2)
	{
		if (pos > vec[i] && pos < vec[i + 1])
			return 0;
	}
	return 1;
}

void get_apostrophes(char *string, int **vect, int *el_number)
{
	*el_number = 0;
	if (strchr(string, '\"'))
	{
		int initial_size;

		initial_size = 10;
		*vect = (int *)malloc(initial_size * sizeof(int));
		(*vect)[*el_number] = strchr(string, '\"') - string;
		(*el_number)++;
		while (1 + (*vect)[(*el_number) - 1] < strlen(string) && strchr(string + 1 + (*vect)[(*el_number) - 1], '\"'))
		{
			(*vect)[*el_number] = strchr(string + 1 + (*vect)[(*el_number) - 1], '\"') - string;
			(*el_number)++;
		}
	}
}

static void check_define_in_define(ht *tabel, char *value)
{
	int j;

	for (j = 0; j < tabel->capacity; j++)
		if (tabel->entries[j].key != NULL)
		{
			if (strstr(value, tabel->entries[j].key) != NULL)
			{
				char *aux;
				int i;

				i = strstr(value, tabel->entries[j].key) -
					value;
				aux = (char *)malloc((strlen(value) + 1) *
									 sizeof(char));
				strcpy(aux, value);
				strncpy(value, aux, i);
				strcpy(value + i, tabel->entries[j].value);
				strcpy(value + i +
						   strlen(tabel->entries[j].value),
					   aux + i + strlen(tabel->entries[j].key));
				free(aux);
				check_define_in_define(tabel, value);
			}
		}
}

void insert_define_from_file(ht *tabel, char *buf, FILE *infd)
{
	char *argumente;
	char *aux;
	char *key;

	// printf("%s", buf);
	argumente = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	strcpy(argumente, buf);
	aux = strtok(argumente, "\n ");
	key = (char *)malloc((strlen(aux) + 1) * sizeof(char));
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	if (aux == NULL)
	{
		ht_set(tabel, key, "");
	}
	else if (aux[strlen(aux) - 1] == '\\')
	{
		char *argumente_concatenate;
		int i;

		argumente_concatenate = (char *)calloc(250, sizeof(char));
		strcat(argumente_concatenate, aux);
		argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';
		for (i = strlen(argumente_concatenate) - 1; i >= 0 && (argumente_concatenate[i] == ' ' || argumente_concatenate[i] == '\t'); i--)
			argumente_concatenate[i] = '\0';
		fgets(buf, 256, infd);
		buf[strlen(buf) - 1] = '\0';
		while (buf[strlen(buf) - 1] == '\\')
		{
			int i;

			for (i = 0; i < strlen(buf) && (buf[i] == ' ' || buf[i] == '\t'); i++)
				;
			strcat(argumente_concatenate, " ");
			strcat(argumente_concatenate, buf + i);
			argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';

			for (i = strlen(argumente_concatenate); i >= 1 && (argumente_concatenate[i] == ' ' && argumente_concatenate[i - 1] == ' '); i--)
				argumente_concatenate[i] = '\0';
			fgets(buf, 256, infd);
			buf[strlen(buf) - 1] = '\0';
		}
		for (i = 0; i < strlen(buf) && (buf[i] == ' ' || buf[i] == '\t'); i++)
			;
		strcat(argumente_concatenate, " ");
		strcat(argumente_concatenate, buf + i);
		check_define_in_define(tabel, aux);
		ht_set(tabel, key, argumente_concatenate);
		free(argumente_concatenate);
	}
	else
	{
		check_define_in_define(tabel, aux);
		ht_set(tabel, key, aux);
	}
	free(key);
	free(argumente);
}

static void check_and_copy(int pos, int dimension, int *v, char *buf, ht *map, int map_index)
{
	if (check_not_in_between(pos, dimension, v))
	{
		char *aux;

		aux = (char *)malloc(256 * sizeof(char));
		strcpy(aux, buf);
		strncpy(buf, aux, pos);
		strcpy(buf + pos, map->entries[map_index].value);
		strcpy(buf + pos + strlen(map->entries[map_index].value), aux + pos + strlen(map->entries[map_index].key));
		free(aux);
	}
}

void analyze_and_print(ht *map, char *buf, FILE *outfd)
{
	int *v;
	int n;
	int j;

	get_apostrophes(buf, &v, &n);

	for (j = 0; j < map->capacity; j++)
		if (map->entries[j].key != NULL && strstr(buf, map->entries[j].key))
		{
			int pos = strstr(buf, map->entries[j].key) - buf;
			check_and_copy(pos, n, v, buf, map, j);
			while (strstr(buf + pos + strlen(map->entries[j].key), map->entries[j].key))
			{
				pos = strstr(buf + pos + strlen(map->entries[j].key), map->entries[j].key) - buf;
				check_and_copy(pos, n, v, buf, map, j);
			}
		}
	if (buf[0] == '\t')
		buf[0] = ' ';
	fprintf(outfd, "%s", buf);
	if (n)
		free(v);
}

void add_argument_mapping(char **argv, int *line, ht *map)
{
	if (strlen(argv[*line]) > 2)
		argv[*line] = argv[*line] + 2;
	else
		(*line)++;

	if (strchr(argv[*line], '=') != NULL)
	{
		char *aux;

		aux = strtok(argv[*line], "=");
		ht_set(map, argv[*line], argv[*line] + strlen(argv[*line]) + 1);
	}
	else
	{
		ht_set(map, argv[*line], "");
	}
}

int undefine_key(ht *map, char *key)
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

int evaluate_if_condition(ht *map, char *key)
{
	char *aux;
	key[strlen(key) - 1] = '\0';
	aux = ht_get(map, key);
	if (aux == NULL)
		return turn_to_int_and_check(key);
	else
	{
		strcpy(key, aux);
		return turn_to_int_and_check(key);
	}
}

int check_if_defined(ht *map, char *key)
{
	key[strlen(key) - 1] = '\0';
	return macro_defined(map, key);
}

static FILE *check_if_file_in_dir(char **directory_list, int directory_list_size, char *file)
{
	int i;
	for (i = 0; i < directory_list_size; i++)
	{
		char *path;
		FILE *fd;
		path = (char *)calloc((strlen(directory_list[i]) + strlen(file) + 2), sizeof(char));
		strcpy(path, directory_list[i]);
		strcat(path, "/");
		strcat(path, file);
		strcat(path, "\0");
		// printf("%s\n", path);*/
		fd = fopen(path, "r");
		if (fd)
		{
			free(path);
			return fd;
		}
		else
		{
			free(path);
		}
	}
	return NULL;
}

int add_header_file(char *buf, char **directory_list, FILE *outfd, ht *map, int directory_list_size, char *relative_path)
{
	int return_value;
	char *total_path;
	return_value = 0;
	buf[strlen(buf) - 1] = '\0';
	buf[strlen(buf) - 1] = '\0';
	FILE *infd;
	total_path = (char *)calloc((strlen(relative_path) + strlen(buf)), sizeof(char));
	strcpy(total_path, relative_path);
	strcat(total_path, buf + 1);
	infd = fopen(total_path, "r");
	if (infd)
	{
		return_value = read_file(map, infd, outfd, directory_list, directory_list_size, relative_path);
		fclose(infd);
		free(total_path);
		return return_value;
	}
	else
	{
		free(total_path);
		FILE *fd = check_if_file_in_dir(directory_list, directory_list_size, buf + 1);
		if (fd)
		{
			return_value = read_file(map, fd, outfd, directory_list, directory_list_size, relative_path);
			fclose(fd);
			return return_value;
		}
		else
			return -1;
	}
}

int read_file(ht *map, FILE *infd, FILE *outfd, char **directory_list, int directory_list_size, char *relative_path)
{
	char buf[256];
	int return_value;
	int allowed_to_interpret;
	allowed_to_interpret = 1;
	return_value = 0;
	while (fgets(buf, 256, infd))
	{
		if (allowed_to_interpret == 1 && return_value == 0)
		{
			if (strncmp(buf, "#define", 7) == 0)
				insert_define_from_file(map, buf + 8, infd);
			else if (strncmp(buf, "#undef", 6) == 0)
				undefine_key(map, buf + 7);
			else if (strncmp(buf, "#ifdef", 6) == 0)
				allowed_to_interpret = check_if_defined(map, buf + 7);
			else if (strncmp(buf, "#ifndef", 7) == 0)
				allowed_to_interpret = (check_if_defined(map, buf + 8) + 1) % 2;
			else if (strncmp(buf, "#if", 3) == 0)
				allowed_to_interpret = evaluate_if_condition(map, buf + 4);
			else if (strncmp(buf, "#else", 5) == 0)
				allowed_to_interpret = 0;
			else if (strncmp(buf, "#include", 8) == 0)
				return_value = add_header_file(buf + 9, directory_list, outfd, map, directory_list_size, relative_path);
			else if (strncmp(buf, "#endif", 6) == 0)
				;
			else if (strlen(buf) == 1 && buf[0] == '\n')
				;
			else
				analyze_and_print(map, buf, outfd);
		}
		else if (strncmp(buf, "#else", 5) == 0)
			allowed_to_interpret = 1;
		else if (strncmp(buf, "#elif", 5) == 0)
			allowed_to_interpret = evaluate_if_condition(map, buf + 6);
		else if (strncmp(buf, "#endif", 6) == 0)
			allowed_to_interpret = 1;
	}
	return return_value;
}

void add_directory_path(char ***list, int *capacity, int *size, char *path)
{
	if ((*capacity) == 0)
	{
		*capacity = 10;
		(*list) = (char **)malloc((*capacity) * sizeof(char *));
		(*list)[*size] = (char *)malloc((strlen(path) + 1) * sizeof(char));
		strcpy((*list)[*size], path);
		(*size)++;
	}
	else if (*size == *capacity)
	{
		*capacity += 10;
		(*list) = (char **)realloc(*list, (*capacity) * sizeof(char *));
		(*list)[*size] = (char *)malloc((strlen(path) + 1) * sizeof(char));
		strcpy((*list)[*size], path);
		(*size)++;
	}
	else
	{
		(*list)[*size] = (char *)malloc((strlen(path) + 1) * sizeof(char));
		strcpy((*list)[*size], path);
		(*size)++;
	}
}
