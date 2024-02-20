#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
} 
int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t config = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if(sys_outb(TIMER_CTRL,(uint32_t) config) != 0){
   return EXIT_FAILURE;
  }

  uint8_t timer_port;
  switch(timer) {
    case 0: timer_port = TIMER_0; break;
    case 1: timer_port = TIMER_1; break;
    case 2: timer_port = TIMER_2; break;
    default: return EXIT_FAILURE; 
    }
  int ret = util_sys_inb(timer_port,st);
  return ret;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  /* To be implemented by the students */
  union timer_status_field_val timer_val;

  switch (field){
    case tsf_all:
      timer_val.byte = st; 
      break;
    case tsf_initial:
      timer_val.in_mode = (0x30 & st) >> 4;
      break;
    case tsf_mode:
      timer_val.count_mode = (0x0E & st) >> 1;
      break;
    case tsf_base:
      timer_val.byte = BIT(0) & st;
      break;
    default:
      return EXIT_FAILURE;
  }
  int ret = timer_print_config(timer,field,timer_val);

  
  return ret;
}
