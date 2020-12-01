extern uint8_t grub_framebuffer_bytes_per_pixel;

bool grub_init_framebuffer(multiboot_info_t* mbi);

bool grub_framebuffer_available();

const char* grub_framebuffer_type_string();

void grub_framebuffer_clear(uint32_t color);
