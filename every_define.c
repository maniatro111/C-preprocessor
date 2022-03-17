#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_not_in_between(int pos, int nr_el, int *vec)
{
	if (!nr_el)
		return 1;
	int i;

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
				stpcpy(value + i, tabel->entries[j].value);
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

	argumente = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	strcpy(argumente, buf);
	aux = strtok(argumente, "\n ");
	key = (char *)malloc((strlen(aux) + 1) * sizeof(char));
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	if (aux[strlen(aux) - 1] == '\\')
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

	get_apostrophes(buf, &v, &n);
	int j;

	for (j = 0; j < (int)map->capacity; j++)
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
	delete_entry(map, key + 7);
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
	// printf("%s", key);
	key[strlen(key) - 1] = '\0';
	char *aux;
	aux = ht_get(map, key);
	if (aux == NULL)
		return turn_to_int_and_check(key);
	else
	{
		strcpy(key, aux);
		return turn_to_int_and_check(key);
	}
	// printf("%s\n", key);
}