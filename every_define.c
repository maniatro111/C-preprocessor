#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int check_not_in_between(int pos, int nr_el, int *vec)
{
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
			// printf("%s\n", tabel->entries[j].key);
			// printf("%s %s\n", tabel->entries[j].key, tabel->entries[j].value);
			//    printf("%s\n", ht_get(map, map->entries[j].key));
			if (strstr(value, tabel->entries[j].key) != NULL)
			{
				char *aux;
				int i;
				i = strstr(value, tabel->entries[j].key) - value;
				aux = (char *)malloc((strlen(value) + 1) * sizeof(char));
				strcpy(aux, value);
				// free(value);
				// value = (char *)
				strncpy(value, aux, i);
				stpcpy(value + i, tabel->entries[j].value);
				strcpy(value + i + strlen(tabel->entries[j].value), aux + i + strlen(tabel->entries[j].key));
				// printf("NICE");
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
	aux = strtok(argumente, " \n");
	key = (char *)malloc((strlen(aux) + 1) * sizeof(char));
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	if (aux[strlen(aux) - 1] == '\\')
	{
		char *argumente_concatenate;
		int i;
		argumente_concatenate = (char *)malloc(250 * sizeof(char));
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
		check_define_in_define(tabel, aux);
		ht_set(tabel, key, argumente_concatenate);
		free(argumente_concatenate);
	}
	else
	{
		// printf("%s %s\n", key, aux);
		check_define_in_define(tabel, aux);
		// printf("%s\n", aux);
		ht_set(tabel, key, aux);
	}
	free(key);
	free(argumente);
}
