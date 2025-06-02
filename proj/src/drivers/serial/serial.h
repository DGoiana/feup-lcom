#ifndef _LCOM_SERIAL_H_
#define _LCOM_SERIAL_H_

#include <lcom/lcf.h>
#include "uart.h"
#include "queue.h"
#include "../utils/utils.h"

/**
 * @brief This file stores all the serial port related functions.\n
*/

/**
 * @brief Print current Serial Port configuration.\n
*/
int (ser_print_conf)(unsigned short base_addr);
/**
 * @brief Set Serial Port Configuration.\n
*/
int (ser_set)(unsigned short base_addr,unsigned long bits,unsigned long stop,long parity,unsigned long rate);
/**
 * @brief Send data via polling.\n
*/
int (ser_send_poll)(unsigned short base_addr,char c);
/**
 * @brief Receive data via polling.\n
*/
int (ser_receive_poll)(unsigned short base_addr,char *c);
/**
 * @brief Subscribe to serial port interrupts.\n
*/
void (ser_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribe to serial port interrupts.\n
*/
void (ser_unsubscribe_int)();
/**
 * @brief Enable serial port receive interrupts.\n
*/
int (ser_enable_rx_int)(unsigned short base_addr);
/**
 * @brief Enable serial port transmitition interrupts.\n
*/
int (ser_enable_tx_int)(unsigned short base_addr);
/**
 * @brief Serial port interrupt handler.\n
*/
int (ser_ih)(unsigned short base_addr,uint8_t iir1);
/**
 * @brief Enable FIFO.\n
*/
int (ser_enable_fifo)(unsigned short base_addr,uint8_t *queue_size);
/**
 * @brief Serial Port FIFO Interrupt handler.\n
*/
int (ser_fifo_ih)(unsigned short base_addr,queue_t *q,uint8_t iir);

#endif
