// Hash table structure: create with ht_create, free with ht_destroy.
#include <stdio.h>
typedef struct ht_entry
{
	char *key; // key is NULL if this slot is empty
	char *value;
} ht_entry;

// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct ht
{
	ht_entry *entries; // hash slots
	size_t capacity;   // size of _entries array
	size_t length;	   // number of items in hash table
} ht;

// Create hash table and return pointer to it, or NULL if out of memory.
int ht_create(ht **table);

// Free memory allocated for hash table, including allocated keys.
void ht_destroy(ht *table);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
char *ht_get(ht *table, char *key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.
int ht_set(ht *table, char *key, char *value);

int delete_entry(ht *table, char *key);

int macro_defined(ht *table, char *key);
