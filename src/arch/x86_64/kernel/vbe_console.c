#include "console.h"
#include <logging.h>
#include <mmu/paging.h>

#ifdef ENABLE_FRAMEBUFFER
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#else
#define SSFN_CONSOLEBITMAP_HICOLOR
#endif

#include <ssfn.h>

extern unsigned char _binary_console_sfn_start;
extern unsigned char _binary_console_sfn_size;

static uint32_t colors[] = {
  [VTCOLOR_BLACK] = 0x00000000, [VTCOLOR_RED] = 0xFFFF0000,
  [VTCOLOR_GREEN] = 0xFF008000, [VTCOLOR_YELLOW] = 0xFFA52A2A,
  [VTCOLOR_BLUE] = 0xFF0000FF,  [VTCOLOR_MAGENTA] = 0xFFFF00FF,
  [VTCOLOR_CYAN] = 0xFF00FFFF,  [VTCOLOR_GREY] = 0xFFD3D3D3,
};

static uint32_t brightcolors[] = {
  [VTCOLOR_BLACK] = 0xFFA9A9A9, [VTCOLOR_RED] = 0xFFFFCCCB,
  [VTCOLOR_GREEN] = 0xFF90EE90, [VTCOLOR_YELLOW] = 0xFFFFFF99,
  [VTCOLOR_BLUE] = 0xFFADD8E6,  [VTCOLOR_MAGENTA] = 0xFFE78BE7,
  [VTCOLOR_CYAN] = 0xFFE0FFFF,  [VTCOLOR_GREY] = 0xFFFFFF,
};

multiboot_tag_framebuffer_common_t* multiboot_framebuffer_common = NULL;

void vbe_console_init(multiboot_tag_framebuffer_common_t* common)
{
  DEBUG("VBE framebuffer: addr=0x%x width=%d height=%d pitch=%d bpp=%d",
        common->framebuffer_addr,
        common->framebuffer_width,
        common->framebuffer_height,
        common->framebuffer_pitch,
        common->framebuffer_bpp);

  multiboot_framebuffer_common = common;

  uint64_t pages_for_fb =
    (common->framebuffer_pitch * common->framebuffer_height) / PAGE_SIZE;

  for (uint64_t i = 0; i < pages_for_fb; i++) {
    identity_map((uint64_t)common->framebuffer_addr + (i * PAGE_SIZE),
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
  }

  ssfn_src =
    (ssfn_font_t*)&_binary_console_sfn_start; /* the bitmap font to use */

  ssfn_dst.ptr =
    (uint8_t*)common->framebuffer_addr; /* address of the linear frame buffer */
  ssfn_dst.w = common->framebuffer_width;  /* width */
  ssfn_dst.h = common->framebuffer_height; /* height */
  ssfn_dst.p = common->framebuffer_pitch;  /* bytes per line */
  ssfn_dst.x = ssfn_dst.y = 0;             /* pen position */
  ssfn_dst.fg = 0xFFFFFFFF;                /* foreground color */
  ssfn_dst.bg = 0xFF000000;                /* background color */
}

void vbe_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y)
{
  ssfn_dst.x = x * CONSOLE_FONT_WIDTH;
  ssfn_dst.y = y * CONSOLE_FONT_HEIGHT;

  if (cell->attr.bright) {
    ssfn_dst.fg = brightcolors[cell->attr.fg];
  } else {
    ssfn_dst.fg = colors[cell->attr.fg];
  }

  ssfn_putc(cell->c);
}

void vbe_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur) {}
