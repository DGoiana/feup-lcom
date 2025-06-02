// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "constants.h"
#include "../lab3/keyboard.c"
#include "../lab2/timer.c"
#include "vg.h"

extern size_t hres;
extern size_t vres;
extern size_t bytes_per_pixel;
extern size_t vram_size;
extern vbe_mode_info_t mode_info;


bool indexed = false;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g1/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g1/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int print_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm,type,&img);
  for(int i = 0; i < img.height; i++) {
    for(int j = 0; j < img.width;j++) {
      if(vg_draw_pixel((uint32_t)*sprite,j+x,i+y) != 0) {
        printf("FAILED TO DRAW PIXMAP\n");
        vg_exit();
        return 1;
      }
      sprite++;
    }
  }
  return 0;
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(val == 0){
    return EXIT_FAILURE;
  }
  *lsb = (uint8_t) (val & 0x00FF);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(val == 0){
    return EXIT_FAILURE;
  }
  *msb = (uint8_t) ((val & 0xFF00) >> 8);
  return 0;
}


int(video_test_init)(uint16_t mode, uint8_t delay) {
  reg86_t r;

  memset(&r,0,sizeof(r));

  r.ax = VBE_CALL | SET_VBE_MODE;
  r.bx = LINEAR_FRAMEBUFFER | mode;
  r.intno = CALL_TO_BIOS;

  if(sys_int86(&r) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  sleep(delay);

  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  uint8_t kbc_irq_set;
  keyboard_subscribe_int(&kbc_irq_set);

  if(vg_setup(mode) != 0) {
    vg_exit();
    return 1;
  }

  if(vg_video_mode(mode) != 0) return 1;

  if(vg_draw_rectangle(x,y,width,height,color) != 0) {
    vg_exit();
    return 1;
  }
  memcpy((void *)video_mem,(void *)frame_buffer,vram_size);

  int ipc_status,response;
  message msg;


  while(data != ESC_KEY)  {
    if((response=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",response);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(kbc_irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }
  keyboard_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t kbc_irq_set;
  keyboard_subscribe_int(&kbc_irq_set);

  if(vg_setup(mode) != 0) {
    vg_exit();
    return 1;
  }

  if(vg_video_mode(mode) != 0) return 1;

  if (mode == 0x105) indexed = true;

  uint32_t color;

  uint32_t hsize = mode_info.XResolution / (no_rectangles);
  uint32_t vsize = mode_info.YResolution / (no_rectangles);

  for(size_t col = 0; col < no_rectangles ; col++) {
    for(size_t row = 0; row < no_rectangles;row++) {
      if(indexed) {
        color = (first+(col * no_rectangles + row) * step) % (1 << mode_info.BitsPerPixel);
      } else {
        color = calculate_color(color,first,step,row,col);
      }
      vg_draw_rectangle(row * hsize, col * vsize,hsize,vsize,color);
    }
  }
  memcpy((void *)video_mem,(void *)frame_buffer,vram_size);

  int ipc_status,response;
  message msg;


  while(data != ESC_KEY)  {
    if((response=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",response);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(kbc_irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }
  keyboard_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t kbc_irq_set;
  keyboard_subscribe_int(&kbc_irq_set);

  if(vg_setup(0x105) != 0) {
    vg_exit();
    return 1;
  }

  if(vg_video_mode(0x105) != 0) return 1;

  print_xpm(xpm,x,y);
  memcpy((void *)video_mem,(void *)frame_buffer,vram_size);

  int ipc_status,response;
  message msg;


  while(data != ESC_KEY)  {
    if((response=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",response);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(kbc_irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }
  keyboard_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  uint8_t kbc_irq_set;
  uint8_t timer_irq_set;
  keyboard_subscribe_int(&kbc_irq_set);
  timer_subscribe_int(&timer_irq_set);


  if(timer_set_frequency(TIMER_SEL0,(uint32_t)fr_rate) != 0) return 1;

  if(vg_setup(0x105) != 0) {
    vg_exit();
    return 1;
  }

  if(vg_video_mode(0x105) != 0) return 1;



  int ipc_status,response;
  message msg;

  bool horizontal_movement = (xi < xf && yi == yf) ? true : false;

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  xpm_load(xpm,type,&img);  

  print_xpm(xpm,xi,yi);
  memcpy((void *)video_mem,(void *)frame_buffer,vram_size);

  while(data != ESC_KEY && (yi < yf || xi < xf))  {
    if((response=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",response);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(kbc_irq_set)) {
            kbc_ih();
          }
          if(msg.m_notify.interrupts & BIT(timer_irq_set)) {
            vg_draw_rectangle(xi,yi,img.width,img.height,0xFFFFFF);
            if(horizontal_movement) {
              xi += speed;
              if(xi > xf) xi = xf;
            } else {
              yi += speed;
              if(yi > yf) yi = yf;
            }
            if(print_xpm(xpm,xi,yi) != 0) return 1;
            memcpy((void *)video_mem,(void *)frame_buffer,vram_size);
          }
          break;
        default:
          break;
      }
    }
  }

  free(frame_buffer);
  vg_exit();

  timer_unsubscribe_int();
  keyboard_unsubscribe_int();

  return 0;
}


int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);



  return 1;
}
