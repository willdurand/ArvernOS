#ifndef _VIDEO_API_H_
#define _VIDEO_API_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAKE_RGBA(r, g, b, a) ((a << 24) | (r << 16) | (g << 8) | (b))
#define MAKE_RGB(r, g, b)     MAKE_RGBA(r, g, b, 0xFF)

typedef enum video_blend_type
{
  video_blend_none,
  video_blend_alpha
} video_blend_type_t;

typedef struct video_driver
{
  uint32_t* buffer;
  void* native_buffer;
  int32_t width;
  int32_t height;
  void* pimpl;
  void (*swap_buffers)();
} video_driver_t;

typedef struct frame_buffer_area
{
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  int32_t target_x;
  int32_t target_y;
  int32_t target_width;
  int32_t target_height;
  uint32_t* buffer;
} frame_buffer_area_t;

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
 * Enables some debug statistics such as FPS
 * @param enable whether to enable them
 */
void video_enable_debug_stats();

/**
 * Disables some debug statistics such as FPS
 * @param enable whether to enable them
 */
void video_disable_debug_stats();

/**
 * Calculates an area in the screen to plot a specific position and size
 * @param x the x position in the screen
 * @param y the y position in the screen
 * @param width the width to plot into the screen
 * @param height the height to plot into the screen
 */
frame_buffer_area_t video_area_at(int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height);

/**
 * Blits a buffer into the video buffer
 *
 * @param src_buffer the source color buffer, containing the graphics we want to
 * blit
 * @param x the x coordinate where we want to blit the source buffer
 * @param y the y coordinate where we want to blit the source buffer
 * @param src_width the local width inside the source buffer we want to blit
 * with
 * @param src_height the local height inside the source buffer we want to blit
 * with
 * @param src_buffer_width the total width of the source buffer
 * @param src_buffer_height the total height of the source buffer
 */
void video_blit(uint32_t* src_buffer,
                int32_t x,
                int32_t y,
                int32_t src_width,
                int32_t src_height,
                int32_t src_buffer_width,
                int32_t src_buffer_height);

/**
 * Blits a buffer into the video buffer
 *
 * @param src_buffer the source color buffer, containing the graphics we want to
 * blit
 * @param x the x coordinate where we want to blit the source buffer
 * @param y the y coordinate where we want to blit the source buffer
 * @param src_width the local width inside the source buffer we want to blit
 * with
 * @param src_height the local height inside the source buffer we want to blit
 * with
 * @param src_buffer_width the total width of the source buffer
 * @param src_buffer_height the total height of the source buffer
 */
void video_blit_blend(uint32_t* src_buffer,
                      int32_t x,
                      int32_t y,
                      int32_t src_width,
                      int32_t src_height,
                      int32_t src_buffer_width,
                      int32_t src_buffer_height,
                      video_blend_type_t blend_type);

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
