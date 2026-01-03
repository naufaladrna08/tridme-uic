#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <ui_core.h>
#include <ui_styles.h>

// Widget IDs
typedef uint32_t ui_id;

/*
 * @brief Render a button widget and handle interaction
 *
 * Renders a clickable button with a background, border and centered label.
 * Tracks hover/click state and returns whether the button was clicked.
 *
 * @param ctx The UI context used for drawing and input state
 * @param label The text label to display inside the button
 * @param bounds The rectangle specifying button position and size
 * @return true if the button was clicked this frame, false otherwise
 */
UI_API bool ui_button(UIContext* ctx, const char* label, rect bounds);

/*
 * @brief Draw a simple centered text label
 *
 * Measures the text and draws it centered within the provided bounds.
 * This function does not perform layout; it only renders the given text.
 *
 * @param ctx The UI context used for drawing and font measurement
 * @param text The text to draw
 * @param bounds The rectangle within which to center the text
 * @param text_color The color to draw the text with
 * @return void
 */
UI_API void ui_label(UIContext* ctx, const char* text, rect bounds, color text_color);

/*
 * @brief Draw a text label using a provided TextStyle
 *
 * Similar to ui_label but accepts a TextStyle structure (which includes
 * color, font size, etc.). Centers the text within bounds and draws it
 * using style.color.
 *
 * @param ctx The UI context used for drawing and font measurement
 * @param text The text to draw
 * @param bounds The rectangle within which to center the text
 * @param style The TextStyle that specifies color and other text properties
 * @return void
 */
UI_API void ui_label_with_style(UIContext* ctx, const char* text, rect bounds, TextStyle style);

/*
 * @brief Render and handle a horizontal float slider
 *
 * Renders a track and thumb, handles mouse interaction and dragging. Updates
 * the value in-place and returns whether it changed this frame.
 *
 * @param ctx The UI context with input state and drawing helpers
 * @param id A unique id string for the widget (used for focus/active tracking)
 * @param bounds The rectangle for the slider track
 * @param value Pointer to the float value to read/update
 * @param min_val Minimum allowed value
 * @param max_val Maximum allowed value
 * @return true if the value was changed by user interaction, false otherwise
 */
UI_API bool ui_slider_float(UIContext* ctx, const char* id, rect bounds, float* value, 
  float min_val, float max_val);

/*
 * @brief Render a checkbox with a label and toggle state
 *
 * Draws a square checkbox on the left and the label on the right. Handles
 * clicks to toggle the boolean state. The checked value is stored via the
 * provided pointer and the function returns whether the value changed.
 *
 * @param ctx The UI context used for drawing and input
 * @param label The label text to display next to the checkbox
 * @param bounds The rectangle area for the checkbox + label
 * @param checked Pointer to the boolean state that will be toggled
 * @return true if the checked state changed this frame, false otherwise
 */
UI_API bool ui_checkbox(UIContext* ctx, const char* label, rect bounds, bool* checked);

/*
 * @brief Render a basic single-line text input and handle keyboard input
 *
 * Draws an editable text field, handles focus, backspace and character
 * input, and performs simple horizontal scrolling when the text overflows
 * the available width. Updates the provided buffer in-place.
 *
 * @param ctx The UI context providing input and drawing utilities
 * @param id A unique id string for the widget (used for focus management)
 * @param bounds The rectangle specifying the input position and size
 * @param buffer Pointer to a char buffer holding the current text (null-terminated)
 * @param buffer_size Size of the buffer in bytes (including null terminator)
 * @return true if the buffer content changed this frame, false otherwise
 */
UI_API bool ui_text_input(UIContext* ctx, const char* id, rect bounds, 
  char* buffer, size_t buffer_size);

/*
 * @brief Begin a panel region and push a layout offset
 *
 * Draws a panel background and pushes the panel position onto the layout
 * stack so child widgets can position themselves relative to the panel.
 *
 * @param ctx The UI context used for drawing and layout management
 * @param id A unique id for the panel (currently unused but reserved)
 * @param bounds The rectangle area of the panel
 * @param bg_color Background color used to draw the panel
 * @return void
 */
UI_API void ui_panel_begin(UIContext* ctx, const char* id, rect bounds, color bg_color);

/*
 * @brief End a panel region and restore previous layout
 *
 * Pops the last layout offset pushed by ui_panel_begin so subsequent widgets
 * are positioned in the previous layout context.
 *
 * @param ctx The UI context used for layout management
 * @return void
 */
UI_API void ui_panel_end(UIContext* ctx);

/*
 * @brief Compute a rectangle adjusted by the current layout offset
 *
 * Returns a rect shifted by the top of the layout stack (if any). This is
 * used by widgets to position themselves relative to the current layout
 * origin provided by panels or other layout pushes.
 *
 * @param ctx The UI context containing the layout stack
 * @param x X position relative to the current layout origin
 * @param y Y position relative to the current layout origin
 * @param w Width of the rectangle
 * @param h Height of the rectangle
 * @return The computed rect with the layout offset applied
 */
UI_API rect ui_layout_rect(UIContext* ctx, float x, float y, float w, float h);

/*
 * @brief Push a layout offset onto the UI context stack
 *
 * Adds a new offset so subsequent calls to ui_layout_rect will be relative
 * to this position. The stack has a fixed maximum depth (32) to prevent
 * unbounded growth.
 *
 * @param ctx The UI context managing the layout stack
 * @param pos The offset position to push
 * @return void
 */
UI_API void ui_push_layout(UIContext* ctx, vec2 pos);

/*
 * @brief Pop the most recent layout offset
 *
 * Restores the previous layout origin by decreasing the layout stack size.
 * Safe to call only when there is at least one pushed layout offset.
 *
 * @param ctx The UI context managing the layout stack
 * @return void
 */
UI_API void ui_pop_layout(UIContext* ctx);

#endif