#ifndef _PNG_LOADER_H_
#define _PNG_LOADER_H_

bool png_load_file(const char* path,
                   uint32_t* width,
                   uint32_t* height,
                   uint32_t** pixels);

bool png_load_buffer(const uint8_t* buffer,
                     uint32_t* width,
                     uint32_t* height,
                     uint32_t** pixels);
#endif
