#include <core/logging.h>
#include <core/multiboot.h>
#include <ssfn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "video_api.h"

video_driver_t video_driver;
static uint32_t video_frames = 0;
static uint64_t video_frame_timer = 0;
static bool video_debug_stats_enabled = false;

extern multiboot_tag_framebuffer_common_t* multiboot_framebuffer_common;

uint32_t video_blend_alpha_callback(uint32_t src, uint32_t dst);

void video_clear(uint32_t color)
{
  uint32_t* buffer = video_driver.buffer;

  if (buffer == NULL) {
    return;
  }

  uint32_t buffer_size = video_driver.width * video_driver.height;
  for (uint32_t i = 0; i < buffer_size; i++) {
    buffer[i] = color;
  }
}

void video_clear_region(uint32_t color,
                        int32_t x,
                        int32_t y,
                        int32_t width,
                        int32_t height)
{
  if (width < 0 || height < 0) {
    return;
  }

  uint32_t* buffer = video_driver.buffer;

  if (buffer == NULL) {
    return;
  }

  for (int32_t y_pos = 0, y_index = y * video_driver.width; y_pos < height;
       y_pos++, y_index += video_driver.width) {

    if (y < 0 && y + y_pos < 0) {
      continue;
    }

    if (y + y_pos >= video_driver.height) {
      break;
    }

    for (int32_t x_pos = 0, x_index = x + y_index; x_pos < width;
         x_pos++, x_index++) {

      if (x < 0 && x + x_pos < 0) {
        continue;
      }

      if (x + x_pos >= video_driver.width) {
        break;
      }

      buffer[x_index] = color;
    }
  }
}

frame_buffer_area_t video_area_at(int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height)
{
  frame_buffer_area_t outArea = {
    .x = x,
    .y = y,
    .width = video_driver.width,
    .height = video_driver.height,
    .target_x = 0,
    .target_y = 0,
    .target_width = width,
    .target_height = height,
    .buffer = NULL,
  };

  if (x >= video_driver.width || y >= video_driver.height || width <= 0 ||
      height <= 0) {
    return outArea;
  }

  if (x < 0) {
    outArea.x = 0;
    outArea.target_x = -x;
    outArea.target_width = width + x;
  }

  if (y < 0) {
    outArea.y = 0;
    outArea.target_y = -y;
    outArea.target_height = height + y;
  }

  if (x + width > (int32_t)video_driver.width) {
    outArea.target_width = (int32_t)video_driver.width - outArea.x;
  }

  if (y + height > video_driver.height) {
    outArea.target_height = video_driver.height - outArea.y;
  }

  if (outArea.target_x < 0 || outArea.target_y < 0 ||
      outArea.target_width <= 0 || outArea.target_height <= 0) {
    return outArea;
  }

  outArea.buffer = video_driver.buffer;

  return outArea;
}

void video_blit(uint32_t* src_buffer,
                int32_t x,
                int32_t y,
                int32_t src_width,
                int32_t src_height,
                int32_t src_buffer_width,
                int32_t src_buffer_height)
{
  frame_buffer_area_t area = video_area_at(x, y, src_width, src_height);

  if (area.buffer == NULL) {
    return;
  }

  uint32_t* buffer = area.buffer;

  for (int32_t y_pos = 0,
               y_index = area.y * area.width,
               src_y_index = area.target_y * src_buffer_width;
       y_pos < area.target_height;
       y_pos++, y_index += area.width, src_y_index += src_buffer_width) {

    for (int32_t x_pos = 0,
                 x_index = y_index + area.x,
                 src_x_index = area.target_x + src_y_index;
         x_pos < area.target_width;
         x_pos++, x_index++, src_x_index++) {

      buffer[x_index] = src_buffer[src_x_index];
    }
  }
}

void video_blit_blend(uint32_t* src_buffer,
                      int32_t x,
                      int32_t y,
                      int32_t src_width,
                      int32_t src_height,
                      int32_t src_buffer_width,
                      int32_t src_buffer_height,
                      video_blend_type_t blend_type)
{
  uint32_t (*blend_function)(uint32_t, uint32_t) = NULL;

  switch (blend_type) {
    case video_blend_alpha:

      blend_function = video_blend_alpha_callback;

      break;

    default:

      video_blit(src_buffer,
                 x,
                 y,
                 src_width,
                 src_height,
                 src_buffer_width,
                 src_buffer_height);

      return;
  }

  frame_buffer_area_t area = video_area_at(x, y, src_width, src_height);

  if (area.buffer == NULL) {
    return;
  }

  uint32_t* buffer = area.buffer;

  for (int32_t y_pos = 0,
               y_index = area.y * area.width,
               src_y_index = area.target_y * src_buffer_width;
       y_pos < area.target_height;
       y_pos++, y_index += area.width, src_y_index += src_buffer_width) {

    for (int32_t x_pos = 0,
                 x_index = y_index + area.x,
                 src_x_index = area.target_x + src_y_index;
         x_pos < area.target_width;
         x_pos++, x_index++, src_x_index++) {

      buffer[x_index] =
        blend_function(src_buffer[src_x_index], buffer[x_index]);
    }
  }
}

void video_enable_debug_stats()
{
  video_debug_stats_enabled = true;
}

void video_disable_debug_stats()
{
  video_debug_stats_enabled = false;
}

void video_swap_buffers()
{
  if (video_debug_stats_enabled) {
    video_frames++;

    if (timer_uptime() != video_frame_timer) {
      video_frame_timer = timer_uptime();

      DEBUG("Video FPS: %u", video_frames);

      video_frames = 0;
    }
  }

  if (video_driver.swap_buffers != NULL) {
    video_driver.swap_buffers();
  } else {
    ERROR("%s", "swap buffers not implemented!");
  }
}

void video_console_attach()
{
  ssfn_dst.ptr =
    (uint8_t*)video_driver.buffer;  /* address of the linear frame buffer */
  ssfn_dst.w = video_driver.width;  /* width */
  ssfn_dst.h = video_driver.height; /* height */
  ssfn_dst.p = video_driver.width * sizeof(uint32_t); /* bytes per line */
}

void video_console_detach()
{
  ssfn_dst.ptr = (uint8_t*)multiboot_framebuffer_common
                   ->framebuffer_addr; /* address of the linear frame buffer */
  ssfn_dst.w = multiboot_framebuffer_common->framebuffer_width;  /* width */
  ssfn_dst.h = multiboot_framebuffer_common->framebuffer_height; /* height */
  ssfn_dst.p =
    multiboot_framebuffer_common->framebuffer_pitch; /* bytes per line */
}

uint32_t video_blend_alpha_callback(uint32_t src, uint32_t dst)
{
  uint32_t alpha = src >> 24;

  if (alpha == 0) {
    return dst;
  }

  uint32_t red_blue =
    (((src & 0x00FF00FF) * alpha) + ((dst & 0x00FF00FF) * (0xFF - alpha))) &
    0xFF00FF00;
  uint32_t green =
    (((src & 0x0000FF00) * alpha) + ((dst & 0x0000FF00) * (0xFF - alpha))) &
    0x00FF0000;

  return (src & 0xFF000000) | ((red_blue | green) >> 8);
}
