#include <ui_core.h>
#include <ui_shaders.h>
#include <ui_widgets.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// Vertex structure for UI rendering
typedef struct {
  vec2 position;
  vec2 texcoord;
  color col;
} ui_vertex;

static unsigned int create_texture_atlas(UIContext* ctx) {
  // Initialize FreeType
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Could not init FreeType Library\n");
    return 0;
  }
  
  // Load font
  FT_Face face;
  const char* font_paths[] = {
    "HelveticaNeueRoman.otf",
    "../HelveticaNeueRoman.otf",
    "../../HelveticaNeueRoman.otf",
    "/home/naufal/Documents/Projects/C_CXX_Projects/tridme-uic/HelveticaNeueRoman.otf"
  };
  
  int loaded = 0;
  for (int i = 0; i < 4; i++) {
    if (FT_New_Face(ft, font_paths[i], 0, &face) == 0) {
      loaded = 1;
      break;
    }
  }
  
  if (!loaded) {
    fprintf(stderr, "Failed to load font from any path\\n");
    FT_Done_FreeType(ft);
    return 0;
  }
  
  // Set pixel size
  FT_Set_Pixel_Sizes(face, 0, 16);
  
  // Create atlas dimensions
  const int atlas_width = 512;
  const int atlas_height = 512;
  unsigned char* atlas_data = (unsigned char*)calloc(atlas_width * atlas_height, 1);
  
  // Render ASCII characters to atlas
  int pen_x = 0;
  int pen_y = 0;
  int row_height = 0;
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  for (unsigned char c = 32; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      fprintf(stderr, "Failed to load Glyph %c\n", c);
      continue;
    }
    
    FT_GlyphSlot g = face->glyph;
    
    // If we go past atlas width, move to next row
    if (pen_x + g->bitmap.width >= atlas_width) {
      pen_x = 0;
      pen_y += row_height;
      row_height = 0;
    }
    
    // Copy glyph bitmap to atlas
    for (unsigned int y = 0; y < g->bitmap.rows; y++) {
      for (unsigned int x = 0; x < g->bitmap.width; x++) {
        int atlas_x = pen_x + x;
        int atlas_y = pen_y + y;
        if (atlas_x < atlas_width && atlas_y < atlas_height) {
          atlas_data[atlas_y * atlas_width + atlas_x] = g->bitmap.buffer[y * g->bitmap.width + x];
        }
      }
    }
    
    // Store glyph info
    ctx->glyphs[c].x = (float)pen_x / atlas_width;
    ctx->glyphs[c].y = (float)pen_y / atlas_height;
    ctx->glyphs[c].width = (float)g->bitmap.width / atlas_width;
    ctx->glyphs[c].height = (float)g->bitmap.rows / atlas_height;
    ctx->glyphs[c].advance = g->advance.x >> 6; // Convert from 1/64th pixels
    ctx->glyphs[c].offset_x = g->bitmap_left;
    ctx->glyphs[c].offset_y = -g->bitmap_top;
    
    // Move pen for next glyph
    pen_x += g->bitmap.width + 1; // 1 pixel padding
    if (g->bitmap.rows > row_height) {
      row_height = g->bitmap.rows + 1;
    }
  }
  
  ctx->font_size = 16.0f;
  
  // Clean up FreeType
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  
  // Create OpenGL texture
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas_data);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Reset to default
  
  free(atlas_data);
  return texture;
}

UIContext* ui_create_context(int window_width, int window_height) {
  UIContext* ctx = (UIContext*)calloc(1, sizeof(UIContext));
  if (!ctx) return NULL;
  
  ctx->width = window_width;
  ctx->height = window_height;
  ctx->shader = ui_create_ui_shader();
  
  // Setup buffers
  glGenVertexArrays(1, &ctx->vao);
  glGenBuffers(1, &ctx->vbo);
  glGenBuffers(1, &ctx->ebo);
  
  glBindVertexArray(ctx->vao);
  glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ebo);
  
  // Vertex attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)0);
  
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), 
                        (void*)offsetof(ui_vertex, texcoord));
  
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex),
                        (void*)offsetof(ui_vertex, col));
  
  // Create texture atlas (must be done after ctx is allocated)
  ctx->texture_atlas = create_texture_atlas(ctx);
  
  // Create a white pixel texture for solid colors
  glGenTextures(1, &ctx->white_texture);
  glBindTexture(GL_TEXTURE_2D, ctx->white_texture);
  unsigned char white_pixel[4] = {255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  return ctx;
}

void ui_destroy_context(UIContext* ctx) {
  if (!ctx) return;
  
  glDeleteTextures(1, &ctx->texture_atlas);
  glDeleteTextures(1, &ctx->white_texture);
  glDeleteBuffers(1, &ctx->vbo);
  glDeleteBuffers(1, &ctx->ebo);
  glDeleteVertexArrays(1, &ctx->vao);
  glDeleteProgram(ctx->shader);
  
  free(ctx);
}

void ui_begin_frame(UIContext* ctx, float delta_time) {
  ctx->delta_time = delta_time;
  ctx->time += delta_time;
  
  // Reset scroll offset
  ctx->scroll_offset = 0;
  
  // Enable blending for UI
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  
  // Setup shader and projection
  glUseProgram(ctx->shader);
  
  // Create orthographic projection matrix (column-major)
  float ortho[16] = {
    2.0f / ctx->width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / ctx->height, 0.0f, 0.0f,
    0.0f, 0.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f
  };
  
  ui_set_uniform_mat4(ctx->shader, "projection", ortho);
  
  // Reset hot widget if no button is pressed
  bool any_mouse_down = false;
  for (int i = 0; i < 3; i++) {
    if (ctx->mouse_buttons[i]) {
      any_mouse_down = true;
      break;
    }
  }
  
  if (!any_mouse_down) {
    ctx->hot_widget = 0;
  }
}

void ui_end_frame(UIContext* ctx) {
  // Save current mouse state for next frame's click detection
  memcpy(ctx->prev_mouse_buttons, ctx->mouse_buttons, sizeof(ctx->mouse_buttons));
  
  // Reset keyboard input for next frame
  ctx->input_char_count = 0;
  ctx->key_backspace = false;
  ctx->key_delete = false;
  ctx->key_left = false;
  ctx->key_right = false;
}

bool ui_is_hovered(UIContext* ctx, rect area) {
  return (ctx->mouse_pos.x >= area.pos.x &&
          ctx->mouse_pos.x <= area.pos.x + area.size.x &&
          ctx->mouse_pos.y >= area.pos.y &&
          ctx->mouse_pos.y <= area.pos.y + area.size.y);
}

bool ui_is_clicked(UIContext* ctx, rect area, int button) {
  return ui_is_hovered(ctx, area) && 
    ctx->mouse_buttons[button] && 
    !ctx->prev_mouse_buttons[button];
}

vec2 ui_get_mouse_pos(UIContext* ctx) {
  return ctx->mouse_pos;
}

// Drawing functions
void ui_draw_rect(UIContext* ctx, rect r, color c) {
  // Create vertices with complete data (position, texcoord, color)
  ui_vertex vertices[4] = {
    {{r.pos.x, r.pos.y}, {0.0f, 0.0f}, c},
    {{r.pos.x + r.size.x, r.pos.y}, {1.0f, 0.0f}, c},
    {{r.pos.x + r.size.x, r.pos.y + r.size.y}, {1.0f, 1.0f}, c},
    {{r.pos.x, r.pos.y + r.size.y}, {0.0f, 1.0f}, c}
  };
  
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};
  
  // Bind VAO and shader
  glBindVertexArray(ctx->vao);
  glUseProgram(ctx->shader);
  
  // Upload to GPU
  glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
  
  // Set color uniform
  ui_set_uniform_vec4(ctx->shader, "color", c.r, c.g, c.b, c.a);
  
  // Bind white texture for solid colors
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ctx->white_texture);
  ui_set_uniform_int(ctx->shader, "tex", 0);
  
  // Draw
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ui_set_mouse_position(UIContext* ctx, float x, float y) {
  ctx->mouse_pos.x = x;
  ctx->mouse_pos.y = y;
}

void ui_set_mouse_button(UIContext* ctx, int button, bool pressed) {
  if (button >= 0 && button < 3) {
    ctx->mouse_buttons[button] = pressed;
  }
}

void ui_set_key(UIContext* ctx, int key, bool pressed) {
  if (!pressed) return; // Only handle key press
  
  // GLFW key codes
  if (key == 259) { // GLFW_KEY_BACKSPACE
    ctx->key_backspace = true;
  } else if (key == 261) { // GLFW_KEY_DELETE
    ctx->key_delete = true;
  } else if (key == 263) { // GLFW_KEY_LEFT
    ctx->key_left = true;
  } else if (key == 262) { // GLFW_KEY_RIGHT
    ctx->key_right = true;
  }
}

void ui_set_scroll(UIContext* ctx, float offset) {
  ctx->scroll_offset = offset;
}

void ui_input_char(UIContext* ctx, unsigned int codepoint) {
  if (ctx->input_char_count < 32 && codepoint < 128 && codepoint >= 32) {
    ctx->input_chars[ctx->input_char_count++] = (char)codepoint;
  }
}

void ui_draw_text(UIContext* ctx, const char* text, vec2 position, color c) {
  if (!text || !*text) return;
  
  float x = position.x;
  float y = position.y;
  
  glBindVertexArray(ctx->vao);
  glUseProgram(ctx->shader);
  
  // Bind font atlas
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ctx->texture_atlas);
  ui_set_uniform_int(ctx->shader, "tex", 0);
  
  // Set text color (use the provided color)
  ui_set_uniform_vec4(ctx->shader, "color", c.r, c.g, c.b, c.a);
  
  for (const char* p = text; *p; p++) {
    unsigned char ch = *p;
    if (ch < 32 || ch >= 128) continue;
    
    ui_glyph_info* glyph = &ctx->glyphs[ch];
    
    // Use actual glyph dimensions from atlas
    float glyph_width = glyph->width * 512.0f;  // Scale back to pixels (atlas is 512x512)
    float glyph_height = glyph->height * 512.0f;
    
    float x0 = x + glyph->offset_x;
    float y0 = y + glyph->offset_y;
    float x1 = x0 + glyph_width;
    float y1 = y0 + glyph_height;
    
    float u0 = glyph->x;
    float v0 = glyph->y;
    float u1 = glyph->x + glyph->width;
    float v1 = glyph->y + glyph->height;
    
    // Create vertices for this character
    ui_vertex vertices[4] = {
      {{x0, y0}, {u0, v0}, c},
      {{x1, y0}, {u1, v0}, c},
      {{x1, y1}, {u1, v1}, c},
      {{x0, y1}, {u0, v1}, c}
    };
    
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    
    // Upload and draw
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    x += glyph->advance;
  }
}

float ui_measure_text(UIContext* ctx, const char* text) {
  if (!text || !*text) return 0.0f;
  
  float width = 0.0f;
  for (const char* p = text; *p; p++) {
    unsigned char ch = *p;
    if (ch < 32 || ch >= 128) continue;
    width += ctx->glyphs[ch].advance;
  }
  return width;
}