#ifndef K_BASIC_IO_H
#define K_BASIC_IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data)
{
  __asm__ volatile ( "outb %b0, %w1" : : "a"(data), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
  uint8_t in;
  __asm__ volatile
  (
      "inb %w1, %b0"
    : "=a"(in)
    : "Nd"(port)
    : "memory"
  );
  return in;
}

#endif // K_BASIC_IO_H
