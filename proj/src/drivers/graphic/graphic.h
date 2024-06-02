#ifndef _LCOM_GRAPHIC_H
#define _LCOM_GRAPHIC_H

#define LINES_PER_MESSAGE(i) (strlen(messages[i]) / (x_res * 0.425 / 12)  )
#define Y_AXIS(y) y_res * y
#define X_AXIS(x) x_res * x
#define VG_COLOR_WHITE 0xFFFFFF
#define VG_COLOR_BLUE 0x0156e6
#define VG_COLOR_GRAY 0xd8e0e5
#define VG_COLOR_RED 0xe25835
#define VG_COLOR_LIGHT_BLUE 0x89CFF0
#define VG_COLOR_BLACK 0x191970 


#define VG_NUM_BUFFER 3

#include <lcom/lcf.h>
#include "math.h"

vbe_mode_info_t mode_info;
u32_t bytesPerPixel;
u16_t y_res;
u16_t x_res;
void *buffers[VG_NUM_BUFFER];
bool buffer_changed[VG_NUM_BUFFER];
int current_buffer; 
uint32_t vram_size;

int startVideoMode(u16_t mode);
int flip_buffer();
int draw_names(char* name, int x, int y, void* buffer);
void clear_buffer(void* buffer);
int initFrameBuffer(u16_t mode);
int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color, void* buffer);
int drawLine(u16_t x, u16_t y, u16_t width, u32_t color, void* buffer);
int drawPixel(u16_t x, u16_t y, u32_t color, void* buffer);
int exitVideoMode();
int formatBackground(void* buffer);
int draw_xpm(xpm_map_t xpm, u16_t x, u16_t y, void* buffer);
int update_buffer(int *text, uint index, bool blink,u16_t x_mouse,u16_t y_mouse);
int draw_mouse(int x,int y);
int drawMessages(void* buffer,bool blink,u16_t x_mouse, u16_t y_mouse);
int reset_screen();
void update_message_buffer(char *message,bool is_yours) ;
void free_message_buffer();
void alloc_mem_messages_buffer();
int check_index(char character);
int draw_title(int x,int y,void *buffer) ;

#endif
