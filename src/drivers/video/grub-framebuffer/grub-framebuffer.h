#ifndef _GRUB_FRAMEBUFFER_H_
#define _GRUB_FRAMEBUFFER_H_
#include <resources/psf2/psf2.h>

extern uint32_t grub_framebuffer_width;
extern uint32_t grub_framebuffer_height;
extern uint8_t grub_framebuffer_bytes_per_pixel;

bool grub_init_framebuffer(multiboot_info_t* mbi);

bool grub_framebuffer_available();

const char* grub_framebuffer_type_string();

void grub_framebuffer_set_console_mode();

void grub_framebuffer_set_canvas_mode();

uint32_t* grub_framebuffer_buffer();

void grub_framebuffer_swap_buffers();

void grub_framebuffer_print(psf2_font_t* font,
                            uint32_t color,
                            char* str,
                            uint32_t start_x,
                            uint32_t start_y);

void grub_framebuffer_measure(psf2_font_t* font,
                              char* str,
                              uint32_t* in_x,
                              uint32_t* in_y,
                              uint32_t* out_width,
                              uint32_t* out_height);
#endif
