#include "game.h"

int main() {
    HashMap hashmap = hmap_instantiate_new(7);

    glfwInit();


    float longest_time = 0.0;
    int32_t worst_index = 0;


    for (int32_t new_key = 0; new_key < 102400; new_key++) {
        float start_time = glfwGetTime();
        hmap_insert_entry(&hashmap, new_key, new_key * 124 - 1218, 8);
        float total_time = glfwGetTime() - start_time;

        if (total_time > longest_time) {
            longest_time = total_time;
            worst_index = new_key;
        }
        //printf("Successfully inserted entry #%d\n", new_key);
    }


    printf("Worst-case entry took %f seconds.\n", longest_time);

    printf("Entry: %d\n", worst_index);

    return 0;
}