#ifndef _LCOM_SERIAL_H_
#define _LCOM_SERIAL_H_

#include <lcom/lcf.h>
#include "uart.h"
#include "queue.h"
#include "../utils/utils.h"

int (ser_print_conf)(unsigned short base_addr);
int (ser_set)(unsigned short base_addr,unsigned long bits,unsigned long stop,long parity,unsigned long rate);
int (ser_send_poll)(unsigned short base_addr,char c);
int (ser_receive_poll)(unsigned short base_addr,char *c);
void (ser_subscribe_int)(uint8_t *bit_no);
void (ser_unsubscribe_int)();
int (ser_enable_rx_int)(unsigned short base_addr);
int (ser_enable_tx_int)(unsigned short base_addr);
int (ser_ih)(unsigned short base_addr,uint8_t iir1);
int (ser_enable_fifo)(unsigned short base_addr,uint8_t *queue_size);
int (ser_fifo_ih)(unsigned short base_addr,queue_t *q,uint8_t iir);

#endif
