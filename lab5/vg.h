#include <lcom/lcf.h>
#include "constants.h"

size_t hres;
size_t vres;
size_t bytes_per_pixel;
void *video_mem;
vbe_mode_info_t mode_info;


uint32_t (calculate_color)(uint32_t color, uint32_t first, uint8_t step,uint8_t col,uint8_t row);
int (vg_draw_pixel)(uint32_t color,uint16_t x,uint16_t y);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_video_mode)(uint16_t mode);
int (vg_setup)(uint16_t mode);
