#include "game.h"

#define INIT_ERR_INSTANCE_SIZE 16

struct ErrInstance {
    int32_t     code;
    int32_t     quantity;
    const char* msg;
    int32_t     line;
    const char* file;
    ErrSeverity severity;
};

ErrInstance* err_instances = NULL;
int32_t  err_num_instances = 0;
int32_t  err_max_instances = 0;
bool   err_is_hashmap_good = false;

int err_reallocate_instance_array() {
    ErrInstance* ptr;

    if (err_max_instances == 0) {
        // Set an initial size on first allocation.
        ptr = realloc(
            err_instances, 
            INIT_ERR_INSTANCE_SIZE * sizeof(ErrInstance)
        );
    } else {
        // Double the size of the error instance array.
        ptr = realloc(
            err_instances, 
            err_max_instances * sizeof(ErrInstance) * 2
        );
    }
    
    // The pointer must be valid.
    if (ptr == NULL) {
        fprintf(stderr, "Failed to reallocate memory for error handler.\n");
        return -1;
    }

    // Update array data.
    err_instances = ptr;
    err_max_instances *= 2;

    return 0;
}

void err_free_instance_array() {
    free(err_instances);
}

int err_raw(int32_t code, const char* msg, int32_t line, const char* file, ErrSeverity severity) {
    // Instantiate an error object.
    ErrInstance err_instance = (ErrInstance){
        code,
        1,
        msg,
        line,
        file,
        severity
    };

    // Expand error object array if necessary.
    int realloc_error_code = 0;
    if (err_num_instances + 1 >= err_max_instances) {
        realloc_error_code = err_reallocate_instance_array();
    }

    // The resize must occur, otherwise memory will be written out of bounds.
    if (realloc_error_code < 0) {
        return -1;
        
        // Failure to reallocate error instance array is fatal.
        exit(-1);
    }

    // Set instance data.
    err_instances[err_num_instances] = err_instance;
    err_num_instances++;

    // Console logs
    fprintf(stderr, "Error in file %s at line %d:\n", file, line);
    fprintf(stderr, "%s", msg);

    // TODO: Add file logging output.

    return code;
}

int32_t err_get_instance_index_by_code(int32_t code) {
    if (!err_is_hashmap_good) {
        // Use a linear search.
        for (int32_t i = 0; i < err_num_instances; i++) {
            if (err_instances[i].code == code) {
                return i;
            }
        }

        // Failed to find instance.
        return -1; 

    } else {
        // Use a hashmap for speed if it's available.
        // TODO: Write a hashmap module.

        return -1;
    }
}

bool err_dependency_failed(int32_t code) {
    // Get the index of the error instance.
    int32_t instance_index = err_get_instance_index_by_code(code);
    
    if (instance_index == -1) {
        // If an error didn't occur, it's likely the system is operational.
        return false;
    }
    
    // If the severity is more than a warning, the system likely failed.
    int32_t instance_severity = err_instances[instance_index].severity;
    if (instance_severity >= ERR_ERROR) {
        return true;
    }

    // Otherwise, it probably works.
    return false;
}