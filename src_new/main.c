#include "game.h"

int main() {
    ERR(26, "Test error\n", ERR_FATAL);
    if (err_dependency_failed(26)) {
        printf("Test 1 passed!\n");
    }
    if (!err_dependency_failed(27)) {
        printf("Test 2 passed!\n");
    }
    return 0;
}