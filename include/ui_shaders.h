#ifndef UI_SHADERS_H
#define UI_SHADERS_H

#ifdef _WIN32  
  #define UI_API __declspec(dllexport)
#else
  #define UI_API __attribute__ ((visibility("default")))
#endif
#include <stdbool.h>  

// Shader compilation
UI_API unsigned int ui_compile_shader(const char* vertex_src, const char* fragment_src);
UI_API unsigned int ui_compile_shader_from_files(const char* vertex_file, const char* fragment_file);

// Predefined shaders
UI_API unsigned int ui_create_ui_shader(void);
UI_API unsigned int ui_create_solid_shader(void);
UI_API unsigned int ui_create_rounded_shader(void);
UI_API unsigned int ui_create_batch_shader(void);

// Uniform setters
UI_API void ui_set_uniform_mat4(unsigned int shader, const char* name, const float* matrix);
UI_API void ui_set_uniform_vec4(unsigned int shader, const char* name, float x, float y, float z, float w);
UI_API void ui_set_uniform_vec3(unsigned int shader, const char* name, float x, float y, float z);
UI_API void ui_set_uniform_vec2(unsigned int shader, const char* name, float x, float y);
UI_API void ui_set_uniform_int(unsigned int shader, const char* name, int value);
UI_API void ui_set_uniform_float(unsigned int shader, const char* name, float value);
UI_API void ui_set_uniform_bool(unsigned int shader, const char* name, bool value);
UI_API void ui_set_uniform_texture(unsigned int shader, const char* name, unsigned int texture_unit);
UI_API void ui_set_rounded_uniforms(unsigned int shader, float width, float height, float radius, float x, float y);

#endif
