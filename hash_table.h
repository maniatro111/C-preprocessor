typedef struct map_entry {
	char *key;
	char *value;
} map_entry;

typedef struct map {
	map_entry *entries;
	unsigned long capacity;
	unsigned long length;
} map;

int map_create(map **table);

void map_destroy(map *table);

char *map_get(map *table, char *key);

int map_set(map *table, char *key, char *value);

int delete_entry(map *table, char *key);

int macro_defined(map *table, char *key);
