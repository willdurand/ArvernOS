#ifndef _PSF2_H
#define _PSF2_H
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  uint32_t magic;
  uint32_t version;
  uint32_t header_size;
  uint32_t flags;
  uint32_t num_glyph;
  uint32_t bytes_per_glyph;
  uint32_t height;
  uint32_t width;
} __attribute__((packed)) psf2_header_t;

typedef struct
{
  uint32_t width;
  uint32_t height;
} psf2_size_t;

typedef struct
{
  psf2_header_t* header;
  void* glyph_buffer;
} psf2_font_t;

psf2_font_t* psf2_load_font(const char* path);
void psf2_unload(psf2_font_t* font);
#endif
