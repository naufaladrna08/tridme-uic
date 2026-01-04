/*
 * Part of Tridme Engine Project
 * (C) Kincir Angin Studio 2025
 */

#ifndef UI_CORE_H
#define UI_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32  
  #define UI_API __declspec(dllexport)
#else
  #define UI_API __attribute__ ((visibility("default")))
#endif

// Basic types
typedef struct { float x, y; } vec2;
typedef struct { float r, g, b, a; } color;
typedef struct { vec2 pos; vec2 size; } rect;


// Font glyph info (internal use)
typedef struct {
  float x, y;
  float width, height;
  float advance;
  float offset_x, offset_y;
} ui_glyph_info;

// UI Context
typedef struct {
  // Window dimensions
  int width, height;
  
  // Input state
  vec2 mouse_pos;
  bool mouse_buttons[3];
  bool prev_mouse_buttons[3];
  float scroll_offset;
  
  // Keyboard input
  char input_chars[32];
  int input_char_count;
  bool key_backspace;
  bool key_delete;
  bool key_left;
  bool key_right;
  
  // Render state
  unsigned int shader;
  unsigned int vao, vbo, ebo;
  unsigned int texture_atlas;
  unsigned int white_texture;
  
  // Font data
  ui_glyph_info glyphs[128];
  float font_size;
  
  // Widget state
  uint32_t hot_widget;
  uint32_t active_widget;
  uint32_t focused_widget;
  
  // Layout stack
  vec2 layout_stack[32];
  int layout_stack_size;
  
  // Time
  float time;
  float delta_time;

  // State
  vec2  last_pos;
  float margin;
} UIContext;

// Function prototypes
UI_API UIContext* ui_create_context(int width, int height);
UI_API void ui_destroy_context(UIContext* ctx);

// Frame Management
UI_API void ui_begin_frame(UIContext* ctx, float delta_time);
UI_API void ui_end_frame(UIContext* ctx);

// Input Handling
UI_API void ui_set_mouse_position(UIContext* ctx, float x, float y);
UI_API void ui_set_mouse_button(UIContext* ctx, int button, bool pressed);
UI_API void ui_set_key(UIContext* ctx, int key, bool pressed);
UI_API void ui_set_scroll(UIContext* ctx, float offset);
UI_API void ui_input_char(UIContext* ctx, unsigned int codepoint);

// Drawing
UI_API void ui_draw_rect(UIContext* ctx, rect r, color c);
UI_API void ui_draw_text(UIContext* ctx, const char* text, vec2 position, color c);
UI_API float ui_measure_text(UIContext* ctx, const char* text);

// State 
UI_API bool ui_is_hovered(UIContext* ctx, rect r);
UI_API bool ui_is_clicked(UIContext* ctx, rect r, int button);
UI_API vec2 ui_get_mouse_position(UIContext* ctx);

#endif