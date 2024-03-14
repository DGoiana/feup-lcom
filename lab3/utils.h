#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>


int (util_sys_inb)(int port,uint8_t *value);
int (timer_subscribe_int)(uint8_t *bit_no);
int (timer_unsubscribe_int)();

#endif
