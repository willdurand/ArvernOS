#include <core/debug.h>
#include <fs/vfs.h>
#include <stdlib.h>
#include <stdio.h>
#include "psf1.h"

#ifdef __is_libc
#include <sys/syscall.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

PSF1_font_t* psf1_load_font(const char *path)
{
    int fd = open(path, 0);

    if(fd < 3)
    {
        DEBUG("Failed to load PSF1 Font at '%s'", path);

        return NULL;
    }

    PSF1_header_t* font_header = (PSF1_header_t*)malloc(sizeof(PSF1_header_t));

    if(read(fd, font_header, sizeof(PSF1_header_t)) < 0 || font_header->magic[0] != PSF1_MAGIC0 ||
        font_header->magic[1] != PSF1_MAGIC1)
    {
        DEBUG("PSF1 Font at '%s' is not a valid file", path);

        close(fd);

        free(font_header);

        return NULL;
    }

    uint32_t glyph_buffer_size = font_header->charsize * 256;

    if(font_header->mode == 1)
    {
        glyph_buffer_size = font_header->charsize * 512; //512 Glyph mode
    }

    void *glyph_buffer = malloc(glyph_buffer_size);

    if(read(fd, glyph_buffer, glyph_buffer_size) < 0)
    {
        DEBUG("PSF1 Font at '%s' is not a valid file", path);

        close(fd);

        free(font_header);
        free(glyph_buffer);

        return NULL;
    }

    close(fd);

    PSF1_font_t *font = (PSF1_font_t*)malloc(sizeof(PSF1_font_t));

    font->psf1_header = font_header;
    font->glyph_buffer = glyph_buffer;

    DEBUG("Loaded PSF1 Font at '%s'", path);

    return font;
}

void psf1_unload(PSF1_font_t* font)
{
    free(font->glyph_buffer);
    free(font);
}
