#ifndef _VIDEO_CONSOLE_H_
#define _VIDEO_CONSOLE_H_

extern uint32_t video_console_colors[8];
extern uint32_t video_console_bright_colors[8];
extern uint32_t video_fps;

void video_console_move_cursor(int x, int y);

#endif