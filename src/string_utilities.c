#include "game.h"

char* concatenate_strings(const char* a, const char* b) {
    uint32_t size_a = strlen(a);
    uint32_t size_b = strlen(b);
    uint32_t result_size = size_a + size_b + 1;
    char* result = malloc(result_size);
    if (result == NULL) {
        return NULL;
    }
    for (uint32_t i = 0; i < size_a; i++) {
        result[i] = a[i];
    }
    for (uint32_t i = 0; i < size_b; i++) {
        result[i + size_a] = b[i];
    }
    result[size_a + size_b] = 0;
    return result;
}

int concatenate_string_in_place(char* a, const char* b) {
    uint32_t size_a = strlen(a);
    uint32_t size_b = strlen(b);

    a = realloc(a, size_a + size_b + 1);
    if (a == NULL) {
        return -1;
    }
    for (uint32_t i = 0; i < size_b; i++) {
        a[i + size_a] = b[i];
    }
    return 0;
}