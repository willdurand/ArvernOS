#ifndef _VIDEO_API_H_
#define _VIDEO_API_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAKE_RGBA(r, g, b, a) ((a << 24) | (r << 16) | (g << 8) | (b))
#define MAKE_RGB(r, g, b)     MAKE_RGBA(r, g, b, 0xFF)

typedef struct video_driver
{
  uint32_t* buffer;
  void* native_buffer;
  uint32_t width;
  uint32_t height;
  void* pimpl;
  void (*swap_buffers)();
} video_driver_t;

extern video_driver_t video_driver;

/**
 * Clears the screen with a specific color
 *
 * @param color the color to clear with. You can use MAKE_RGB/RGBA to make one.
 */
void video_clear(uint32_t color);

/**
 * Clears a region of the screen with a specific color
 *
 * @param color the color to clear with. You can use MAKE_RGB/RGBA to make one.
 * @param x the x coordinate of the rectangle you want to clear
 * @param y the y coordinate of the rectangle you want to clear
 * @param width the width of the rectangle you want to clear
 * @param height the height of the rectangle you want to clear
 */
void video_clear_region(uint32_t color,
                        int32_t x,
                        int32_t y,
                        int32_t width,
                        int32_t height);

/**
 * Gets the width of the video screen
 */
static inline uint32_t video_width()
{
  return video_driver.width;
}

/**
 * Gets the height of the video screen
 */
static inline uint32_t video_height()
{
  return video_driver.height;
}

/**
 * Enables or disables some debug statistics such as FPS
 * @param enable whether to enable them
 */
void video_enable_debug_stats(bool enable);

/**
 * Blits a buffer into the video buffer
 *
 * @param src_buffer the source color buffer, containing the graphics we want to
 * blit
 * @param src_x the local x coordinate inside the source buffer
 * @param src_y the local y coordinate inside the source buffer
 * @param src_width the local width inside the source buffer we want to blit
 * with
 * @param src_height the local height inside the source buffer we want to blit
 * with
 * @param src_buffer_width the total width of the source buffer
 * @param src_buffer_height the total height of the source buffer
 * @param dst_x the x coordinate where we want to blit the source buffer
 * @param dst_y the y coordinate where we want to blit the source buffer
 */
void video_blit(uint32_t* src_buffer,
                int32_t src_x,
                int32_t src_y,
                int32_t src_width,
                int32_t src_height,
                int32_t src_buffer_width,
                int32_t src_buffer_height,
                int32_t dst_x,
                int32_t dst_y);

/**
 * Swaps the buffers for the video screen, presenting your changes
 */
void video_swap_buffers();

/**
 * Attaches the video API to the console API
 * Used to print the console to the double-buffer
 */
void video_console_attach();

/**
 * Detaches the video API from the console API
 */
void video_console_detach();

#endif
