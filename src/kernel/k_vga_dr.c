#include "k_vga_dr.h"

#define MAX_CURSOR_POS (VGA_TEXT_BUFFER_WIDTH * VGA_TEXT_BUFFER_HEIGHT)
static uint16_t cursor_pos = 0;
void cursor_update(void)
{
  outb(0x3d4, 0x0f);
  outb(0x3d5, (uint8_t) (cursor_pos & 0xff));
  outb(0x3d4, 0x0e);
  outb(0x3d5, (uint8_t) ((cursor_pos >> 8) & 0xff));
}
void cursor_set(uint8_t x, uint8_t y)
{
  cursor_pos = y * VGA_TEXT_BUFFER_WIDTH + x;
  cursor_update();
}
bool cursor_new_line(void)
{
  uint16_t new = (1 + cursor_pos / VGA_TEXT_BUFFER_WIDTH) * VGA_TEXT_BUFFER_WIDTH;
  if (new < MAX_CURSOR_POS) cursor_pos = new;
  return (new >= MAX_CURSOR_POS);
}
bool cursor_advance(void)
{
  if (cursor_pos + 1 >= MAX_CURSOR_POS) return true;
  cursor_pos++;
  return false;
}

void k_print_vga_cls(enum VGA_COLOUR_4 cls_colour)
{
  volatile uint16_t* vga_buf = (volatile uint16_t*)VGA_TEXT_BUFFER;
  uint16_t counter = MAX_CURSOR_POS;
  while (counter--)
  {
    *vga_buf++ = (cls_colour << 12) | ' ';
  }
  cursor_set(0, 0);
}

void k_print_vga_str(const char* str, enum VGA_COLOUR_4 bg, enum VGA_COLOUR_4 fg)
{
  volatile uint16_t* vga_buf = (volatile uint16_t*)VGA_TEXT_BUFFER + cursor_pos;
  unsigned char c;
  bool overflow = false;
  while ((c = (unsigned char)*str++))
  {
    if (c == '\n') 
    {
      overflow = cursor_new_line();
      vga_buf = (volatile uint16_t*)VGA_TEXT_BUFFER + cursor_pos;
      continue;
    }
    if (overflow) break;
    *vga_buf++ = (bg << 12) | (fg << 8) | c;
    overflow = cursor_advance();
  }
  cursor_update();
}
