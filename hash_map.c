#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_map.h"
#endif

#define INITIAL_CAPACITY 2

/**
 * @brief
 * This is the function that allocates memory for the
 * creation of the map data structure.
 * @param map - reference to the memory zone of the map
 * @return 0 - if the creation is a success
 *         12 - in case on of the mallocs fail
 */
int map_create(map **mp)
{
	/* Allocate memory for the map data structure */
	(*mp) = (map *)malloc(sizeof(map));
	if ((*mp) == NULL)
		return 12;
	/* Set the number of entries to 0*/
	(*mp)->length = 0;
	/* Set the initial capacity to the value of the macro*/
	(*mp)->capacity = INITIAL_CAPACITY;
	/* Allocate memory for the entry array */
	(*mp)->entries = (entry *)calloc((*mp)->capacity, sizeof(entry));
	if ((*mp)->entries == NULL) {
		free((*mp));
		return 12;
	}
	return 0;
}

/**
 * @brief this function frees all the memory allocated for
 * the map data structure
 * @param mp - reference to the memory zone of the map
 */
void map_destroy(map *mp)
{
	unsigned long i;

	/* Iterate through the entries array */
	for (i = 0; i < mp->capacity; i++)
		/* If the entry is not NULL */
		if (mp->entries[i].key != NULL) {
			/* Free the allocated memory for that entry */
			free(mp->entries[i].key);
			free(mp->entries[i].value);
		}
	/* Free the entries array */
	free(mp->entries);
	/* Free the map */
	free(mp);
}

/**
 * @brief This is the hash function that I
 * used for the map insertion.
 * Found it here:
 * https://stackoverflow.com/questions/7666509/hash-function-for-string
 *
 * @param str - The key of the entry. Here it is used as an argument to
 * calculate the hash of the entry
 * @return the hash of the entry
 */
static unsigned long hash_function(char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = (unsigned char)*str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

/**
 * @brief This function deletes the entry with the
 * key given as argument
 *
 * @param mp - the map data structure which has the key
 * @param key - the key of the map entry that we need to delete
 * @return 0 - if we found the key and managed to erase it
 *         -1 - otherwise
 */
int delete_entry(map *mp, char *key)
{
	/* Calculate the index of the key */
	unsigned long index = hash_function(key) % mp->capacity;
	/* Iterate till we find the entry with the key given as argument */
	/* or till we find an empty entry */
	if (mp->entries[index].key != NULL &&
	    strcmp(mp->entries[index].key, key) == 0) {
		/* Free the key and value and make them NULL*/
		/* http://tiny.cc/Link-to-discussion
		 */

		free(mp->entries[index].key);
		free(mp->entries[index].value);
		mp->entries[index].key = NULL;
		mp->entries[index].value = NULL;
		return 0;
	}
	return -1;
}

/**
 * @brief This function returns the value corresponding
 * to the key given as argument
 * @param mp - the map in which we search
 * @param key - the key that we want to see the value of
 * @return the value located at key
 *         NULL if the key cannot be found
 */
char *map_get(map *mp, char *key)
{
	/* Calculate the index of the key */
	unsigned long index = hash_function(key) % mp->capacity;

	/* Search for the entry */
	while (mp->entries[index].key != NULL) {
		if (strcmp(key, mp->entries[index].key) == 0)
			/* Return the value located at the key */
			return mp->entries[index].value;
		index = (index + 1) % mp->capacity;
	}
	/* Return NULL if the entry does not exist */
	return NULL;
}

/**
 * @brief This function checks if the map has a key
 * given as argument
 *
 * @param mp - the map in which we search
 * @param key - the key we are searching for
 * @return 1 - if the key is found
 *         0 - if the key is not found
 */
int key_exists(map *table, char *key)
{
	/* Calculate the index of the key */
	unsigned long index = hash_function(key) % table->capacity;

	/* Search for the entry with that key */
	while (table->entries[index].key != NULL) {
		if (strcmp(key, table->entries[index].key) == 0)
			/* Return 1 if the key is defined */
			return 1;
		index = (index + 1) % table->capacity;
	}
	/* Return 0 if the key does not exist */
	return 0;
}

/**
 * @brief This function adds a new entry to the map
 *
 * @param entries - the entries array of the map
 * @param capacity - the maximum capacity of the entries array
 * @param key - the key of the new entry
 * @param value - the value of the new entry
 * @param plength - the number of entries in the map
 * @return 0 - if the insertion succeeds or there is already another entry with
 *             that key in the map
 *         12 - if one of the malloc fails
 *         -1 - if the key is NULL
 */
static int map_set_entry(entry *entries, unsigned long capacity, char *key,
			 char *value, unsigned long *plength)
{
	/* Calculate the index of the key */
	unsigned long index = hash_function(key) % capacity;

	/* Search if an entry with the key given as argument already exists */
	while (entries[index].key != NULL) {
		if (strcmp(key, entries[index].key) == 0)
			return 0;
		index = (index + 1) % capacity;
	}

	/* If the key is NULL, return -1 */
	if (key == NULL)
		return -1;
	/* Else, allocate memory for the key field and copy the key in it*/
	entries[index].key = (char *)malloc((strlen(key) + 1) * sizeof(char));
	if (entries[index].key == NULL)
		return 12;
	strcpy(entries[index].key, key);
	/* Increase the number of entries in the map */
	(*plength)++;
	/* Then, allocate memory for the value field and copy the value in it */
	entries[index].value =
	    (char *)malloc((strlen(value) + 1) * sizeof(char));
	if (entries[index].value == NULL)
		return 12;
	strcpy(entries[index].value, value);
	/* Return 0 on success */
	return 0;
}

/**
 * @brief This function expands the size of the map
 *
 * @param mp - the map that we want to expand
 * @return 0 - if the expand fails
 *         1 - if the expand succeeds
 */
static int map_expand(map *mp)
{
	unsigned long i;
	entry *new_entries;
	unsigned long new_capacity = mp->capacity * 2;
	int return_value = 1;

	/* if the new capacitty is smaller than the previous one, */
	/* return 0 because of overflow */
	if (new_capacity < mp->capacity)
		return 0;
	/* Else, allocate memory for a new entries array */
	new_entries = calloc(new_capacity, sizeof(entry));
	if (new_entries == NULL)
		return 0;

	/* Reset the number of entries to 0 */
	mp->length = 0;
	/* Copy all the entries from the old array to the new one */
	for (i = 0; i < mp->capacity && return_value == 1; i++)
		if (mp->entries[i].key != NULL) {
			int aux;

			aux = map_set_entry(new_entries, new_capacity,
					    mp->entries[i].key,
					    mp->entries[i].value, &mp->length);
			if (aux == 12)
				return_value = 0;
			free(mp->entries[i].key);
			free(mp->entries[i].value);
		}
	/* Free the old entries array */
	free(mp->entries);
	/* Make the entries field of the map point to the new entries array */
	mp->entries = new_entries;
	/* Modify the capacity of the map with the new one */
	mp->capacity = new_capacity;
	return return_value;
}

/**
 * @brief The function that is used to add a new entry to the map
 * that also checks if the map should be expanded
 *
 * @param mp - the map in which we want to add the entry
 * @param key - the key of the new entry
 * @param value - the value of the entry
 * @return 1 - if the value is NULL
 *         -1 - if the key is NULL
 *         12 - if the expansion fails or one of the malloc fails
 *         0 - if the insertion succeeds or there is already another entry with
 *             that key in the map
 */
int map_set(map *mp, char *key, char *value)
{
	/* If the value is NULL, return -1 */
	if (value == NULL)
		return -1;
	/* If the number of entries in the map is greater than half of the*/
	/* map capacity, try to expand the map */
	if (mp->length >= mp->capacity / 2)
		if (map_expand(mp) == 0)
			return 12;
	/*  Try to add the new entry */
	return map_set_entry(mp->entries, mp->capacity, key, value,
			     &mp->length);
}
