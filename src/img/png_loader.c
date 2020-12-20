#include <drivers/video/video_api.h>
#include <libpng/png.h>
#include <core/core-logging.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __is_libk
#include <sys/syscall.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "png_loader.h"

typedef struct png_read_buffer_state
{
  uint8_t* buffer;
  uint32_t position;
} png_read_buffer_state_t;

void png_read_from_buffer(png_structp ptr,
                          png_bytep destination,
                          png_size_t length)
{
  png_read_buffer_state_t* state =
    (png_read_buffer_state_t*)png_get_io_ptr(ptr);

  memcpy(destination, state->buffer + state->position, length);

  state->position += length;
}

void png_error_function(png_structp png_ptr, png_const_charp message)
{
  DEBUG_OUT("png_load error: %s", message);
}

void png_warning_function(png_structp png_ptr, png_const_charp message)
{
  DEBUG_OUT("png_load warning: %s", message);
}

bool png_load_buffer(const uint8_t* buffer,
                     uint32_t* width,
                     uint32_t* height,
                     uint32_t** pixels)
{
  if (buffer == NULL) {
    DEBUG_OUT("%s", "png_load_buffer: Invalid buffer");

    return false;
  }

  png_structp png_ptr =
    png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

  if (!png_ptr) {
    DEBUG_OUT("%s", "png_load_buffer: Unable to make PNG ptr");

    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if (!info_ptr) {
    DEBUG_OUT("%s", "png_load_buffer: Unable to make PNG ptr");
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

    return false;
  }

  png_read_buffer_state_t state = { .buffer = buffer, .position = 0 };

  png_set_read_fn(png_ptr, (png_voidp)&state, png_read_from_buffer);
  png_set_error_fn(
    png_ptr, (png_voidp)NULL, png_error_function, png_warning_function);

  uint8_t signature[4];

  memcpy(signature, buffer, sizeof(uint8_t[4]));

  if (!png_check_sig(signature, 4)) {
    DEBUG_OUT("%s", "png_load_buffer: Not a valid PNG");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    png_destroy_info_struct(png_ptr, &info_ptr);

    return false;
  }

  png_read_info(png_ptr, info_ptr);

  int bit_depth = 0;
  int color_type = 0;
  int interlace = 0;
  int compression = 0;
  int filter = 0;

  png_get_IHDR(png_ptr,
               info_ptr,
               (png_uint_32*)width,
               (png_uint_32*)height,
               &bit_depth,
               &color_type,
               &interlace,
               &compression,
               &filter);

  if (bit_depth > 8) {
    png_set_strip_16(png_ptr);
  }

  DEBUG_OUT(
    "png_load: color_type: %d, bit_depth: %d, interlace: %d, compression: "
    "%d, filter: %d",
    color_type,
    bit_depth,
    interlace,
    compression,
    filter);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_ptr);

      color_type = PNG_COLOR_TYPE_RGBA;
    } else {
      color_type = PNG_COLOR_TYPE_RGB;
    }
  } else if (color_type == PNG_COLOR_TYPE_GA ||
             color_type == PNG_COLOR_TYPE_GRAY) {
    png_set_gray_to_rgb(png_ptr);

    color_type = color_type == PNG_COLOR_TYPE_GA ? PNG_COLOR_TYPE_RGBA
                                                 : PNG_COLOR_TYPE_RGB;
  }

  *pixels = (uint32_t*)malloc(sizeof(uint32_t) * (*width) * (*height));

  switch (color_type) {
    case PNG_COLOR_TYPE_RGBA:

    {
      uint32_t* data_ptr = *pixels;

      for (uint32_t i = 0; i < *height; i++, data_ptr += *width) {
        png_read_row(png_ptr, (png_bytep)data_ptr, NULL);
      }

      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      png_destroy_info_struct(png_ptr, &info_ptr);

      return true;
    }

    case PNG_COLOR_TYPE_RGB:

    {
      uint8_t row[*width * 3];
      uint32_t* data_ptr = *pixels;

      for (uint32_t i = 0; i < *height; i++, data_ptr += *width) {
        png_read_row(png_ptr, (png_bytep)row, NULL);

        for (uint32_t j = 0, index = 0; j < sizeof(row) / sizeof(row[0]);
             j += 3, index++) {
          data_ptr[index] = MAKE_RGB(row[j], row[j + 1], row[j + 2]);
        }
      }

      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      png_destroy_info_struct(png_ptr, &info_ptr);

      return true;
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_destroy_info_struct(png_ptr, &info_ptr);

  return false;
}

bool png_load_file(const char* path,
                   uint32_t* width,
                   uint32_t* height,
                   uint32_t** pixels)
{
  int fd = open(path, 0);

  if (fd < 3) {
    return false;
  }

  uint8_t* file_buffer = NULL;
  uint32_t size = 0;

  uint8_t buf[1024];
  int bytes_read = 0;
  while ((bytes_read = read(fd, buf, sizeof(buf) / sizeof(buf[0]))) > 0) {

    uint8_t* new_buffer =
      (uint8_t*)malloc(sizeof(uint8_t) * (size + bytes_read));

    if (file_buffer != NULL) {
      memcpy(new_buffer, file_buffer, sizeof(uint8_t) * (size + bytes_read));

      free(file_buffer);
    }

    file_buffer = new_buffer;

    memcpy(file_buffer + size, buf, bytes_read);

    size += bytes_read;
  }

  close(fd);

  bool result = png_load_buffer(file_buffer, width, height, pixels);

  free(file_buffer);

  return result;
}
