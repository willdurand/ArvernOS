#ifndef _DRIVERS_VBE_VIDEO_H_
#define _DRIVERS_VBE_VIDEO_H_
#include <core/multiboot.h>
#include <graphics/video_api.h>

void vbe_video_init(multiboot_tag_framebuffer_common_t* common);

#endif
