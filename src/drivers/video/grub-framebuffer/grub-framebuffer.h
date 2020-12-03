#define MAKE_RGBA(r, g, b, a) ((a << 24) | (r << 16) | (g << 8) | (b))
#define MAKE_RGB(r, g, b)     MAKE_RGBA(r, g, b, 0xFF)

extern uint32_t grub_framebuffer_width;
extern uint32_t grub_framebuffer_height;
extern uint8_t grub_framebuffer_bytes_per_pixel;

bool grub_init_framebuffer(multiboot_info_t* mbi);

bool grub_framebuffer_available();

const char* grub_framebuffer_type_string();

void grub_framebuffer_clear(uint32_t color);

void grub_framebuffer_set_console_mode();

void grub_framebuffer_set_canvas_mode();

uint32_t* grub_framebuffer_buffer();

void grub_framebuffer_swap_buffers();
