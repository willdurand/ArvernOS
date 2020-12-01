
typedef struct PSF1_header {
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
} __attribute__((packed)) PSF1_header_t;

typedef struct PSF1_font {
	PSF1_header_t* psf1_header;
	void* glyph_buffer;
} __attribute__((packed)) PSF1_font_t;

PSF1_font_t* psf1_load_font(const char* path);
void psf1_unload(PSF1_font_t* font);
