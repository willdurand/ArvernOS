#include <core/debug.h>
#include <core/multiboot.h>
#include <drivers/video/video_console.h>
#include <mmu/paging.h>
#include <resources/psf1/psf1.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vtconsole/vtconsole.h>

#include "grub-framebuffer.h"

uint32_t framebuffer_cursor_x = 0;
uint32_t framebuffer_cursor_y = 0;
uint32_t video_frames = 0;
uint64_t video_frame_timer = 0;

//  Kernel Externals

extern PSF1_font_t* kernel_console_font;
extern vtconsole_t vtc;

//  Framebuffer Common

void* framebuffer_ptr = NULL;
uint32_t* framebuffer_buffer = NULL;
void* framebuffer_native_buffer =
  NULL; // Used to prevent extra allocations when swapping
uint8_t framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED;
uint8_t framebuffer_bpp = 0;
uint32_t framebuffer_pitch = 0;

//  Indexed

multiboot_color_t* framebuffer_palette = NULL;
uint16_t framebuffer_palette_num_colors = 0;

//  RGB
uint8_t framebuffer_red_mask_size = 0;
uint8_t framebuffer_green_mask_size = 0;
uint8_t framebuffer_blue_mask_size = 0;

uint8_t framebuffer_red_field_position = 0;
uint8_t framebuffer_green_field_position = 0;
uint8_t framebuffer_blue_field_position = 0;

//  Public

uint32_t grub_framebuffer_width = 0;
uint32_t grub_framebuffer_height = 0;
uint8_t grub_framebuffer_bytes_per_pixel = 0;
bool grub_framebuffer_is_console = false;

bool grub_framebuffer_available()
{
  return framebuffer_ptr != NULL &&
         framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT;
}

const char* grub_framebuffer_type_string()
{
  if (framebuffer_ptr == NULL) {
    return "(None)";
  }

  switch (framebuffer_type) {
    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

      return "EGA Text";

    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

      return "Indexed";

    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

      return "RGB";
  }

  return "(Unknown)";
}

void grub_framebuffer_clear_region(uint32_t color,
                                   uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height)
{
  for (uint32_t y_pos = 0, y_index = y * grub_framebuffer_width; y_pos < height;
       y_pos++, y_index += grub_framebuffer_width) {
    for (uint32_t x_pos = 0, x_index = x + y_index; x_pos < width;
         x_pos++, x_index++) {

      framebuffer_buffer[x_index] = color;
    }
  }
}

void grub_framebuffer_clear(uint32_t color)
{
  grub_framebuffer_clear_region(
    color, 0, 0, grub_framebuffer_width, grub_framebuffer_height);
}

void grub_framebuffer_put_char(PSF1_font_t* font,
                               uint32_t color,
                               char character,
                               uint32_t x,
                               uint32_t y)
{
  if (font == NULL || !grub_framebuffer_available()) {
    return;
  }

  char* font_ptr =
    (char*)font->glyph_buffer + (character * font->psf1_header->charsize);

  for (uint32_t y_pos = 0, y_index = y * grub_framebuffer_width; y_pos < 16;
       y_pos++, y_index += grub_framebuffer_width) {
    if (y + y_pos >= grub_framebuffer_height) {
      continue;
    }

    for (uint32_t x_pos = 0, x_index = y_index + x; x_pos < 8;
         x_pos++, x_index++) {
      uint32_t plot_color = 0x00000000;

      if ((*font_ptr & (0b10000000 >> (x_pos))) > 0) {
        plot_color = color;
      }

      if (x + x_pos >= grub_framebuffer_width) {
        continue;
      }

      framebuffer_buffer[x_index] = plot_color;
    }

    font_ptr++;
  }
}

void grub_framebuffer_console_print(PSF1_font_t* font,
                                    uint32_t color,
                                    char* str,
                                    uint32_t start_x,
                                    uint32_t start_y)
{
  char* chr = str;
  uint32_t x = start_x;
  uint32_t y = start_y;

  while (*chr != 0) {
    grub_framebuffer_put_char(font, color, *chr, x, y);

    x += 8;

    if (x + 8 > grub_framebuffer_width) {
      x = start_x;
      y += 16;
    }

    chr++;
  }
}

void grub_framebuffer_console_measure(PSF1_font_t* font,
                                      char* str,
                                      uint32_t* in_x,
                                      uint32_t* in_y,
                                      uint32_t* out_width,
                                      uint32_t* out_height)
{
  char* chr = str;
  uint32_t start_x = in_x != NULL ? *in_x : 0;
  uint32_t start_y = in_y != NULL ? *in_y : 0;
  uint32_t x = start_x;
  uint32_t y = start_y;
  uint32_t max_x = 0;
  uint32_t max_y = 0;

  while (*chr != 0) {

    x += 8;

    if (x + 8 > grub_framebuffer_width) {
      if (max_x < x) {
        max_x = x;
      }

      x = start_x;
      y += 16;

      if (max_y < y) {
        max_y = y;
      }
    }

    chr++;
  }

  if (max_x < x) {
    max_x = x;
  }

  if (max_y < y) {
    max_y = y;
  }

  *out_width = max_x - start_x;
  *out_height = max_y - start_y + 16;
}

void grub_framebuffer_console_on_paint_callback(vtconsole_t* vtc,
                                                vtcell_t* cell,
                                                int x,
                                                int y)
{
  if (grub_framebuffer_is_console == false) {
    return;
  }

  uint32_t color;

  if (cell->attr.bright) {
    color = video_console_bright_colors[cell->attr.fg];
  } else {
    color = video_console_colors[cell->attr.fg];
  }

  uint32_t x_pos = x * 8;
  uint32_t y_pos = y * 16;

  if (cell->c == ' ' || cell->c == '\b') {
    grub_framebuffer_clear_region(0x00000000, x_pos, y_pos, 8, 16);

    return;
  }

  grub_framebuffer_put_char(kernel_console_font, color, cell->c, x_pos, y_pos);
}

void grub_framebuffer_console_on_move_callback(vtconsole_t* vtc,
                                               vtcursor_t* cur)
{
  framebuffer_cursor_x = cur->x * 8;
  framebuffer_cursor_y = cur->y * 16;
}

bool grub_init_framebuffer(multiboot_info_t* mbi)
{
  multiboot_tag_framebuffer_t* entry =
    (multiboot_tag_framebuffer_t*)find_multiboot_tag(
      mbi, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);

  if (entry == NULL) {
    DEBUG("%s", "Failed to find framebuffer tag in multiboot");

    return false;
  }

  framebuffer_ptr = (void*)entry->common.framebuffer_addr;
  framebuffer_type = entry->common.framebuffer_type;
  grub_framebuffer_width = entry->common.framebuffer_width;
  grub_framebuffer_height = entry->common.framebuffer_height;
  framebuffer_bpp = entry->common.framebuffer_bpp;
  framebuffer_pitch = entry->common.framebuffer_pitch;

  switch (framebuffer_bpp) {
    case 8:

      grub_framebuffer_bytes_per_pixel = 1;

      break;

    case 15:
    case 16:

      grub_framebuffer_bytes_per_pixel = 2;

      break;

    case 24:

      grub_framebuffer_bytes_per_pixel = 3;

      break;

    case 32:

      grub_framebuffer_bytes_per_pixel = 4;

      break;
  }

  uint32_t frames_for_vram =
    framebuffer_pitch * grub_framebuffer_height / PAGE_SIZE;

  for (uint32_t i = 0; i < frames_for_vram; i++) {
    identity_map(framebuffer_ptr + (i * PAGE_SIZE),
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
  }

  uint32_t framebuffer_size =
    sizeof(uint32_t[grub_framebuffer_width * grub_framebuffer_height]);

  framebuffer_buffer = (uint32_t*)malloc(
    sizeof(uint32_t[grub_framebuffer_width * grub_framebuffer_height]));
  framebuffer_native_buffer =
    malloc(sizeof(uint8_t[framebuffer_pitch * grub_framebuffer_height]));

  // memset(framebuffer_buffer, 0, framebuffer_size);

  switch (framebuffer_type) {
    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

      framebuffer_palette = entry->framebuffer_palette;
      framebuffer_palette_num_colors = entry->framebuffer_palette_num_colors;

      break;

    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

      framebuffer_red_mask_size = entry->framebuffer_red_mask_size;
      framebuffer_green_mask_size = entry->framebuffer_green_mask_size;
      framebuffer_blue_mask_size = entry->framebuffer_blue_mask_size;

      framebuffer_red_field_position = entry->framebuffer_red_field_position;
      framebuffer_green_field_position =
        entry->framebuffer_green_field_position;
      framebuffer_blue_field_position = entry->framebuffer_blue_field_position;

      break;

    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

      DEBUG("%s", "EGA Text Framebuffer not supported as a framebuffer");

      return false;
  }

  DEBUG("Got Framebuffer Info: %dx%dx%d (%s)",
        grub_framebuffer_width,
        grub_framebuffer_height,
        framebuffer_bpp,
        grub_framebuffer_type_string());

  return true;
}

void grub_framebuffer_set_console_mode()
{
  DEBUG("%s", "Changing grub framebuffer to console mode");

  grub_framebuffer_is_console = true;

  vtc.on_move = grub_framebuffer_console_on_move_callback;
  vtc.on_paint = grub_framebuffer_console_on_paint_callback;

  framebuffer_cursor_x = 0;
  framebuffer_cursor_y = 0;

  for (uint32_t y = 0, y_index = 0; y < vtc.height; y++, y_index += vtc.width) {
    for (uint32_t x = 0; x < vtc.width; x++) {
      vtc.on_paint(&vtc, &vtc.buffer[x + y_index], x, y);
    }
  }
}

void grub_framebuffer_set_canvas_mode()
{
  DEBUG("%s", "Changing grub framebuffer to canvas mode");

  grub_framebuffer_is_console = false;

  grub_framebuffer_clear(0);
}

uint32_t* grub_framebuffer_buffer()
{
  return framebuffer_buffer;
}

void grub_framebuffer_swap_buffers()
{
  uint32_t framebuffer_pixel_size =
    grub_framebuffer_width * grub_framebuffer_height;
  uint32_t* source_ptr = framebuffer_buffer;

  char buffer[1024];

  video_frames++;

  if (timer_uptime() != video_frame_timer) {
    video_frame_timer = timer_uptime();
    video_fps = video_frames;
    video_frames = 0;
  }

  sprintf(buffer, "%d FPS", video_fps);

  uint32_t string_width = 0;
  uint32_t string_height = 0;

  grub_framebuffer_console_measure(
    kernel_console_font, buffer, NULL, NULL, &string_width, &string_height);

  grub_framebuffer_console_print(kernel_console_font,
                                 0xFFFFFF,
                                 buffer,
                                 grub_framebuffer_width - string_width,
                                 grub_framebuffer_height - string_height);

  switch (framebuffer_type) {
    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

      switch (framebuffer_bpp) {
        case 8:

        {
          uint8_t* ptr = (uint8_t*)framebuffer_native_buffer;

          for (uint32_t i = 0; i < framebuffer_pixel_size; i++) {
            *ptr++ = *source_ptr++;
          }
        }

          memcpy(framebuffer_ptr,
                 framebuffer_native_buffer,
                 framebuffer_pitch * grub_framebuffer_height);

          break;

        case 15:
        case 16:

        {
          uint16_t* ptr = (uint16_t*)framebuffer_native_buffer;

          for (uint32_t i = 0; i < framebuffer_pixel_size; i++) {
            *ptr++ = *source_ptr++;
          }
        }

          memcpy(framebuffer_ptr,
                 framebuffer_native_buffer,
                 framebuffer_pitch * grub_framebuffer_height);

          break;

        case 24:

        {
          uint8_t* ptr = (uint8_t*)framebuffer_native_buffer;

          for (uint32_t i = 0; i < framebuffer_pixel_size; i++, ptr += 3) {
            uint32_t* pixel = (uint32_t*)ptr;

            *pixel = ((*source_ptr++) & 0xffffff) | (*pixel & 0xff000000);
          }
        }

          memcpy(framebuffer_ptr,
                 framebuffer_native_buffer,
                 framebuffer_pitch * grub_framebuffer_height);

          break;

        case 32:

          memcpy(framebuffer_ptr,
                 framebuffer_buffer,
                 sizeof(uint32_t[framebuffer_pixel_size]));

          break;
      }

      break;

    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

      // TODO

      break;

    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

      // Unused

      break;
  }
}
