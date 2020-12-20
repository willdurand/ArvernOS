#include <core/core-logging.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __is_libk
#include <sys/syscall.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "psf2.h"

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

psf2_font_t* psf2_load_font(const char* path)
{
  int fd = open(path, 0);

  if (fd < 3) {
    CORE_DEBUG("Failed to load PSF2 Font at '%s'", path);

    return NULL;
  }

  psf2_header_t* font_header = (psf2_header_t*)malloc(sizeof(psf2_header_t));

  if (read(fd, font_header, sizeof(psf2_header_t)) < 0 ||
      ((uint8_t*)&font_header->magic)[0] != PSF2_MAGIC0 ||
      ((uint8_t*)&font_header->magic)[1] != PSF2_MAGIC1 ||
      ((uint8_t*)&font_header->magic)[2] != PSF2_MAGIC2 ||
      ((uint8_t*)&font_header->magic)[3] != PSF2_MAGIC3) {
    CORE_DEBUG("PSF2 Font at '%s' is not a valid file", path);

    close(fd);

    free(font_header);

    return NULL;
  }

  uint32_t glyph_buffer_size =
    font_header->bytes_per_glyph * font_header->num_glyph;

  void* glyph_buffer = malloc(glyph_buffer_size);

  lseek(fd, font_header->header_size, SEEK_SET);

  if (read(fd, glyph_buffer, glyph_buffer_size) < 0) {
    CORE_DEBUG("PSF2 Font at '%s' is not a valid file", path);

    close(fd);

    free(font_header);
    free(glyph_buffer);

    return NULL;
  }

  close(fd);

  psf2_font_t* font = (psf2_font_t*)malloc(sizeof(psf2_font_t));

  font->header = font_header;
  font->glyph_buffer = glyph_buffer;

  CORE_DEBUG("Loaded PSF1 Font at '%s'", path);

  return font;
}

void psf2_unload(psf2_font_t* font)
{
  free(font->glyph_buffer);
  free(font);
}
