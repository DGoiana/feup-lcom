#include "timer.h"

int hook_id = TIMER0_IRQ;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st;
  if(timer_get_conf(timer,&st) != 0){
    return EXIT_FAILURE;
  }
  uint8_t lsb = 0;
  uint8_t msb = 0;
  uint8_t timer_port;
  uint8_t timer_sel;

  switch (timer) {
    case 0: timer_port = TIMER_0; timer_sel = TIMER_SEL0; break;
    case 1: timer_port = TIMER_1; timer_sel = TIMER_SEL1; break;
    case 2: timer_port = TIMER_2; timer_sel = TIMER_SEL2; break;
    default: return EXIT_FAILURE;
  }

  uint8_t conf = ((BIT(0) | BIT(1) | BIT(2) | BIT(3) ) & st) | TIMER_LSB_MSB | timer_sel; 
  
  printf("conf : %x\n",conf);
  if(sys_outb(TIMER_CTRL,(uint32_t) conf) != 0){

  }
  
  uint16_t div = TIMER_FREQ / freq;
  util_get_LSB(div,&lsb);
  util_get_MSB(div,&msb);

  sys_outb(timer_port,(uint32_t) lsb);
  sys_outb(timer_port,(uint32_t) msb);

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id);
  return 0;
}

int (timer_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_id);
  return 0;
}

void (timer_int_handler)() {
  counter++;
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
