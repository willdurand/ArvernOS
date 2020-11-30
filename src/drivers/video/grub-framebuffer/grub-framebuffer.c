#include <core/debug.h>
#include <core/multiboot.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "grub-framebuffer.h"

void *framebuffer_ptr = NULL;
uint8_t framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED;
uint32_t framebuffer_width = 0;
uint32_t framebuffer_height = 0;
uint8_t framebuffer_bpp = 0;
uint32_t framebuffer_pitch = 0;

//Indexed

multiboot_color_t *framebuffer_palette = NULL;
int framebuffer_palette_num_colors = 0;

//RGB
int framebuffer_red_mask_size = 0;
int framebuffer_green_mask_size = 0;
int framebuffer_blue_mask_size = 0;

int framebuffer_red_field_position = 0;
int framebuffer_green_field_position = 0;
int framebuffer_blue_field_position = 0;

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

    return true;
}

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
