#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <ui_core.h>
#include <ui_widgets.h>

/*
 * Vertical Box Layout, a Layout that manage widgets by placing them in a columns.
 * 
 * Example of usage:
 *   ui_begin_vbox(ui); <- This will create new vbox layout. You can configure the
 *                         parameters like how you want the widgets to expand, and 
 *                         etc. The default height of a Vbox is adjusted by the max
 *                         height of it's widgets. You can override the height and
 *                         Force the widget to follow the layout's height. 
 *   ... Render some widgets. 
 *   ui_end_vbox();
 */

typedef struct VBoxConfig {
  float spacing;          // Space between widgets (default: 4.0f)
  float padding_top;      // Padding inside vbox (default: 0.0f)
  float padding_bottom;
  float padding_left;
  float padding_right;
  bool expand_width;      // Should widgets expand to fill width? (default: false)
  float fixed_height;     // If > 0, use fixed height instead of auto (default: 0.0f)
  bool clip_overflow;     // Clip widgets outside bounds? (default: false)
} VBoxConfig;

typedef struct VBoxState {
  char* id;
  vec2 cursor;            // Current position for next widget
  float max_widget_width; // Maximum widget width seen so far
  float total_height;     // Total height of all widgets + spacing
  int widget_count;       // Number of widgets added
  bool is_active;         // Is this vbox currently being used?
  VBoxConfig config;      // Configuration for this vbox
} VBoxState;

// Public API
UI_API void ui_vbox_begin_ex(UIContext* ctx, const char* id, rect bounds, const VBoxConfig* config);
UI_API rect ui_vbox_next(UIContext* ctx, const char* id, float widget_height);
UI_API void ui_vbox_end(UIContext* ctx, const char* id);

// Convenience macros
#define ui_begin_vbox(ctx, id, bounds) ui_vbox_begin_ex(ctx, id, bounds, NULL)
#define ui_next_vbox_widget(ctx, id, height) ui_vbox_next(ctx, id, height)

#endif