#include <core/multiboot.h>
#include <drivers/video/video_api.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  video_driver.video_width = video_driver.video_height = 16;
  uint32_t video_size = video_driver.video_width * video_driver.video_height;
  video_driver.video_buffer = (uint32_t*)malloc(sizeof(uint32_t) * video_size);

  describe("returns video sizes");
  assert(video_width() == 16, "Expected the right video width");
  assert(video_height() == 16, "Expected the right video height");
  end_describe();

  describe("clear RGB FFFF0000");
  video_clear(0xFFFF0000);

  for (uint32_t i = 0; i < video_size; i++) {
    assert(video_driver.video_buffer[i] == 0xFFFF0000,
           "Expected a valid pixel");
  }
  end_describe();

  describe("clear half region FF00FF00 with full size should not crash");
  video_clear_region(
    0xFF00FF00, video_width() / 2, 0, video_width(), video_height());

  for (uint32_t y = 0, y_index = 0; y < video_height();
       y++, y_index += video_width()) {
    for (uint32_t x = 0; x < video_width(); x++) {
      uint32_t index = x + y_index;

      if (x < video_width() / 2) {
        assert(video_driver.video_buffer[index] == 0xFFFF0000,
               "Expected a valid clear pixel");
      } else {
        assert(video_driver.video_buffer[index] == 0xFF00FF00,
               "Expected a valid region pixel");
      }
    }
  }
  end_describe();

  describe("blits a buffer with 4x4 pixels properly");

  video_clear(0);

  uint32_t blit_buffer[] = {

    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
  };

  video_blit(blit_buffer, 0, 0, 4, 4, 4, 4, 0, 0);

  for (uint32_t y = 0, y_index = 0; y < video_height();
       y++, y_index += video_width()) {
    for (uint32_t x = 0; x < video_width(); x++) {
      uint32_t index = x + y_index;

      if (x < 4 && y < 4) {
        uint32_t buffer_pixel = blit_buffer[x + y * 4];

        assert(video_driver.video_buffer[index] == buffer_pixel,
               "Expected the right blit pixel");
      } else {
        assert(video_driver.video_buffer[index] == 0, "Expected a clear pixel");
      }
    }
  }

  end_describe();

  describe("blits a buffer with 4x4 pixels crossing the boundary of the screen properly");

  video_clear(0);

  video_blit(blit_buffer, 0, 0, 4, 4, 4, 4, -2, -2);

  for (uint32_t y = 0, y_index = 0; y < video_height();
       y++, y_index += video_width()) {
    for (uint32_t x = 0; x < video_width(); x++) {
      uint32_t index = x + y_index;

      if (x < 2 && y < 2) {
        uint32_t buffer_pixel = blit_buffer[x + 2 + (y + 2) * 4];

        assert(video_driver.video_buffer[index] == buffer_pixel,
               "Expected the right blit pixel");
      } else {
        assert(video_driver.video_buffer[index] == 0, "Expected a clear pixel");
      }
    }
  }

  end_describe();

  describe("blits half the 4x4 pixel buffer properly");

  video_clear(0);

  video_blit(blit_buffer, 2, 2, 2, 2, 4, 4, 0, 0);

  for (uint32_t y = 0, y_index = 0; y < video_height();
       y++, y_index += video_width()) {
    for (uint32_t x = 0; x < video_width(); x++) {
      uint32_t index = x + y_index;

      if (x < 2 && y < 2) {
        uint32_t buffer_pixel = blit_buffer[x + 2 + (y + 2) * 4];

        assert(video_driver.video_buffer[index] == buffer_pixel,
               "Expected the right blit pixel");
      } else {
        assert(video_driver.video_buffer[index] == 0, "Expected a clear pixel");
      }
    }
  }

  end_describe();

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
