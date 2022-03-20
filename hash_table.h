/* These are the structures that I used for the map data structure */
typedef struct entry {
	char *key;
	char *value;
} entry;

typedef struct map {
	entry *entries;
	unsigned long capacity;
	unsigned long length;
} map;

/******************************************
 *   ALL THE BRIEF EXPLANATIONS OF EACH   *
 * FUNCTION ARE IN THE HASH_TABLE.C FILE. *
 ******************************************/

int map_create(map **mp);

void map_destroy(map *mp);

char *map_get(map *mp, char *key);

int map_set(map *mp, char *key, char *value);

int delete_entry(map *mp, char *key);

int key_exists(map *mp, char *key);
