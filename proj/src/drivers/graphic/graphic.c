#include "graphic.h"

u32_t bytesPerPixel;

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
    u32_t vram_size = mode_info.XResolution * mode_info.YResolution * bytesPerPixel;

    struct minix_mem_range address;
    address.mr_base = mode_info.PhysBasePtr;
    address.mr_limit = address.mr_base + vram_size;

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &address) != 0) return 1;

    videoMem = vm_map_phys(SELF, (void*) address.mr_base, vram_size);

    return 0;
}

int drawPixel(u16_t x, u16_t y, u32_t color){
    u8_t* position = (u8_t*) videoMem + ((y * mode_info.XResolution + x) * bytesPerPixel);
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

int paintBackground(){
    drawRectangle(0, 0, mode_info.XResolution, mode_info.YResolution, 0xFFFFFFFF);
    return 0;
}

