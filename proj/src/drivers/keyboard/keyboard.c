#include "keyboard.h"

#define MAX_SIZE 1024

uint8_t stat;
uint8_t data;
int timer_counter = 0;

void (timer_ih)() {
  timer_counter++;
}


int kbc_hook_id = KBC_IRQ;

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
  *bit_no = kbc_hook_id;
  sys_irqsetpolicy(KBC_IRQ,IRQ_REENABLE | IRQ_EXCLUSIVE,&kbc_hook_id);
}


void (keyboard_unsubscribe_int)() {
  sys_irqrmpolicy(&kbc_hook_id);
}


int (kbc_issue_command)(uint8_t port,uint8_t message) {
  int num_tries = MAX_NUM_TRIES;
  while(num_tries) {
    util_sys_inb(KBC_STATUS_PORT,&stat);
    if((stat & KBC_IBF) == 0) {
      sys_outb(port, (uint32_t) message);
      return 0;
    }
    //tickdelay(micros_to_ticks(DELAY_US));
    num_tries--;
  }
  return 1;
}

int (kbc_read_value)(uint8_t port, uint8_t *message) {
  int num_tries = MAX_NUM_TRIES;
  while(num_tries) {
    if(util_sys_inb(KBC_STATUS_PORT,&stat) != 0) return 1;
    if((stat & KBC_OBF) && ((stat &(KBC_PAR_ERR | KBC_TO_ERR)) == 0)) {
      if(util_sys_inb(port,message) != 0) return 1;
      return 0;
    }
    //tickdelay(micros_to_ticks(DELAY_US));
    num_tries--;
  }
  return 1;
}

int (restore_kbc)() {
  uint8_t command_byte;
  if(kbc_issue_command(KBC_COMMAND_PORT,READ_COMMAND_BYTE) != 0) return 1;
  if(kbc_read_value(KBC_OUT_BUF,&command_byte) != 0) return 1;
  
  command_byte = command_byte | ENABLE_OBF_INT_KEYBOARD;

  if(kbc_issue_command(KBC_COMMAND_PORT,WRITE_COMMAND_BYTE) != 0) return 1;
  if(kbc_issue_command(KBC_IN_BUF,command_byte) != 0) return 1;

  return 0;
}
