// Hash table structure: create with map_create, free with map_destroy.
typedef struct map_entry {
	char *key; // key is NULL if this slot is empty
	char *value;
} map_entry;

// Hash table structure: create with map_create, free with map_destroy.
typedef struct map {
	map_entry *entries;	// hash slots
	unsigned long capacity; // size of _entries array
	unsigned long length;	// number of items in hash table
} map;

// Create hash table and return pointer to it, or NULL if out of memory.
int map_create(map **table);

// Free memory allocated for hash table, including allocated keys.
void map_destroy(map *table);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with map_set), or NULL if key not found.
char *map_get(map *table, char *key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when map_destroy is
// called). Return address of copied key, or NULL if out of memory.
int map_set(map *table, char *key, char *value);

int delete_entry(map *table, char *key);

int macro_defined(map *table, char *key);
