#ifndef _LCOM_SERIAL_C_
#define _LCOM_SERIAL_C_

#include <lcom/lcf.h>
#include "uart.h"
#include "../utils/utils.h"

int (ser_print_conf)(unsigned short base_addr);
int (ser_set)(unsigned short base_addr,unsigned long bits,unsigned long stop,long parity,unsigned long rate);


#endif
