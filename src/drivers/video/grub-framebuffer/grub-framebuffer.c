#include <core/debug.h>
#include <core/multiboot.h>
#include <drivers/video/video_console.h>
#include <mmu/paging.h>
#include <vtconsole/vtconsole.h>
#include <resources/psf1/psf1.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "grub-framebuffer.h"

uint32_t framebuffer_cursor_x = 0;
uint32_t framebuffer_cursor_y = 0;

#define PLOT_8_BPP_PIXEL(color, index) \
                                {\
                                uint8_t *pixel = (uint8_t*)framebuffer_ptr + (index);\
                                \
                                *pixel = (color);\
                                }

#define PLOT_16_BPP_PIXEL(color, index) \
                                {\
                                uint16_t *pixel = (uint16_t*)((uint8_t*)framebuffer_ptr + (index) * sizeof(uint16_t));\
                                \
                                *pixel = (color);\
                                }

#define PLOT_24_BPP_PIXEL(color, index) \
                                {\
                                uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer_ptr + (index) * sizeof(uint8_t[3]));\
                                \
                                *pixel = ((color) & 0xffffff) | (*pixel & 0xff000000);\
                                }

#define PLOT_32_BPP_PIXEL(color, index) \
                                {\
                                uint32_t *pixel = (uint32_t*)((uint8_t*)framebuffer_ptr + (index) * sizeof(uint32_t));\
                                \
                                *pixel = (color);\
                                }

//Kernel Externals

extern PSF1_font_t *kernel_console_font;
extern vtconsole_t vtc;

//Framebuffer Common

volatile void *framebuffer_ptr = NULL;
uint8_t framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED;
uint32_t framebuffer_width = 0;
uint32_t framebuffer_height = 0;
uint8_t framebuffer_bpp = 0;
uint32_t framebuffer_pitch = 0;

//Indexed

multiboot_color_t *framebuffer_palette = NULL;
uint16_t framebuffer_palette_num_colors = 0;

//RGB
uint8_t framebuffer_red_mask_size = 0;
uint8_t framebuffer_green_mask_size = 0;
uint8_t framebuffer_blue_mask_size = 0;

uint8_t framebuffer_red_field_position = 0;
uint8_t framebuffer_green_field_position = 0;
uint8_t framebuffer_blue_field_position = 0;

//Public

uint8_t grub_framebuffer_bytes_per_pixel = 0;

bool grub_framebuffer_available()
{
    return framebuffer_ptr != NULL && framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT;
}

const char *grub_framebuffer_type_string()
{
    if(framebuffer_ptr == NULL)
    {
        return "(None)";
    }

    switch(framebuffer_type)
    {
        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

            return "EGA Text";

        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

            return "Indexed";

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

            return "RGB";
    }

    return "(Unknown)";
}

void grub_framebuffer_clear_region(uint32_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    for(uint32_t y_pos = 0, y_index = y * framebuffer_width; y_pos < height; y_pos++, y_index += framebuffer_width)
    {
        for(uint32_t x_pos = 0, x_index = x + y_index; x_pos < width; x_pos++, x_index++)
        {
            switch(framebuffer_type)
            {
                case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

                switch(framebuffer_bpp)
                {
                    case 8:

                        PLOT_8_BPP_PIXEL(color, x_index);

                        break;

                    case 15:
                    case 16:

                        PLOT_16_BPP_PIXEL(color, x_index);

                        break;

                    case 24:

                        PLOT_24_BPP_PIXEL(color, x_index);

                        break;

                    case 32:

                        PLOT_32_BPP_PIXEL(color, x_index);

                        break;

                case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

                    //TODO

                    break;

                case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

                    //Unused

                    break;
                }
            }
        }
    }
}

void grub_framebuffer_clear(uint32_t color)
{
    grub_framebuffer_clear_region(color, 0, 0, framebuffer_width, framebuffer_height);
}

void grub_framebuffer_put_char(PSF1_font_t *font, uint32_t color, char character, uint32_t x, uint32_t y)
{
    if(font == NULL || !grub_framebuffer_available())
    {
        return;
    }

    uint8_t *local_framebuffer = (uint8_t*)framebuffer_ptr;
    char *font_ptr = (char*)font->glyph_buffer + (character * font->psf1_header->charsize);

    for(uint32_t y_pos = y, y_index = y * framebuffer_width; y_pos < y + 16; y_pos++, y_index += framebuffer_width)
    {
        if(y_pos >= framebuffer_height)
        {
            continue;
        }

        for(uint32_t x_pos = x; x_pos < x + 8; x_pos++)
        {
            if((*font_ptr & (0b10000000 >> (x_pos - x))) > 0)
            {
                if(x_pos >= framebuffer_width)
                {
                    continue;
                }

                switch(framebuffer_type)
                {
                    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

                        switch(framebuffer_bpp)
                        {
                            case 8:

                                PLOT_8_BPP_PIXEL(color, y_index + x_pos);

                            break;

                            case 15:
                            case 16:

                                PLOT_16_BPP_PIXEL(color, y_index + x_pos);

                            break;

                            case 24:

                                PLOT_24_BPP_PIXEL(color, y_index + x_pos);

                            break;

                            case 32:

                                PLOT_32_BPP_PIXEL(color, y_index + x_pos);

                            break;
                        }

                        break;

                    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

                        //TODO

                        break;

                    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

                        //Unused

                        break;
                }
            }
        }

        font_ptr++;
    }
}

void grub_framebuffer_console_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y)
{
    uint32_t color;

    if (cell->attr.bright) {
        color = video_console_bright_colors[cell->attr.fg];
    } else {
        color = video_console_colors[cell->attr.fg];
    }

    uint32_t x_pos = x * 8;
    uint32_t y_pos = y * 16;

    if(cell->c == ' ' || cell->c == '\b') {
        grub_framebuffer_clear_region(0x00000000, x_pos, y_pos, 8, 16);

        return;
    }

    grub_framebuffer_put_char(kernel_console_font, color, cell->c, x_pos, y_pos);
}

void grub_framebuffer_console_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur)
{
    framebuffer_cursor_x = cur->x * 8;
    framebuffer_cursor_y = cur->y * 16;
}

bool grub_init_framebuffer(multiboot_info_t* mbi)
{
    multiboot_tag_framebuffer_t *entry = (multiboot_tag_framebuffer_t *)find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);

    if(entry == NULL)
    {
        DEBUG("%s", "Failed to find framebuffer tag in multiboot");

        return false;
    }

    framebuffer_ptr = (void *)entry->common.framebuffer_addr;
    framebuffer_type = entry->common.framebuffer_type;
    framebuffer_width = entry->common.framebuffer_width;
    framebuffer_height = entry->common.framebuffer_height;
    framebuffer_bpp = entry->common.framebuffer_bpp;
    framebuffer_pitch = entry->common.framebuffer_pitch;

    switch(framebuffer_bpp)
    {
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

    switch(framebuffer_type)
    {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:

            framebuffer_palette = entry->framebuffer_palette;
            framebuffer_palette_num_colors = entry->framebuffer_palette_num_colors;

            break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:

            framebuffer_red_mask_size = entry->framebuffer_red_mask_size;
            framebuffer_green_mask_size = entry->framebuffer_green_mask_size;
            framebuffer_blue_mask_size = entry->framebuffer_blue_mask_size;

            framebuffer_red_field_position = entry->framebuffer_red_field_position;
            framebuffer_green_field_position = entry->framebuffer_green_field_position;
            framebuffer_blue_field_position = entry->framebuffer_blue_field_position;

            break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:

            DEBUG("%s", "EGA Text Framebuffer not supported as a framebuffer");

            return false;
    }

    DEBUG("Got Framebuffer Info: %dx%dx%d (%s)", framebuffer_width, framebuffer_height, framebuffer_bpp, grub_framebuffer_type_string());

    DEBUG("%s", "Attaching grub framebuffer to console");

    uint32_t frames_for_vram = framebuffer_pitch * framebuffer_height / PAGE_SIZE;

    for(uint32_t i = 0; i < frames_for_vram; i++)
    {
        identity_map(entry->common.framebuffer_addr + (i * PAGE_SIZE), PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
    }

    vtc.on_move = grub_framebuffer_console_on_move_callback;
    vtc.on_paint = grub_framebuffer_console_on_paint_callback;

    framebuffer_cursor_x = 0;
    framebuffer_cursor_y = 0;

    for(uint32_t y = 0, y_index = 0; y < vtc.height; y++, y_index += vtc.width)
    {
        for(uint32_t x = 0; x < vtc.width; x++)
        {
            vtc.on_paint(&vtc, &vtc.buffer[x + y_index], x, y);
        }
    }

    return true;
}
