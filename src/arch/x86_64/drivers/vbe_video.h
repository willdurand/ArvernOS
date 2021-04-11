#ifndef DRIVERS_VBE_VIDEO_H
#define DRIVERS_VBE_VIDEO_H

#include <core/multiboot.h>

void vbe_video_init(multiboot_tag_framebuffer_common_t* common);

#endif
