#ifndef _LCOM_GRAPHIC_H
#define _LCOM_GRAPHIC_H

#include <lcom/lcf.h>

vbe_mode_info_t mode_info;
void* videoMem;

int startVideoMode(u16_t mode);
int initFrameBuffer(u16_t mode);
int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color);
int drawLine(u16_t x, u16_t y, u16_t width, u32_t color);
int drawPixel(u16_t x, u16_t y, u32_t color);
int exitVideoMode();
int paintBackground();

#endif
