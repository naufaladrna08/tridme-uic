#include "ui_core.h"
#include "ui_styles.h"
#include <stdio.h>
#include <ui_widgets.h>
#include <GL/glew.h>

static ui_id hash_string(const char* str) {
  ui_id hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

bool ui_button(UIContext* ctx, const char* label, rect bounds) {
  ui_id id = hash_string(label);

  /*
   * If the rect bounds is (0, 0), we count the size for the label's length
   * and measure the size based of that.
   */ 
  if (bounds.size.x == 0 && bounds.size.y == 0) {
    float text_width = ui_measure_text(ctx, label);
    float padding = 10;

    bounds.size.x = text_width + padding * 2;
    bounds.size.y = ctx->font_size + padding * 2;
  }

  // Check interaction
  bool hovered = ui_is_hovered(ctx, bounds);
  bool clicked = ui_is_clicked(ctx, bounds, 0);
  
  // Visual state
  color bg_color = {0.2f, 0.2f, 0.2f, 1.0f};
  if (hovered) {
    bg_color.r += 0.1f;
    bg_color.g += 0.1f;
    bg_color.b += 0.1f;
  }
  
  // Draw border first (background)
  rect border = {
    {bounds.pos.x - 1, bounds.pos.y - 1},
    {bounds.size.x + 2, bounds.size.y + 2}
  };

  color border_color = {0.0f, 0.0f, 0.0f, 1.0f};
  ui_draw_rect(ctx, border, border_color);
  
  // Draw button on top
  ui_draw_rect(ctx, bounds, bg_color);
  
  // Draw label (simplified - would use actual text rendering)
  color text_color = {1.0f, 1.0f, 1.0f, 1.0f};
  ui_label(ctx, label, bounds, text_color);
  
  return clicked;
}

void ui_label(UIContext* ctx, const char* text, rect bounds, color text_color) {
  // Measure text width
  float text_width = ui_measure_text(ctx, text);
  
  // Center text horizontally and vertically
  // For vertical: position baseline at center, then adjust for typical ascent
  vec2 text_pos = {
    bounds.pos.x + (bounds.size.x - text_width) * 0.5f,
    bounds.pos.y + bounds.size.y * 0.5f + 4.0f  // Center baseline with offset
  };
  ui_draw_text(ctx, text, text_pos, text_color);
}

void ui_label_with_style(UIContext* ctx, const char* text, rect bounds, TextStyle style) {
  // Measure text width
  float text_width = ui_measure_text(ctx, text);
  
  // Center text horizontally and vertically
  // For vertical: position baseline at center, then adjust for typical ascent
  vec2 text_pos = {
    bounds.pos.x + (bounds.size.x - text_width) * 0.5f,
    bounds.pos.y + bounds.size.y * 0.5f + 4.0f  // Center baseline with offset
  };
  ui_draw_text(ctx, text, text_pos, style.color);
}

bool ui_slider_float(UIContext* ctx, const char* id, rect bounds, float* value, float min_val, float max_val) {
  ui_id widget_id = hash_string(id);
  bool value_changed = false;
  
  // Track state
  if (ui_is_hovered(ctx, bounds)) {
    ctx->hot_widget = widget_id;
  }
  
  // Handle dragging
  if (ctx->active_widget == widget_id || (ctx->hot_widget == widget_id && ui_is_clicked(ctx, bounds, 0))) {
    if (!ctx->active_widget) {
      ctx->active_widget = widget_id;
    }
    
    // Update value based on mouse position
    float normalized = (ctx->mouse_pos.x - bounds.pos.x) / bounds.size.x;
    normalized = normalized < 0.0f ? 0.0f : (normalized > 1.0f ? 1.0f : normalized);
    float new_value = min_val + normalized * (max_val - min_val);
    
    if (new_value != *value) {
      *value = new_value;
      value_changed = true;
    }
  } 
  
  if (!ctx->mouse_buttons[0]) {
    ctx->active_widget = 0;
  }
  
  // Draw slider track
  color track_color = {0.3f, 0.3f, 0.3f, 1.0f};
  ui_draw_rect(ctx, bounds, track_color);
  
  /* 
   * Draw the slider thumb 
   *
   * The thumb position is calculated based on the current value. 
   */
  float normalized_value = (*value - min_val) / (max_val - min_val);
  rect thumb = {
    { bounds.pos.x + normalized_value * bounds.size.x - 5.0f, bounds.pos.y },
    { 10.0f, bounds.size.y }
  };
  color thumb_color = {0.5f, 0.5f, 0.8f, 1.0f};

  ui_draw_rect(ctx, thumb, thumb_color);
  
  return value_changed;
}

bool ui_checkbox(UIContext* ctx, const char* label, rect bounds, bool* checked) {
  ui_id id = hash_string(label);
  bool value_changed = false;
  
  // Check interaction
  bool hovered = ui_is_hovered(ctx, bounds);
  bool clicked = ui_is_clicked(ctx, bounds, 0);
  
  if (clicked) {
    *checked = !(*checked);
    value_changed = true;
  }
  
  // Calculate checkbox box (square on the left)
  float box_size = bounds.size.y * 0.8f; // Use 80% of height
  if (box_size < 16.0f) box_size = 16.0f; // Minimum 16 pixels
  if (box_size > 24.0f) box_size = 24.0f; // Maximum 24 pixels
  
  rect checkbox_box = {
    {bounds.pos.x, bounds.pos.y + (bounds.size.y - box_size) * 0.5f},
    {box_size, box_size}
  };
  
  // Draw border first
  rect border = {
    {checkbox_box.pos.x - 1, checkbox_box.pos.y - 1},
    {checkbox_box.size.x + 2, checkbox_box.size.y + 2}
  };
  color border_color = {0.0f, 0.0f, 0.0f, 1.0f};
  ui_draw_rect(ctx, border, border_color);
  
  // Draw checkbox background on top
  color bg_color = {0.2f, 0.2f, 0.2f, 1.0f};
  if (hovered) {
    bg_color.r += 0.1f;
    bg_color.g += 0.1f;
    bg_color.b += 0.1f;
  }
  ui_draw_rect(ctx, checkbox_box, bg_color);
  
  // Draw checkmark if checked
  if (*checked) {
    float padding = box_size * 0.2f;
    rect checkmark = {
      {checkbox_box.pos.x + padding, checkbox_box.pos.y + padding},
      {box_size - padding * 2.0f, box_size - padding * 2.0f}
    };
    color check_color = {0.3f, 0.7f, 0.3f, 1.0f};
    ui_draw_rect(ctx, checkmark, check_color);
  }
  
  // Draw label
  rect label_bounds = {
    {bounds.pos.x + box_size + 5.0f, bounds.pos.y},
    {bounds.size.x - box_size - 5.0f, bounds.size.y}
  };
  color text_color = {1.0f, 1.0f, 1.0f, 1.0f};
  ui_label(ctx, label, label_bounds, text_color);
  
  return value_changed;
}

bool ui_text_input(UIContext* ctx, const char* id, rect bounds, char* buffer, size_t buffer_size) {
  ui_id widget_id = hash_string(id);
  bool value_changed = false;
  
  // Check interaction
  bool hovered = ui_is_hovered(ctx, bounds);
  bool clicked = ui_is_clicked(ctx, bounds, 0);

  // Handle focus
  if (clicked) {
    ctx->focused_widget = widget_id;
  }
  
  bool is_focused = (ctx->focused_widget == widget_id);
  
  // Process keyboard input if focused
  if (is_focused) {
    size_t len = 0;
    while (buffer[len] != '\0') len++;
    
    // Handle backspace
    if (ctx->key_backspace && len > 0) {
      buffer[len - 1] = '\0';
      value_changed = true;
    }
    
    // Handle character input
    for (int i = 0; i < ctx->input_char_count; i++) {
      if (len < buffer_size - 1) {
        buffer[len++] = ctx->input_chars[i];
        buffer[len] = '\0';
        value_changed = true;
      }
    }
  }
  
  // Draw border first
  rect border = {
    {bounds.pos.x - 1, bounds.pos.y - 1},
    {bounds.size.x + 2, bounds.size.y + 2}
  };
  
  color border_color = is_focused ? 
    (color){0.3f, 0.3f, 0.8f, 1.0f} : 
    (color){0.0f, 0.0f, 0.0f, 1.0f};
  
    ui_draw_rect(ctx, border, border_color);
  
  // Draw background on top
  color bg_color = {0.15f, 0.15f, 0.15f, 1.0f};
  
  if (is_focused) {
    bg_color.r += 0.05f;
    bg_color.g += 0.05f;
    bg_color.b += 0.15f;
  } else if (hovered) {
    bg_color.r += 0.05f;
    bg_color.g += 0.05f;
    bg_color.b += 0.05f;
  }

  ui_draw_rect(ctx, bounds, bg_color);
  
  // Calculate text scrolling for overflow
  float text_width = ui_measure_text(ctx, buffer);
  float available_width = bounds.size.x - 10.0f; // 5px padding on each side
  float text_offset = 0.0f;
  
  // If text is wider than available space, scroll to show the end (where cursor is)
  if (text_width > available_width) {
    text_offset = available_width - text_width;
  }
  
  // Enable scissor test to clip text to bounds
  glEnable(GL_SCISSOR_TEST);
  glScissor((int)(bounds.pos.x + 5.0f), 
            (int)(ctx->height - (bounds.pos.y + bounds.size.y)), 
            (int)(bounds.size.x - 10.0f), 
            (int)bounds.size.y);
  
  // Draw text content with scroll offset
  vec2 text_pos = {
    bounds.pos.x + 5.0f + text_offset,
    bounds.pos.y + bounds.size.y * 0.5f + 4.0f
  };
  color text_color = {1.0f, 1.0f, 1.0f, 1.0f};
  ui_draw_text(ctx, buffer, text_pos, text_color);
  
  // Draw cursor if focused
  if (is_focused) {
    // Cursor at the end of text
    float cursor_x = bounds.pos.x + 5.0f + text_offset + text_width;
    // Blink cursor using time
    if (((int)(ctx->time * 2)) % 2 == 0) {
      rect cursor = {
        {cursor_x, bounds.pos.y + 4.0f},
        {2.0f, bounds.size.y - 8.0f}
      };
      color cursor_color = {1.0f, 1.0f, 1.0f, 1.0f};
      ui_draw_rect(ctx, cursor, cursor_color);
    }
  }
  
  // Disable scissor test
  glDisable(GL_SCISSOR_TEST);
  
  return value_changed;
}

void ui_panel_begin(UIContext *ctx, const char *id, rect bounds, color bg_color) {
  static bool is_printed = false;

  if (!is_printed) {
    is_printed = true;
  }
  
  // Push layout offset for child widgets
  vec2 panel_offset = {bounds.pos.x, bounds.pos.y};
  ui_push_layout(ctx, panel_offset);

  // Draw panel background
  ui_draw_rect(ctx, bounds, bg_color);
}

void ui_panel_end(UIContext* ctx) {
  // Pop the layout offset
  ui_pop_layout(ctx);
}

rect ui_layout_rect(UIContext* ctx, float x, float y, float w, float h) {
  // Apply current layout offset
  vec2 offset = {0, 0};
  if (ctx->layout_stack_size > 0) {
    offset = ctx->layout_stack[ctx->layout_stack_size - 1];
  }
  
  return (rect){{x + offset.x, y + offset.y}, {w, h}};
}

void ui_push_layout(UIContext* ctx, vec2 pos) {
  if (ctx->layout_stack_size < 32) {
    ctx->layout_stack[ctx->layout_stack_size++] = pos;
  }
}

void ui_pop_layout(UIContext* ctx) {
  if (ctx->layout_stack_size > 0) {
    ctx->layout_stack_size--;
  }
}