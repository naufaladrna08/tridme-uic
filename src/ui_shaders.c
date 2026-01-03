#include <ui_shaders.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Vertex shader source
static const char* vertex_shader_source = 
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"layout(location = 2) in vec4 aColor;\n"
"\n"
"out vec2 TexCoord;\n"
"out vec4 Color;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"  gl_Position = projection * vec4(aPos, 0.0, 1.0);\n"
"  TexCoord = aTexCoord;\n"
"  Color = aColor;\n"
"}\n";

// Fragment shader source
static const char* fragment_shader_source = 
"#version 330 core\n"
"\n"
"in vec2 TexCoord;\n"
"in vec4 Color;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D tex;\n"
"uniform vec4 color;\n"
"\n"
"void main() {\n"
"  float alpha = texture(tex, TexCoord).r;\n"
"  FragColor = vec4(color.rgb, color.a * alpha) * Color;\n"
"}\n";

// Fragment shader for solid colors (no texture)
static const char* fragment_shader_solid_source = 
"#version 330 core\n"
"\n"
"in vec2 TexCoord;\n"
"in vec4 Color;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform vec4 color;\n"
"\n"
"void main() {\n"
"  FragColor = color * Color;\n"
"}\n";

// Rounded rectangle fragment shader
static const char* fragment_shader_rounded_source = 
"#version 330 core\n"
"\n"
"in vec2 TexCoord;\n"
"in vec4 Color;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform vec4 color;\n"
"uniform vec2 size;\n"
"uniform float radius;\n"
"uniform vec2 position;\n"
"\n"
"float roundedBoxSDF(vec2 p, vec2 b, float r) {\n"
"  vec2 q = abs(p) - b + r;\n"
"  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
"}\n"
"\n"
"void main() {\n"
"  vec2 center = vec2(size.x * 0.5, size.y * 0.5);\n"
"  vec2 p = TexCoord * size - center;\n"
"  \n"
"  float distance = roundedBoxSDF(p, center, radius);\n"
"  \n"
"  if (distance > 0.0) {\n"
"    discard;\n"
"  }\n"
"  \n"
"  FragColor = color * Color;\n"
"}\n";

// Utility function to read shader from file
static char* read_shader_file(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "Failed to open shader file: %s\n", filename);
    return NULL;
  }
  
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  char* buffer = (char*)malloc(length + 1);
  if (!buffer) {
    fclose(file);
    return NULL;
  }
  
  fread(buffer, 1, length, file);
  buffer[length] = '\0';
  
  fclose(file);
  return buffer;
}

// Compile a single shader
static unsigned int compile_shader(GLenum type, const char* source) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  
  // Check compilation status
  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "Shader compilation failed:\n%s\n", info_log);
    glDeleteShader(shader);
    return 0;
  }
  
  return shader;
}

// Create a shader program from source strings
unsigned int ui_compile_shader(const char* vertex_src, const char* fragment_src) {
  unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
  if (!vertex_shader) {
    return 0;
  }
  
  unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
  if (!fragment_shader) {
    glDeleteShader(vertex_shader);
    return 0;
  }
  
  // Link shaders
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  
  // Check linking status
  int success;
  char info_log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "Shader program linking failed:\n%s\n", info_log);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);
    return 0;
  }
  
  // Clean up shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  return program;
}

// Create a shader program from files
unsigned int ui_compile_shader_from_files(const char* vertex_file, const char* fragment_file) {
  char* vertex_source = read_shader_file(vertex_file);
  if (!vertex_source) {
    return 0;
  }
  
  char* fragment_source = read_shader_file(fragment_file);
  if (!fragment_source) {
    free(vertex_source);
    return 0;
  }
  
  unsigned int program = ui_compile_shader(vertex_source, fragment_source);
  
  free(vertex_source);
  free(fragment_source);
  
  return program;
}

// Create the default UI shader
unsigned int ui_create_ui_shader(void) {
  return ui_compile_shader(vertex_shader_source, fragment_shader_source);
}

// Create a solid color shader (no texture)
unsigned int ui_create_solid_shader(void) {
  return ui_compile_shader(vertex_shader_source, fragment_shader_solid_source);
}

// Create a rounded rectangle shader
unsigned int ui_create_rounded_shader(void) {
  return ui_compile_shader(vertex_shader_source, fragment_shader_rounded_source);
}

// Get uniform location
static int get_uniform_location(unsigned int shader, const char* name) {
  int location = glGetUniformLocation(shader, name);
  if (location == -1) {
    fprintf(stderr, "Warning: Uniform '%s' not found in shader\n", name);
  }
  return location;
}

// Set uniform functions
void ui_set_uniform_mat4(unsigned int shader, const char* name, const float* matrix) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
  }
}

void ui_set_uniform_vec4(unsigned int shader, const char* name, float x, float y, float z, float w) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform4f(location, x, y, z, w);
  }
}

void ui_set_uniform_vec3(unsigned int shader, const char* name, float x, float y, float z) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform3f(location, x, y, z);
  }
}

void ui_set_uniform_vec2(unsigned int shader, const char* name, float x, float y) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform2f(location, x, y);
  }
}

void ui_set_uniform_int(unsigned int shader, const char* name, int value) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform1i(location, value);
  }
}

void ui_set_uniform_float(unsigned int shader, const char* name, float value) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform1f(location, value);
  }
}

void ui_set_uniform_bool(unsigned int shader, const char* name, bool value) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform1i(location, value ? 1 : 0);
  }
}

// Set texture uniform
void ui_set_uniform_texture(unsigned int shader, const char* name, unsigned int texture_unit) {
  glUseProgram(shader);
  int location = get_uniform_location(shader, name);
  if (location != -1) {
    glUniform1i(location, texture_unit);
  }
}

// Set multiple uniforms for rounded rectangle
void ui_set_rounded_uniforms(unsigned int shader, float width, float height, float radius, float x, float y) {
  glUseProgram(shader);
  
  int size_loc = get_uniform_location(shader, "size");
  if (size_loc != -1) {
    glUniform2f(size_loc, width, height);
  }
  
  int radius_loc = get_uniform_location(shader, "radius");
  if (radius_loc != -1) {
    glUniform1f(radius_loc, radius);
  }
  
  int pos_loc = get_uniform_location(shader, "position");
  if (pos_loc != -1) {
    glUniform2f(pos_loc, x, y);
  }
}

// Create a batch rendering shader (for drawing multiple quads)
unsigned int ui_create_batch_shader(void) {
  const char* batch_vertex_source = 
  "#version 330 core\n"
  "layout(location = 0) in vec2 aPos;\n"
  "layout(location = 1) in vec2 aTexCoord;\n"
  "layout(location = 2) in vec4 aColor;\n"
  "layout(location = 3) in vec4 aTransform;\n"  // x, y, width, height
  "\n"
  "out vec2 TexCoord;\n"
  "out vec4 Color;\n"
  "\n"
  "uniform mat4 projection;\n"
  "\n"
  "void main() {\n"
  "  vec2 transformedPos = aPos * aTransform.zw + aTransform.xy;\n"
  "  gl_Position = projection * vec4(transformedPos, 0.0, 1.0);\n"
  "  TexCoord = aTexCoord;\n"
  "  Color = aColor;\n"
  "}\n";
  
  return ui_compile_shader(batch_vertex_source, fragment_shader_source);
}

// Shader manager structure (optional, for managing multiple shaders)
typedef struct {
  unsigned int ui_shader;
  unsigned int solid_shader;
  unsigned int rounded_shader;
  unsigned int batch_shader;
} shader_manager;

// Create and initialize shader manager
shader_manager* ui_create_shader_manager(void) {
  shader_manager* manager = (shader_manager*)malloc(sizeof(shader_manager));
  if (!manager) return NULL;
  
  manager->ui_shader = ui_create_ui_shader();
  manager->solid_shader = ui_create_solid_shader();
  manager->rounded_shader = ui_create_rounded_shader();
  manager->batch_shader = ui_create_batch_shader();
  
  // Check if all shaders were created successfully
  if (!manager->ui_shader || !manager->solid_shader || 
    !manager->rounded_shader || !manager->batch_shader) {
    fprintf(stderr, "Failed to create one or more shaders\n");
    free(manager);
    return NULL;
  }
  
  return manager;
}

// Destroy shader manager
void ui_destroy_shader_manager(shader_manager* manager) {
  if (!manager) return;
  
  if (manager->ui_shader) glDeleteProgram(manager->ui_shader);
  if (manager->solid_shader) glDeleteProgram(manager->solid_shader);
  if (manager->rounded_shader) glDeleteProgram(manager->rounded_shader);
  if (manager->batch_shader) glDeleteProgram(manager->batch_shader);
  
  free(manager);
}

// Set projection matrix for all shaders in manager
void ui_shader_manager_set_projection(shader_manager* manager, const float* matrix) {
  if (!manager) return;
  
  ui_set_uniform_mat4(manager->ui_shader, "projection", matrix);
  ui_set_uniform_mat4(manager->solid_shader, "projection", matrix);
  ui_set_uniform_mat4(manager->rounded_shader, "projection", matrix);
  ui_set_uniform_mat4(manager->batch_shader, "projection", matrix);
}

// Get specific shader from manager
unsigned int ui_shader_manager_get_shader(shader_manager* manager, int shader_type) {
  if (!manager) return 0;
  
  switch (shader_type) {
    case 0: return manager->ui_shader;      // Default UI shader
    case 1: return manager->solid_shader;   // Solid color shader
    case 2: return manager->rounded_shader; // Rounded rectangle shader
    case 3: return manager->batch_shader;   // Batch rendering shader
    default: return manager->ui_shader;
  }
}