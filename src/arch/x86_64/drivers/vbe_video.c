#include "vbe_video.h"
#include <graphics/video_api.h>
#include <logging.h>

void vbe_swap_buffers()
{
  uint32_t framebuffer_pixel_size = video_driver.width * video_driver.height;

  memcpy(video_driver.native_buffer,
         video_driver.buffer,
         framebuffer_pixel_size * sizeof(uint32_t));
}

void vbe_video_init(multiboot_tag_framebuffer_common_t* common)
{
  video_driver.width = (int32_t)common->framebuffer_width;
  video_driver.height = (int32_t)common->framebuffer_height;
  video_driver.native_buffer = common->framebuffer_addr;
  video_driver.buffer = (uint32_t*)malloc(
    common->framebuffer_width * common->framebuffer_height * sizeof(uint32_t));
  video_driver.swap_buffers = &vbe_swap_buffers;
}
