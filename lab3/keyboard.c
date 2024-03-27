#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include "utils.h"

#define MAX_SIZE 1024

uint8_t stat;
uint8_t data;
int timer_counter = 0;

void (timer_ih)() {
  timer_counter++;
}


int hook_id = KBC_IRQ;

void (kbc_ih)() {
  // TODO: READ MORE THAN ONE BYTE
  while( 1 ) {
    util_sys_inb(KBC_STATUS_PORT, &stat);
    if( stat & KBC_OBF && (stat & (KBC_PAR_ERR | KBC_TO_ERR) ) == 0) {
      util_sys_inb(KBC_OUT_BUF, &data); /* ass. it returns OK */
    }
    else {
      return;
    }
  }
  tickdelay(micros_to_ticks(DELAY_US));
}


void (keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  sys_irqsetpolicy(KBC_IRQ,IRQ_REENABLE | IRQ_EXCLUSIVE,&hook_id);
}


void (keyboard_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_id);
}
