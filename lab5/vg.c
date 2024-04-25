#include <vg.h>

uint32_t (calculate_color)(uint32_t color, uint32_t first, uint8_t step,uint8_t col,uint8_t row) {
  uint8_t R = ((1 << mode_info.RedMaskSize) - 1) & (first >> mode_info.RedFieldPosition);
  uint8_t G = ((1 << mode_info.GreenMaskSize) - 1) & (first >> mode_info.GreenFieldPosition);
  uint8_t B = ((1 << mode_info.BlueMaskSize) - 1) & (first >> mode_info.BlueFieldPosition);
  uint8_t red = (R + col * step) % (1 << mode_info.RedMaskSize);
  uint8_t green = (G + row * step) % (1 << mode_info.GreenMaskSize);
  uint8_t blue = (B + (col + row) * step) % (1 << mode_info.BlueMaskSize);

  return (red << mode_info.RedFieldPosition) | (green << mode_info.GreenFieldPosition) | (blue << mode_info.BlueFieldPosition);
}

void alloc_mem_frame_buffer() {
  frame_buffer = malloc(vram_size);
  memset(frame_buffer,0,vram_size);
  return; 
}

int (vg_draw_pixel)(uint32_t color,uint16_t x,uint16_t y) {
  uint8_t *pixel_pos = ((uint8_t *)frame_buffer + ((y*hres + x) * (bytes_per_pixel)));
  //uint32_t new_color = color & (BIT(bytes_per_pixel+1)-1);
  memcpy(pixel_pos,&color,bytes_per_pixel);
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for(size_t i = 0; i < height && y+i < vres; i++) {
    if(vg_draw_hline(x,y+i,width,color) != 0) return 1;
  }
  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for(size_t i = 0; i < len && x + i < hres; i++) {
    if(vg_draw_pixel(color,x+i,y) != 0) return 1;
  }
  return 0;
}

int (vg_video_mode)(uint16_t mode) {
  reg86_t r;
  memset(&r,0,sizeof(r));

  r.ax = VBE_CALL | SET_VBE_MODE;
  r.bx = LINEAR_FRAMEBUFFER | mode;
  r.intno = CALL_TO_BIOS;

  if(sys_int86(&r) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  return 0;
}
  
int (vg_setup)(uint16_t mode) {
  int r;

  memset(&mode_info,0,sizeof(mode_info));
  if(vbe_get_mode_info(mode,&mode_info) != 0) return 1;
  
  vres = mode_info.YResolution;
  hres = mode_info.XResolution;
  bytes_per_pixel = ( mode_info.BitsPerPixel + 7 ) / 8;

  vram_size = hres * vres * (bytes_per_pixel); 

  struct minix_mem_range address;

  address.mr_base = (phys_bytes) mode_info.PhysBasePtr;
  address.mr_limit = address.mr_base + vram_size;

  if((r = sys_privctl(SELF,SYS_PRIV_ADD_MEM,&address)) != 0){
    printf("error in sys_privctl : %d\n",r);
    return 1;
  }


  video_mem = vm_map_phys(SELF,(void *)(address.mr_base),vram_size);
  if(video_mem == MAP_FAILED)
    panic("couldn’t map video memory"); 


  alloc_mem_frame_buffer();
  
  return 0;
}
