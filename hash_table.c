#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

#define INITIAL_CAPACITY 2

int map_create(map **table)
{
	(*table) = (map *)malloc(sizeof(map));
	if ((*table) == NULL)
		return 12;
	(*table)->length = 0;
	(*table)->capacity = INITIAL_CAPACITY;
	(*table)->entries =
	    (map_entry *)calloc((*table)->capacity, sizeof(map_entry));
	if ((*table)->entries == NULL) {
		free((*table));
		return 12;
	}
	return 0;
}

void map_destroy(map *table)
{
	unsigned long i;

	for (i = 0; i < table->capacity; i++)
		if (table->entries[i].key != NULL) {
			free(table->entries[i].key);
			free(table->entries[i].value);
		}
	free(table->entries);
	free(table);
}

static unsigned long hash_function(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

int delete_entry(map *table, char *key)
{
	unsigned long index = hash_function(key) % table->capacity;

	if (table->entries[index].key != NULL &&
	    strcmp(table->entries[index].key, key) == 0) {
		free(table->entries[index].key);
		free(table->entries[index].value);
		table->entries[index].key = NULL;
	}
	return 0;
}

char *map_get(map *table, char *key)
{
	unsigned long index = hash_function(key) % table->capacity;

	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			return table->entries[index].value;
		index = (index + 1) % table->capacity;
	}
	return NULL;
}

int macro_defined(map *table, char *key)
{
	unsigned long index = hash_function(key) % table->capacity;

	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			return 1;
		index = (index + 1) % table->capacity;
	}
	return 0;
}

static int map_set_entry(map_entry *entries, unsigned long capacity, char *key,
			 char *value, unsigned long *plength)
{
	unsigned long index = hash_function(key) % capacity;

	while (entries[index].key != NULL) {
		if (strcmp(key, entries[index].key) == 0)
			return 0;
		index = (index + 1) % capacity;
	}
	if (key == NULL)
		return -1;
	entries[index].key = (char *)malloc((strlen(key) + 1) * sizeof(char));
	if (entries[index].key == NULL)
		return 12;
	strcpy(entries[index].key, key);
	(*plength)++;
	entries[index].value =
	    (char *)malloc((strlen(value) + 1) * sizeof(char));
	if (entries[index].value == NULL)
		return 12;
	strcpy(entries[index].value, value);
	return 0;
}

static int map_expand(map *table)
{
	unsigned long i;
	map_entry *new_entries;
	unsigned long new_capacity = table->capacity * 2;
	int return_value = 1;

	if (new_capacity < table->capacity)
		return 0;
	new_entries = calloc(new_capacity, sizeof(map_entry));
	if (new_entries == NULL)
		return 0;

	table->length = 0;
	for (i = 0; i < table->capacity && return_value == 1; i++) {
		map_entry entry = table->entries[i];
		if (entry.key != NULL) {
			int aux;

			aux =
			    map_set_entry(new_entries, new_capacity, entry.key,
					  entry.value, &table->length);
			if (aux == 12)
				return_value = 0;
			free(table->entries[i].key);
			free(table->entries[i].value);
		}
	}
	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;
	return return_value;
}

int map_set(map *table, char *key, char *value)
{
	if (value == NULL)
		return 1;
	if (table->length >= table->capacity / 2)
		if (!map_expand(table))
			return 12;
	return map_set_entry(table->entries, table->capacity, key, value,
			     &table->length);
}
