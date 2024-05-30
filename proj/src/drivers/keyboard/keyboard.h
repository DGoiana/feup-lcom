#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include "../mouse/i8042.h"

void (kbc_ih)();
void (keyboard_subscribe_int)(uint8_t *bit_no);
void (keyboard_unsubscribe_int)();
void (timer_ih)();
int (kbc_issue_command)(uint8_t port,uint8_t message);
int (kbc_read_value)(uint8_t port, uint8_t *message);
int (restore_kbc)();

#endif
