#ifndef UI_HELPER_H
#define UI_HELPER_H
#include <string.h>
#include <ui_core.h>
#include <stdlib.h>

/*
 * if first string = "#", then remove first char
 * loop through the string, take 2 char, strtol and / 255 put it in the 
 */
static inline color color_from_hex(const char* hex) {
  color out = {0, 0, 0, 255}; // default alpha

  if (!hex) return out;

  int offset = (hex[0] == '#') ? 1 : 0;

  int len = strlen(hex + offset);
  if (len != 6 && len != 8) {
    // invalid format
    return out;
  }

  char buf[3] = {0};

  buf[0] = hex[offset + 0];
  buf[1] = hex[offset + 1];
  out.r = (float) strtol(buf, NULL, 16) / 255.0f;

  buf[0] = hex[offset + 2];
  buf[1] = hex[offset + 3];
  out.g = (float) strtol(buf, NULL, 16) / 255.0f;

  buf[0] = hex[offset + 4];
  buf[1] = hex[offset + 5];
  out.b = (float)strtol(buf, NULL, 16) / 255.0f;

  if (len == 8) {
    buf[0] = hex[offset + 6];
    buf[1] = hex[offset + 7];
    out.a = (float) strtol(buf, NULL, 16) / 255.0f;
  }

  return out;
}


#endif