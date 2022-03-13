// Simple hash table implemented in C.
#include <stdlib.h>
#include <string.h>

// Hash table entry (slot may be filled or empty).
typedef struct {
    char* key;  // key is NULL if this slot is empty
    char* value;
} ht_entry;

// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct {
    ht_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
} ht;

#define INITIAL_CAPACITY 1000  // must not be zero

ht* ht_create() {
    // Allocate space for hash table struct.
    ht* table = (ht *)malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    // Allocate (zero'd) space for entry buckets.
    table->entries = (ht_entry *)calloc(table->capacity, sizeof(ht_entry));
    if (table->entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

void ht_destroy(ht* table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->entries[i].key != NULL) {
            free(table->entries[i].key);
            free(table->entries[i].value);
        }
    }

    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static u_int64_t hash_key(char* key) {
    u_int64_t hash = FNV_OFFSET;
    for (char* p = key; *p; p++) {
        hash ^= (u_int64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

char* ht_get(ht* table, char* key) {
    // AND hash with capacity-1 to ensure it's within entries array.
    u_int64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (u_int64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            // Found key, return value.
            return table->entries[index].value;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
}

// Internal function to set an entry (without expanding table).
static char* ht_set_entry(ht_entry* entries, size_t capacity,
        char* key, char* value, size_t* plength) {
    // AND hash with capacity-1 to ensure it's within entries array.
    u_int64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (u_int64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            //entries[index].value = value;
            strcpy(entries[index].value, value);
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        //key = strdup(key);
        entries[index].key = (char *) malloc((strlen(key) + 1) * sizeof(char));
        strcpy(entries[index].key, key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    //entries[index].key = (char*)key;
    //entries[index].value = value;
    entries[index].value = (char *) malloc((strlen(value) + 1) * sizeof(char));
    strcpy(entries[index].value, value);
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static int ht_expand(ht* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return 0;  // overflow (capacity would be too big)
    }
    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return 0;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key,
                         entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return 1;
}

char* ht_set(ht* table, char* key, char* value) {
    if (value == NULL) {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    // Set entry and update length.
    return ht_set_entry(table->entries, table->capacity, key, value,
                        &table->length);
}