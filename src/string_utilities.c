#include "game.h"

char* concatenate_strings(const char* a, const char* b) {
    uint32_t size_a = strlen(a);
    uint32_t size_b = strlen(b);
    uint32_t result_size = size_a + size_b + 1;
    char* result = tracked_malloc(result_size);
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

    a = tracked_realloc(a, size_a + size_b + 1);
    if (a == NULL) {
        return -1;
    }
    for (uint32_t i = 0; i < size_b; i++) {
        a[i + size_a] = b[i];
    }
    return 0;
}

uint32_t stack_memory_usage = 0;
void*    pointer_stack[MAX_TRACKED_POINTERS];
uint32_t buffer_sizes[MAX_TRACKED_POINTERS];
uint32_t pointer_stack_top = 0;

void initialize_tracked_memory_buffers() {
    for (uint32_t i = 0; i < MAX_TRACKED_POINTERS; i++) {
        pointer_stack[i] = NULL;
        buffer_sizes[i] = 0;
    }
}

void trigger_memory_failure() {
    // Cleanup functions here


    // Exit the program early to prevent execution of unsafe code.
    printf("Failed to allocate memory!\n");
    exit(-1);
}

void* tracked_malloc(uint32_t size) {

    void *pointer = NULL;
    int i = 0;
    do {
        i++;
        pointer = malloc(size);
    } while (pointer == NULL && i < MAX_MALLOC_RETRIES);

    if (pointer == NULL) {
        trigger_memory_failure();
        return NULL;
    }

    stack_memory_usage += size;
    pointer_stack[pointer_stack_top] = pointer;
    buffer_sizes[pointer_stack_top]  = size;
    pointer_stack_top++;

    printf("Allocation sizes: ");
    for (uint32_t i = 0; i < pointer_stack_top; i++) {
        printf("%d, ", buffer_sizes[i]);
    }
    printf("\n");

    return pointer;
}

void tracked_free(void* pointer) {
    if (pointer == NULL) {
        return;
    }
    // Search for the pointer instance.
    uint32_t pointer_index = UINT32_MAX;
    for (uint32_t i = 0; i < pointer_stack_top; i++) {
        if (pointer_stack[i] == pointer) {
            pointer_index = i;
            break;
        }
    }    

    // Exit in case the pointer was never tracked.
    if (pointer_index == UINT32_MAX) {
        printf("Warning: freed an untracked pointer %p\n", pointer);
        free(pointer);
        return;
    }

    free(pointer);

    // Account for the memory deletion.
    stack_memory_usage -= buffer_sizes[pointer_index];
    
    // Delete the old pointer and shift all following instances to fill the gap.
    for (uint32_t i = pointer_index; i < pointer_stack_top - 1; i++) {
        pointer_stack[i] = pointer_stack[i + 1];
        buffer_sizes[i]  = buffer_sizes[i + 1];
    }

    // Account for the shift.
    pointer_stack_top--;
}

void* tracked_realloc(void* pointer, uint32_t size) {
    // Handle NULL case.
    if (pointer == NULL) {
        return tracked_malloc(size);
    }

    // Search for the pointer instance.
    uint32_t pointer_index = UINT32_MAX;
    for (uint32_t i = 0; i < pointer_stack_top; i++) {
        if (pointer_stack[i] == pointer) {
            pointer_index = i;
            break;
        }
    }

    if (pointer_index == UINT32_MAX) {
        printf("Warning: Attempted to reallocate an untracked pointer.\n");
        return realloc(pointer, size);
    }

    // Attempt the reallocation several times if it fails.
    int i = 0;
    void* new_pointer = NULL;
    do {
        new_pointer = realloc(pointer, size);
        i++;
    } while (new_pointer == NULL && i < MAX_MALLOC_RETRIES);

    // Exit if the allocation still did not happen.
    if (new_pointer == NULL) {
        trigger_memory_failure();
        return NULL;
    }

    pointer_stack[pointer_index] = new_pointer;
    buffer_sizes[pointer_index] = size;
    return new_pointer;
}