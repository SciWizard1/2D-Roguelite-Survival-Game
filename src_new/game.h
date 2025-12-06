#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ERR(code, msg, severity) err_raw(code, msg, __LINE__, __FILE__, severity)

typedef struct ErrInstance ErrInstance;

typedef enum ErrSeverity {
    ERR_MESSAGE,
    ERR_WARNING,
    ERR_ERROR,
    ERR_FATAL,
} ErrSeverity;

int err_raw(int32_t code, char* msg, int32_t line, char* file, ErrSeverity severity);
int32_t err_get_instance_index_by_code(int32_t code);
bool err_dependency_failed(int32_t code);