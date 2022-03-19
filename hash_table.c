// Simple hash table implemented in C.
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

// Hash table entry (slot may be filled or empty).

#define INITIAL_CAPACITY 2 // must not be zero

int map_create(map **table)
{
	// Allocate space for hash table struct.
	(*table) = (map *)malloc(sizeof(map));
	if ((*table) == NULL)
		return 12;
	(*table)->length = 0;
	(*table)->capacity = INITIAL_CAPACITY;

	// Allocate (zero'd) space for entry buckets.
	(*table)->entries =
	    (map_entry *)calloc((*table)->capacity, sizeof(map_entry));
	if ((*table)->entries == NULL) {
		free((*table)); // error, free table before we return!
		return 12;
	}
	return 0;
}

void map_destroy(map *table)
{
	// First free allocated keys.
	unsigned long i;

	for (i = 0; i < table->capacity; i++)
		if (table->entries[i].key != NULL) {
			free(table->entries[i].key);
			free(table->entries[i].value);
		}

	// Then free entries array and table itself.
	free(table->entries);
	free(table);
}

#define FNV_OFFSET 2166136261UL
#define FNV_PRIME 16777619UL

static unsigned long hash_key(char *key)
{
	unsigned long hash = FNV_OFFSET;
	char *p;

	for (p = key; *p; p++) {
		hash ^= (unsigned long)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}
	return hash;
}

int delete_entry(map *table, char *key)
{
	unsigned long hash = hash_key(key);
	unsigned long index =
	    (unsigned long)(hash & (unsigned long)(table->capacity - 1));

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
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned long hash = hash_key(key);
	unsigned long index =
	    (unsigned long)(hash & (unsigned long)(table->capacity - 1));

	// Loop till we find an empty entry.
	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			// Found key, return value.
			return table->entries[index].value;
		// Key wasn't in this slot, move to next (linear probing).
		index = (index + 1) % table->capacity;
	}
	return NULL;
}

int macro_defined(map *table, char *key)
{
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned long hash = hash_key(key);
	unsigned long index =
	    (unsigned long)(hash & (unsigned long)(table->capacity - 1));

	// Loop till we find an empty entry.
	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			// Found key, return value.
			return 1;
		// Key wasn't in this slot, move to next (linear probing).
		index = (index + 1) % table->capacity;
	}
	return 0;
}

static int map_set_entry(map_entry *entries, unsigned long capacity, char *key,
			 char *value, unsigned long *plength)
{
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned long hash = hash_key(key);
	unsigned long index =
	    (unsigned long)(hash & (unsigned long)(capacity - 1));

	// Loop till we find an empty entry.
	while (entries[index].key != NULL) {
		if (strcmp(key, entries[index].key) == 0)
			return 0;
		// Key wasn't in this slot, move to next (linear probing).
		index = (index + 1) % capacity;
	}

	// Didn't find key, allocate+copy if needed, then insert it.
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

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static int map_expand(map *table)
{
	// Allocate new entries array.
	unsigned long i;
	map_entry *new_entries;
	unsigned long new_capacity = table->capacity * 2;
	int return_value = 1;

	if (new_capacity < table->capacity)
		return 0; // overflow (capacity would be too big)
	new_entries = calloc(new_capacity, sizeof(map_entry));
	if (new_entries == NULL)
		return 0;

	table->length = 0;
	// Iterate entries, move all non-empty ones to new table's entries.
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

	// Free old entries array and update this table's details.
	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;
	return return_value;
}

int map_set(map *table, char *key, char *value)
{
	if (value == NULL)
		return 1;

	// If length will exceed half of current capacity, expand it.
	if (table->length >= table->capacity / 2)
		if (!map_expand(table))
			return 12;
	//  Set entry and update length.
	return map_set_entry(table->entries, table->capacity, key, value,
			     &table->length);
}
