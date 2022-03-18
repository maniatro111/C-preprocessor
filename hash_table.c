// Simple hash table implemented in C.
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

// Hash table entry (slot may be filled or empty).

#define INITIAL_CAPACITY 2 // must not be zero

int ht_create(ht **table)
{
	// Allocate space for hash table struct.
	(*table) = (ht *)malloc(sizeof(ht));
	if ((*table) == NULL)
		return 12;
	(*table)->length = 0;
	(*table)->capacity = INITIAL_CAPACITY;

	// Allocate (zero'd) space for entry buckets.
	(*table)->entries = (ht_entry *)calloc((*table)->capacity, sizeof(ht_entry));
	if ((*table)->entries == NULL)
	{
		free((*table)); // error, free table before we return!
		return 12;
	}
	return 0;
}

void ht_destroy(ht *table)
{
	// First free allocated keys.
	size_t i;

	for (i = 0; i < table->capacity; i++)
		if (table->entries[i].key != NULL)
		{
			free(table->entries[i].key);
			free(table->entries[i].value);
		}

	// Then free entries array and table itself.
	free(table->entries);
	free(table);
}

#define FNV_OFFSET 2166136261UL
#define FNV_PRIME 16777619UL

static unsigned int hash_key(char *key)
{
	unsigned int hash = FNV_OFFSET;
	char *p;

	for (p = key; *p; p++)
	{
		hash ^= (unsigned int)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}
	return hash;
}

int delete_entry(ht *table, char *key)
{
	unsigned int hash = hash_key(key);
	size_t index = (size_t)(hash & (unsigned int)(table->capacity - 1));

	if (table->entries[index].key != NULL &&
		strcmp(table->entries[index].key, key) == 0)
	{
		free(table->entries[index].key);
		free(table->entries[index].value);
		table->entries[index].key = NULL;
	}
	return 0;
}

char *ht_get(ht *table, char *key)
{
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned int hash = hash_key(key);
	size_t index = (size_t)(hash & (unsigned int)(table->capacity - 1));

	// Loop till we find an empty entry.
	while (table->entries[index].key != NULL)
	{
		if (strcmp(key, table->entries[index].key) == 0)
		{
			// Found key, return value.
			return table->entries[index].value;
		}
		// Key wasn't in this slot, move to next (linear probing).
		index++;
		if (index >= table->capacity)
		{
			// At end of entries array, wrap around.
			index = 0;
		}
	}
	return NULL;
}

int macro_defined(ht *table, char *key)
{
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned int hash = hash_key(key);
	size_t index = (size_t)(hash & (unsigned int)(table->capacity - 1));

	// Loop till we find an empty entry.
	while (table->entries[index].key != NULL)
	{
		if (strcmp(key, table->entries[index].key) == 0)
			// Found key, return value.
			return 1;
		// Key wasn't in this slot, move to next (linear probing).
		index++;
		if (index >= table->capacity)
			// At end of entries array, wrap around.
			index = 0;
	}
	return 0;
}

static int ht_set_entry(ht_entry *entries, size_t capacity, char *key, char *value, size_t *plength)
{
	// AND hash with capacity-1 to ensure it's within entries array.
	unsigned int hash = hash_key(key);
	size_t index = (size_t)(hash & (unsigned int)(capacity - 1));

	// Loop till we find an empty entry.
	while (entries[index].key != NULL)
	{
		if (strcmp(key, entries[index].key) == 0)
			return -1;
		// Key wasn't in this slot, move to next (linear probing).
		index++;
		if (index >= capacity)
			// At end of entries array, wrap around.
			index = 0;
	}

	// Didn't find key, allocate+copy if needed, then insert it.
	if (plength != NULL)
	{
		if (key == NULL)
			return -1;
		entries[index].key = (char *)malloc((strlen(key) + 1) * sizeof(char));
		if (entries[index].key == NULL)
			return 12;
		strcpy(entries[index].key, key);
		(*plength)++;
	}
	entries[index].value = (char *)malloc((strlen(value) + 1) * sizeof(char));
	if (entries[index].value == NULL)
		return 12;
	strcpy(entries[index].value, value);
	return 0;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static int ht_expand(ht *table)
{
	// Allocate new entries array.
	size_t i;
	ht_entry *new_entries;
	size_t new_capacity = table->capacity * 2;

	if (new_capacity < table->capacity)
		return 0; // overflow (capacity would be too big)
	new_entries = calloc(new_capacity, sizeof(ht_entry));
	if (new_entries == NULL)
		return 0;

	table->length = 0;
	// Iterate entries, move all non-empty ones to new table's entries.
	for (i = 0; i < table->capacity; i++)
	{
		ht_entry entry = table->entries[i];
		if (entry.key != NULL)
		{
			ht_set_entry(new_entries, new_capacity, entry.key, entry.value, &table->length);
			free(table->entries[i].key);
			free(table->entries[i].value);
		}
	}

	// Free old entries array and update this table's details.
	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;
	return 1;
}

int ht_set(ht *table, char *key, char *value)
{
	if (value == NULL)
		return 1;

	// If length will exceed half of current capacity, expand it.
	if (table->length >= table->capacity / 2)
		if (!ht_expand(table))
			return 12;
	//  Set entry and update length.
	return ht_set_entry(table->entries, table->capacity, key, value, &table->length);
}
