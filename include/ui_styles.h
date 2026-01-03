#ifndef UI_TEXT_H
#define UI_TEXT_H

#include <ui_core.h>

typedef struct {
  color color;
  int line_height; 
  int font_size;
  // TODO: font family
} TextStyle;

typedef struct {
  int   radius;
  color color;
} RectStyle;

#endif