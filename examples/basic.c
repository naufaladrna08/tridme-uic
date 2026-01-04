#include "ui_core.h"
#include "ui_widgets.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

static UIContext* g_ui = NULL;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
  if (g_ui) {
    printf("%c", codepoint);
    ui_input_char(g_ui, codepoint);
  }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (g_ui && action == GLFW_PRESS) {
    printf("Key pressed: %d\n", key);
    ui_set_key(g_ui, key, true);
  }
}

int main(void) {
  // Initialize GLFW
  if (!glfwInit()) {
      return -1;
  }
  
  glfwSetErrorCallback(error_callback);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow(800, 600, "UI Library Demo", NULL, NULL);
  if (!window) {
      glfwTerminate();
      return -1;
  }
  
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();
  
  // Create UI context
  UIContext* ui = ui_create_context(800, 600);
  g_ui = ui; // Set global for callbacks
  
  // Set up keyboard callbacks
  glfwSetCharCallback(window, char_callback);
  glfwSetKeyCallback(window, key_callback);
  
  // Demo state
  float slider_value = 0.5f;
  bool checkbox_checked = false;
  char text_buffer[256] = "Hello UI!";
  
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
    ui_begin_frame(ui, 0.016f); // 60 FPS delta
    
    // Create a panel
    ui_panel_begin(ui, "main_panel", (rect){{50, 50}, {300, 400}}, 
                  (color) {0.5f, 0.5f, 0.5f, 1.0f});
    
    // Push layout for this panel
    ui_push_layout(ui, (vec2){60, 60});
    
    // Add widgets
    if (ui_button(ui, "Click Me!", ui_layout_rect(ui, 0, 0, 100, 30))) {
      printf("Button clicked!\n");
    }
    
    if (ui_slider_float(ui, "slider1", ui_layout_rect(ui, 0, 40, 200, 20),
                        &slider_value, 0.0f, 1.0f)) {
        printf("Slider value: %f\n", slider_value);
    }
    
    if (ui_checkbox(ui, "Enable feature", ui_layout_rect(ui, 0, 70, 150, 20), &checkbox_checked)) {
      printf("Checkbox: %s\n", checkbox_checked ? "checked" : "unchecked");
    }
    
    ui_text_input(ui, "text_input", ui_layout_rect(ui, 0, 100, 200, 25),
      text_buffer, sizeof(text_buffer));
    
    ui_pop_layout(ui);
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