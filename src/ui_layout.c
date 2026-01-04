#include <ui_layout.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_VBOXES 32
static VBoxState g_vboxes[MAX_VBOXES];
static int g_active_vbox_count = 0;

// Find or create vbox state
static VBoxState* get_vbox_state(const char* id) {
  // Hash the ID to find slot (simplified)
  uint32_t hash = 0;
  for (const char* p = id; *p; p++) {
    hash = (hash * 31) + *p;
  }
  int slot = hash % MAX_VBOXES;
  
  // Linear probe if slot is occupied by different ID
  for (int i = 0; i < MAX_VBOXES; i++) {
    int idx = (slot + i) % MAX_VBOXES;
    if (!g_vboxes[idx].is_active || strcmp(g_vboxes[idx].id, id) == 0) {
      return &g_vboxes[idx];
    }
  }
  return NULL; // No space (shouldn't happen with reasonable usage)
}

void ui_vbox_begin_ex(UIContext* ctx, const char* id, rect bounds, const VBoxConfig* config) {
  VBoxState* vbox = get_vbox_state(id);
  if (!vbox) return;
  
  // Initialize or reset
  vbox->is_active = true;
  vbox->cursor = (vec2){bounds.pos.x + (config ? config->padding_left : 0),
                        bounds.pos.y + (config ? config->padding_top : 0)};
  vbox->max_widget_width = 0;
  vbox->total_height = 0;
  vbox->widget_count = 0;
  
  if (config) {
      vbox->config = *config;
  } else {
      // Default configuration
      vbox->config = (VBoxConfig){
          .spacing = 4.0f,
          .padding_top = 0.0f,
          .padding_bottom = 0.0f,
          .padding_left = 0.0f,
          .padding_right = 0.0f,
          .expand_width = false,
          .fixed_height = 0.0f,
          .clip_overflow = false
      };
  }
  
  // Store ID
  size_t len = strlen(id);
  vbox->id = malloc(len + 1);
  strcpy(vbox->id, id);
  
  // Set clipping if enabled
  if (vbox->config.clip_overflow) {
    ui_push_clip(ctx, bounds);
  }
  
  // Push this vbox as current layout
  ui_push_layout(ctx, vbox->cursor);
}

rect ui_vbox_next(UIContext* ctx, const char* id, float widget_height) {
  VBoxState* vbox = get_vbox_state(id);
  if (!vbox || !vbox->is_active) {
      return (rect){0, 0, 0, 0};
  }
  
  // Calculate widget width
  float widget_width;
  if (vbox->config.fixed_height > 0) {
      // Fixed height mode
      widget_width = vbox->config.fixed_height - 
                    vbox->config.padding_left - vbox->config.padding_right;
  } else {
      // Auto width (use available space)
      widget_width = ctx->width - 
                     vbox->config.padding_left - vbox->config.padding_right;
  }
  
  if (vbox->config.expand_width) {
      widget_width = widget_width; // Use full width
  }
  
  // Create rect for this widget
  rect widget_rect = {
    .pos.x = vbox->cursor.x,
    .pos.y = vbox->cursor.y,
    .size.x = widget_width,
    .size.y = widget_height
  };
  
  // Update vbox state
  vbox->max_widget_width = fmaxf(vbox->max_widget_width, widget_width);
  vbox->total_height += widget_height + 
                        (vbox->widget_count > 0 ? vbox->config.spacing : 0);
  vbox->widget_count++;
  
  // Move cursor for next widget
  vbox->cursor.y += widget_height + vbox->config.spacing;
  
  return widget_rect;
}

void ui_vbox_end(UIContext* ctx, const char* id) {
  VBoxState* vbox = get_vbox_state(id);
  if (!vbox || !vbox->is_active) return;
  
  // Pop the layout we pushed in begin
  ui_pop_layout(ctx);
  
  // Pop clip if we pushed one
  if (vbox->config.clip_overflow) {
    ui_pop_clip(ctx);
  }
  
  // Cleanup
  free(vbox->id);
  vbox->is_active = false;
  vbox->id = NULL;
}