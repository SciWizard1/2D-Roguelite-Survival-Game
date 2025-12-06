#include "game.h"

int keyboard[KEY_COUNT];
int mouse[MOUSE_BUTTON_COUNT];
double cursor_x;
double cursor_y;

void keyboard_event_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    
    if (key < 0 || key >= KEY_COUNT) {return;}
    keyboard[key] = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void mouse_button_event_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)mods;

    if (button < 0 || button >= MOUSE_BUTTON_COUNT) {return;}
    mouse[button] = action == GLFW_PRESS;
}

void cursor_position_event_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;

    cursor_x = xpos;
    cursor_y = ypos;
}