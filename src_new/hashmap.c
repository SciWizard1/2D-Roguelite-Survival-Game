#include "game.h"

#define INIT_HASHMAP_SIZE_EXPONENT 4

const int64_t init_hashmap_size = (1 << INIT_HASHMAP_SIZE_EXPONENT);

HashMap hmap_instantiate_new(int32_t failure_code) {
    HashMap new_hashmap;

    // Allocate space for hashmap entries.
    HashMapEntry* map_ptr = malloc(init_hashmap_size * sizeof(HashMapEntry));
    
    // Ensure allocation was successful before continuing.
    if (map_ptr == NULL) {
        (void)ERR(failure_code, "Failed to allocate hashmap.\n", ERR_ERROR);
    }

    // Initialize the hashmap memory.
    for (int64_t i = 0; i < init_hashmap_size; i++) {
        map_ptr[i] = (HashMapEntry){0, 0, false};
    }
    
    // Initialize necessary values.
    new_hashmap.size_exponent = INIT_HASHMAP_SIZE_EXPONENT;
    new_hashmap.num_entries   = 0;
    new_hashmap.map           = map_ptr;

    return new_hashmap;
}

int64_t hmap_hash_function(int64_t key) {
    key += 0x9e3779b97f4a7c15ULL;
    key = (key ^ (key >> 30)) * 0xbf58476d1ce4e5b9ULL;
    key = (key ^ (key >> 27)) * 0x94d049bb133111ebULL;
    key = key ^ (key >> 31);
    return key;
}

int64_t hmap_find_next_available_slot(
    HashMapEntry *map, 
    int64_t key, 
    int64_t map_index, 
    int64_t size_exponent
) {
    const int64_t hashmap_size = 1 << size_exponent;
    

    // Find a free slot.
    for (int total_traversed = 0; total_traversed < hashmap_size; map_index++, total_traversed++) {
    
        map_index &= hashmap_size - 1;

        if (
            map[map_index].is_used == false || 
            map[map_index].key == key
        ) {
            return map_index;
        }
    }

    // Return -1 if there are no more free slots.
    return -1;
}

int64_t hmap_find_key(
    HashMap *hashmap, 
    int64_t key, 
    int64_t map_index
) {
    const int64_t hashmap_size = 1 << hashmap->size_exponent;
    
    // Find a matching key.
    for (int total_traversed = 0; total_traversed < hashmap_size; map_index++, total_traversed++) {

        map_index &= hashmap_size - 1;

        if (hashmap->map[map_index].key == key) {
            return map_index;
        }

    }

    // Return -1 if the key wasn't found.
    return -1;
}

/*
    This function is specifically for inserting elements into the map itself, 
    and shouldn't use the full HashMap struct.
*/

void hmap_insert_entry_raw(
    HashMapEntry *map, 
    int64_t key, 
    int64_t reference, 
    int64_t size_exponent, 
    int32_t failure_code
) {
    // Hash the key.
    int64_t hashed_key = hmap_hash_function(key);

    // Subtraction of a 1 with leading zeros results in all of the leading zeros becoming 1.
    const int64_t hash_mask = (1 << size_exponent) - 1;

    // Restrict the hash domain to the size of the hashmap.
    int64_t map_index = hashed_key & hash_mask;

    // Check for a collision:
    if (map[map_index].is_used) {
        map_index = hmap_find_next_available_slot(map, key, map_index, size_exponent);
    }

    if (map_index == -1) {
        // -1 means that the hashmap is full.
        (void)ERR(failure_code, "Hashmap is full!\n", ERR_ERROR);
        return;
    }

    // Insert the data.
    map[map_index] = (HashMapEntry){key, reference, true};
    
}

int hmap_expand(
    HashMap *hashmap, 
    int32_t failure_code
) {

    int64_t hashmap_old_size = (1 << hashmap->size_exponent);
    int64_t hashmap_new_size = hashmap_old_size * 2;

    // Allocate a new buffer for the hashmap that is double the size.
    HashMapEntry* new_map_ptr = malloc(hashmap_new_size * sizeof(HashMapEntry));

    // Check if allocation was successful.
    if (new_map_ptr == NULL) {
        (void)ERR(failure_code, "Failed to allocate memory to expand hashmap.\n", ERR_ERROR);
        return -1;
    }

    // Initialize memory.
    for (int64_t i = 0; i < hashmap_new_size; i++) {
        new_map_ptr[i] = (HashMapEntry){0, 0, false};
    }

    hashmap->size_exponent += 1;

    // Rehash all entries and populate the new buffer.
    for (int64_t i = 0; i < hashmap_old_size; i++) {
        HashMapEntry entry = hashmap->map[i];

        if (entry.is_used) {
            hmap_insert_entry_raw(
                new_map_ptr, 
                entry.key,
                entry.reference,
                hashmap->size_exponent,
                failure_code
            );
        }
    }

    // Free the old hashmap buffer.
    free(hashmap->map);

    // Assign the hashmap object with the new buffer.
    hashmap->map = new_map_ptr;


    return 0;
}

void hmap_insert_entry(
    HashMap *hashmap, 
    int64_t key, 
    int64_t reference, 
    int32_t failure_code
) {
    
    // Expand the hashmap if it is at least 50% full.
    if (2 * hashmap->num_entries >= (1 << hashmap->size_exponent)) {
        hmap_expand(hashmap, failure_code);
    }

    hmap_insert_entry_raw(hashmap->map, key, reference, hashmap->size_exponent, failure_code);

    hashmap->num_entries++;
}

int64_t hmap_get_entry(
    HashMap *hashmap, 
    int64_t key
) {

    // Calculate the hashmap index.
    int64_t hashed_key = hmap_hash_function(key);
    int64_t hash_mask  = (1 << hashmap->size_exponent) - 1;
    int64_t map_index  = hashed_key & hash_mask;

    // Handle collisions before accessing.
    map_index = hmap_find_key(hashmap, key, map_index);

    // Failure means that the entry does not exist.
    if (map_index == -1) {
        return -1;
    }

    // Return the reference.
    return hashmap->map[map_index].reference;
}