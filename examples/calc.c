/*
 * Tridme UI Example Program: Calculator
 * 
 * In this example, I will demonstrate how to use Tridme UI for making 
 * real-world application like Calculator. We will use some widgets, like
 * Input, Buttons and Panel.
 * 
 * Author: Naufal Adriansyah
 * (C) Kincir Angin 2026
 */

#include <stdio.h> // Essential for debugging


/* Tridme UI's include files */
#include <stdlib.h>
#include <ui_core.h>
#include <ui_widgets.h>
#include <ui_utils.h>

/* 
 * Tridme UI doesn't provides window instance creation class, so we need a 
 * window handling libraries like GLFW or you can use native Windows or X11
 * to build the Window. In this example, we'll use GLFW for it's simplicity
 * and cross-platform support. We also need OpenGL function loader and we'll
 * GLEW for this example. 
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Global Variables */
const char* APP_NAME = "CALCULATOR";
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 800;
UIContext* g_ui;

static void error_callback(int error_code, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

int main(int argc, const char** argv) {
  printf("Calculator App Running...\n");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create Window Instance */
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
  }

  glfwSetErrorCallback(error_callback);

  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME, NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create GLFW window");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glewExperimental = true;
  glewInit();

  /* Create Graphic Context */
  UIContext* ui = ui_create_context(WINDOW_WIDTH, WINDOW_HEIGHT);
  g_ui = ui;

  glfwSetCharCallback(window, char_callback);
  glfwSetKeyCallback(window, key_callback);

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  const char* menu[4] = { "FILE", "MODE", "OPTION", "ABOUT" };
  float button_size = 100.0f;
  
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - currentFrame;
    lastFrame = currentFrame;
 
    glfwPollEvents();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* To make event works, sadly Tridme UI need some data to be filled */
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    ui_set_mouse_position(ui, mouse_x, mouse_y);
    ui_set_mouse_button(ui, 0, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
    ui_set_mouse_button(ui, 1, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT));

    ui_begin_frame(ui, deltaTime);
    ui_panel_begin(ui, "main_panel", (rect) { { 0, 0 }, { WINDOW_WIDTH, WINDOW_HEIGHT } }, color_from_hex("#3f3f3fff"));
    ui_push_layout(ui, (vec2) { 0, 0 });

    // for (int i = 0; i < 4; i++) {
    //   rect btn_bounds = {
    //     { i * button_size, 0 },
    //     { button_size, button_size / 3 }
    //   };

    //   if (ui_button(ui, menu[i], btn_bounds)) {
    //     printf("Button is pressed\n");
    //   }
    // }

    if (ui_button(ui, "Hello World", (rect) {{ 0, 0} , {0, 0}})) {
      printf("const char *restrict format, ..\n");
    }

    if (ui_button(ui, "Hello World 2", (rect) {{ 0, 0} , {0, 0}})) {
      printf("const char *restrict format, ..\n");
    }

    if (ui_button(ui, "Hello World 3", (rect) {{ 0, 0} , {0, 0}})) {
      printf("const char *restrict format, ..\n");
    }
    
    ui_pop_layout(ui);
    ui_panel_end(ui);
    ui_end_frame(ui);

    glfwSwapBuffers(window);
  }

  ui_destroy_context(ui);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}