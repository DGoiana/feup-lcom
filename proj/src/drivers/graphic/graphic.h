#ifndef _LCOM_GRAPHIC_H
#define _LCOM_GRAPHIC_H

#include <lcom/lcf.h>
#include "math.h"

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

vbe_mode_info_t mode_info;
u32_t bytesPerPixel;
u16_t y_res;
u16_t x_res;
void *buffers[VG_NUM_BUFFER];
bool buffer_changed[VG_NUM_BUFFER];
int current_buffer; 
uint32_t vram_size;

/**
 * @brief All the functions related to graphics can be found in this file.Using page flipping between 2 different buffers, all the graphics in the chat are drawn.\n
 * mode_info: Defines the screen attributes\n
 * buffers[]: Array of the buffers that are involved in the page flipping.\n
 * buffer_changed[]: Array that stores if a buffer has any changes(with only 2 buffers is not really used, but it can scale).\n
 * current_buffer: Index of the buffer that is being modified.\n
*/


/**
 * @brief Starts the video mode on the current PC.\n
*/
int startVideoMode(u16_t mode);
/**
 * @brief Flips buffers. (Related to Page Flipping)\n
*/
int flip_buffer();
/**
 * @brief Draw the usernames on the side.\n
*/
int draw_names(char* name, int x, int y, void* buffer);

/**
 * @brief Clear a buffer.\n
*/
void clear_buffer(void* buffer);

/**
 * @brief Initializes the frame buffer.\n
*/
int initFrameBuffer(u16_t mode);

/**
 * @brief Draws Rectangle.\n
*/
int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color, void* buffer);

/**
 * @brief Draws Line.\n
*/
int drawLine(u16_t x, u16_t y, u16_t width, u32_t color, void* buffer);


/**
 * @brief Draws Pixel.\n
*/
int drawPixel(u16_t x, u16_t y, u32_t color, void* buffer);

/**
 * @brief Exits video mode.\n
*/
int exitVideoMode();

/**
 * @brief Formats the background(drawing the background).\n
*/
int formatBackground(void* buffer);

/**
 * @brief Draws a XPM.\n
*/
int draw_xpm(xpm_map_t xpm, u16_t x, u16_t y, void* buffer);

/**
 * @brief Updates the current buffer.\n
*/
int update_buffer(int *text, uint index, bool blink,u16_t x_mouse,u16_t y_mouse, bool right_click);

/**
 * @brief Draws the mouse xpm on a given position.\n
*/
int draw_mouse(int x,int y);

/**
 * @brief Draws the current messages.\n
*/
int drawMessages(void* buffer,bool blink,u16_t x_mouse, u16_t y_mouse,bool right_click);

/**
 * @brief Resets the screen.\n
*/
int reset_screen();

/**
 * @brief Updates the current message buffer.\n
*/
void update_message_buffer(char *message,bool is_yours) ;

/**
 * @brief Frees message buffer.\n
*/
void free_message_buffer();

/**
 * @brief Allocates memory to the message buffer.\n
*/
void alloc_mem_messages_buffer();

/**
 * @brief Transforms character in index of the alphabet array.\n
*/
int check_index(char character);

/**
 * @brief Draws the app title.\n
*/
int draw_title(int x,int y,void *buffer) ;

#endif
