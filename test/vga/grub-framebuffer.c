#include <core/multiboot.h>
#include <drivers/video/grub-framebuffer/grub-framebuffer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  describe("MAKE_RGBA");
  uint32_t rgba = MAKE_RGBA(0x1, 0x2, 0x3, 0x4);
  assert(rgba == 0x04010203, "Expected a valid RGBA");
  end_describe();

  describe("MAKE_RGB");
  uint32_t rgb = MAKE_RGB(0x1, 0x2, 0x3);
  assert(rgb == 0xFF010203, "Expected a valid RGB");
  end_describe();

  return test_summary();
}
