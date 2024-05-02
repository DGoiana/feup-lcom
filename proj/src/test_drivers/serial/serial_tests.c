#include "serial_tests.h"

int ser_test_conf(unsigned short base_addr) {
    ser_print_conf(base_addr);
    return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate) { 
    if(ser_set(base_addr,bits,stop,parity,rate) != 0) return 1;
    if(ser_print_conf(base_addr) != 0) return 1;
    return 0;
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
                    unsigned long stop, long parity, unsigned long rate, 
                    int stringc, char *strings[]) {
    /* To be completed */
    return 0;
}

int ser_test_int(/* details to be provided */) { 
    /* To be completed */
    return 0;
}

int ser_test_fifo(/* details to be provided */) {
    /* To be completed */
    return 0;
}

