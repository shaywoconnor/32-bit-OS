#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "k_basic_io.h"

#define VGA_TEXT_BUFFER 0xb8000
#define VGA_TEXT_BUFFER_WIDTH 80
#define VGA_TEXT_BUFFER_HEIGHT 25
#define VGA_TEXT_ATTRIB_BLINK 0b10000000

enum VGA_COLOUR_4
{
  VGA_COLOUR_BLACK = 0,
  VGA_COLOUR_BLUE = 1,
  VGA_COLOUR_GREEN = 2,
  VGA_COLOUR_CYAN = 3,
  VGA_COLOUR_RED = 4,
  VGA_COLOUR_MAGENTA = 5,
  VGA_COLOUR_BROWN = 6,
  VGA_COLOUR_LIGHT_GREY = 7,
  VGA_COLOUR_DARK_GREY = 8,
  VGA_COLOUR_LIGHT_BLUE = 9,
  VGA_COLOUR_LIGHT_GREEN = 10,
  VGA_COLOUR_LIGHT_CYAN = 11,
  VGA_COLOUR_LIGHT_RED = 12,
  VGA_COLOUR_LIGHT_MAGENTA = 13,
  VGA_COLOUR_LIGHT_BROWN = 14,
  VGA_COLOUR_WHITE = 15
};

typedef uint8_t vga_text_attrib_t;

void k_print_vga_str(const char* str, enum VGA_COLOUR_4 bg, enum VGA_COLOUR_4 fg);
void k_print_vga_cls(enum VGA_COLOUR_4 cls_colour);
void k_print_vga_init(void);

#endif // VGA_DRIVER_H
