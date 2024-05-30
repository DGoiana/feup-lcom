#ifndef _LCOM_GRAPHIC_H
#define _LCOM_GRAPHIC_H

#define LINES_PER_MESSAGE(i) (strlen(messages[i]) / (x_res * 0.425 / 12))

#include <lcom/lcf.h>
#include "math.h"

vbe_mode_info_t mode_info;
u32_t bytesPerPixel;
u16_t y_res;
u16_t x_res;
void *frame_buffer;
void* video_mem;
uint32_t vram_size;

int startVideoMode(u16_t mode);
int initFrameBuffer(u16_t mode);
int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color, void* buffer);
int drawLine(u16_t x, u16_t y, u16_t width, u32_t color, void* buffer);
int drawPixel(u16_t x, u16_t y, u32_t color, void* buffer);
int exitVideoMode();
int formatBackground(void* buffer);
int draw_xpm(xpm_map_t xpm, u16_t x, u16_t y, void* buffer);
int update_buffer(int *text, uint index);
int draw_mouse(int x,int y);
int drawMessages(void* buffer);
int reset_screen();

#endif
