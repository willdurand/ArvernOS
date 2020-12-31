#ifndef _VBE_VIDEO_H_
#define _VBE_VIDEO_H_
#include <core/multiboot.h>
#include <drivers/video/video_api.h>

void vbe_video_init(multiboot_tag_framebuffer_common_t* common);

#endif
