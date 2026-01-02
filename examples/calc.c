#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <ui_core.h>
#include <ui_widgets.h>

static UIContext* g_ui = NULL;
const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 400;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
  if (g_ui) {
    ui_input_char(g_ui, codepoint);
  }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (g_ui && action == GLFW_PRESS) {
    printf("Key pressed: %d\n", key);
    ui_set_key(g_ui, key, true);
  }
}

int main(int argc, char** argv) {
  // Initialize GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }
  
  glfwSetErrorCallback(error_callback);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Calculator Demo", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();
  
  /* Create Tridme UI context */
  UIContext* ui = ui_create_context(WINDOW_WIDTH, WINDOW_HEIGHT);
  g_ui = ui;
  
  /* Setup callbacks */
  glfwSetCharCallback(window, char_callback);
  glfwSetKeyCallback(window, key_callback);

  const char* label = "Calculator";
  char buffer[256] = "0";
  char buttons[4][5] = {
    {'7', '8', '9', '/', 'A'},
    {'4', '5', '6', '*', 'B'},
    {'1', '2', '3', '-', 'C'},
    {'0', '.', '=', '+', 'D'}
  };
  float button_size = 20.0f;
  
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Update input BEFORE beginning frame (so prev state is captured correctly)
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    ui_set_mouse_position(ui, (float)mouse_x, (float)mouse_y);
    
    ui_set_mouse_button(ui, 0, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
    ui_set_mouse_button(ui, 1, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT));
    
    // Begin UI frame (this copies current mouse state to previous)
    ui_begin_frame(ui, 0.016f);

    // Create a panel for the calculator
    ui_panel_begin(ui, "calc_panel", (rect){{10, 10},
                  {180, 380}}, 
                  (color){0.2f, 0.2f, 0.2f, 1.0f});

    // Draw label
    ui_label(ui, label, (rect){{20, 20}, {140, 30}}, 
             (color){1.0f, 1.0f, 1.0f, 1.0f});

    // Draw text input for display
    ui_text_input(ui, "calc_display", (rect){{20, 60}, {140, 30}}, 
                  buffer, sizeof(buffer));

    // Draw buttons
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 5; col++) {
        char btn_label[2] = {buttons[row][col], '\0'};
        rect btn_bounds = {
          {20.0f + col * (button_size + 10.0f), 110.0f + row * (button_size + 10.0f)},
          {button_size, button_size}
        };
        if (ui_button(ui, btn_label, btn_bounds)) {
          // Handle button press
          size_t len = 0;
          while (buffer[len] != '\0') len++;
          if (len < sizeof(buffer) - 1) {
            buffer[len] = buttons[row][col];
            buffer[len + 1] = '\0';
          }

          if (buttons[row][col] == 'A') {
            buffer[0] = '\0'; // Clear
          }

          if (buttons[row][col] == 'D') {
            // Simple evaluation for demonstration (only handles single digit operations)
            int result = 0;
            int num1 = buffer[0] - '0';
            int num2 = buffer[2] - '0';
            char op = buffer[1];
            switch (op) {
              case '+': result = num1 + num2; break;
              case '-': result = num1 - num2; break;
              case '*': result = num1 * num2; break;
              case '/': if (num2 != 0) result = num1 / num2; break;
              default: break;
            }
            snprintf(buffer, sizeof(buffer), "%d", result);
          }
        }
      }
    }

    ui_panel_end(ui);
    
    // End UI frame and render
    ui_end_frame(ui);
    
    glfwSwapBuffers(window);
  }
  
  // Cleanup
  ui_destroy_context(ui);
  glfwDestroyWindow(window);
  glfwTerminate();
  
  return 0;
}