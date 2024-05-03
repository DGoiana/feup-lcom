#include "graphic.h"
#include "../../assets/letters.xpm"

u32_t bytesPerPixel;
u16_t y_res;
u16_t x_res;

int startVideoMode(u16_t mode){
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ax = 0x4F02;
    reg.bx = mode | BIT(14);
    if(sys_int86(&reg) != OK) return 1;
    return 0;
}

int exitVideoMode(){
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ah = 0x00;
    reg.al = 0x03;
    if(sys_int86(&reg) != OK) return 1;
    return 0;
}

int initFrameBuffer(u16_t mode){
    memset(&mode_info, 0, sizeof(mode_info));
    if(vbe_get_mode_info(mode, &mode_info) != 0) return 1;

    bytesPerPixel = (mode_info.BitsPerPixel + 7) / 8;
    x_res = mode_info.XResolution;
    y_res = mode_info.YResolution;

    u32_t vram_size = x_res * y_res * bytesPerPixel;

    struct minix_mem_range address;
    address.mr_base = mode_info.PhysBasePtr;
    address.mr_limit = address.mr_base + vram_size;

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &address) != 0) return 1;

    videoMem = vm_map_phys(SELF, (void*) address.mr_base, vram_size);

    return 0;
}

int drawPixel(u16_t x, u16_t y, u32_t color){
    u8_t* position = (u8_t*) videoMem + ((y * x_res + x) * bytesPerPixel);
    memcpy(position, &color, bytesPerPixel);
    return 0;
}

int drawLine(u16_t x, u16_t y, u16_t width, u32_t color){
    for(u32_t i = 0; i < width; i++){
        if(drawPixel(x + i, y, color) != 0) return 1;
    }
    return 0;
}

int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color){
    for(u32_t i = 0; i < height; i++){
        if(drawLine(x, y + i, width, color) != 0) return 1;
    }
    return 0;
}

int draw_xpm(xpm_map_t xpm, u16_t x, u16_t y) {
    xpm_image_t img;
    u32_t *map = (u32_t*) xpm_load(xpm, XPM_8_8_8_8, &img);
    for(int i = 0; i < img.height; i++){
        for(int j = 0; j < img.width; j++){
            drawPixel(x + j, y + i, *map);
            map++;
        }
    }
    return 0;
}

int formatBackground(){
    drawRectangle(0, 0, x_res, y_res, 0xFFFFFF);
    drawRectangle(0, 0, x_res, y_res * 0.075, 0x57ACEA);
    drawRectangle(0, y_res * 0.075, x_res * 0.25, y_res * 0.925, 0xd8e0e5);
    drawRectangle(x_res * 0.75, y_res * 0.075, x_res * 0.25, y_res * 0.925, 0xd8e0e5);
    drawRectangle(x_res * 0.95, y_res * 0.0175, y_res * 0.04, y_res * 0.04, 0xf53400);
    return 0;
}
