#include "uart.h"
#include "lcom/lcf.h"
#include "test7.h"

int main(int argc,char *argv[]) {
    if(ser_test_conf(SER_BASE_ADDR_1) != 0) return 1;
    return 0;
}

int ser_test_conf(unsigned short base_addr) {
/*     uint32_t long_value;
    uint8_t st;
    if(sys_inb(base_addr + SER_ADDR_LCR,&long_value) != 0) {
        printf("failed to read from base addr:%u",base_addr);
        return 1;
    };
    st = long_value & 0xFF;

    uint8_t word_length = 5 + (st & (BIT(1) | BIT(0)));
    if(word_length > 8 || word_length < 5){
        printf("invalid word size \n");
        return 1;
    }
    uint8_t no_stop_bits = 1 + (st & SER_LCR_TWO_STOP_BITS);
    if(no_stop_bits != 2 || no_stop_bits != 1) {
        printf("invalid no_stop_bits \n");
        return 1;
    }
    uint8_t parity = st & (BIT(3) | BIT(4) | BIT(5)) >> 3;
    if(parity > 3 || parity < 0) {
        printf("invalid parity number \n");
        return 1;
    }
    uint8_t dlab = st & SER_LCR_SEL_DL;

    printf("WORD SIZE: %d\nNUMBER OF STOP BITS: %d\nPARITY NUMBER: %d\nDLAB: %d\n",word_length,no_stop_bits,parity,dlab); */
    printf("Hello world\n");
    return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate) { 
    /* To be completed */
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

