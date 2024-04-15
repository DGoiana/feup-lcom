#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>
#include "utils.h"
#include "i8042.h"

int timer_hook_id = TIMER_IRQ;

int cnt = 0;



int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id;
  sys_irqsetpolicy(TIMER_IRQ,IRQ_REENABLE,&timer_hook_id);
  return 0;
}

int (timer_unsubscribe_int)() {
  sys_irqrmpolicy(&timer_hook_id);
  return 0;
}



#endif
