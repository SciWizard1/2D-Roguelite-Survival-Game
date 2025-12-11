#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* -------- error.c -------- */

#define ERR(code, msg, severity) err_raw(code, msg, __LINE__, __FILE__, severity)

typedef struct ErrInstance ErrInstance;

typedef enum ErrSeverity {
    ERR_MESSAGE,
    ERR_WARNING,
    ERR_ERROR,
    ERR_FATAL,
} ErrSeverity;

int err_raw(int32_t code, const char* msg, int32_t line, const char* file, ErrSeverity severity);
int32_t err_get_instance_index_by_code(int32_t code);
bool err_dependency_failed(int32_t code);

/* -------- hashmap.c -------- */

struct HashMapEntry {
    int64_t key;
    int64_t reference;
    bool    is_used;
};

typedef struct HashMapEntry HashMapEntry;

struct HashMap {
    HashMapEntry* map;
    int64_t       size_exponent;
    int64_t       num_entries;
};

typedef struct HashMap HashMap;

HashMap hmap_instantiate_new(int32_t failure_code);
void hmap_insert_entry(HashMap *hashmap, int64_t key, int64_t reference, int32_t failure_code);
int64_t hmap_hash_function(int64_t key);
int64_t hmap_get_entry(HashMap *hashmap, int64_t key);
int hmap_expand(HashMap *hashmap, int32_t failure_code);