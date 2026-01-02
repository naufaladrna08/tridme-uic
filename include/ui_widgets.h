#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <ui_core.h>

// Widget IDs
typedef uint32_t ui_id;

// Button
UI_API bool ui_button(UIContext* ctx, const char* label, rect bounds);

// Label
UI_API void ui_label(UIContext* ctx, const char* text, rect bounds, color text_color);

// Slider
UI_API bool ui_slider_float(UIContext* ctx, const char* id, rect bounds, float* value, 
  float min_val, float max_val);

// Checkbox
UI_API bool ui_checkbox(UIContext* ctx, const char* label, rect bounds, bool* checked);
// Text input
UI_API bool ui_text_input(UIContext* ctx, const char* id, rect bounds, 
  char* buffer, size_t buffer_size);

// Panel
UI_API void ui_panel_begin(UIContext* ctx, const char* id, rect bounds, color bg_color);
UI_API void ui_panel_end(UIContext* ctx);

// Layout helpers
UI_API rect ui_layout_rect(UIContext* ctx, float x, float y, float w, float h);
UI_API void ui_push_layout(UIContext* ctx, vec2 pos);
UI_API void ui_pop_layout(UIContext* ctx);

#endif