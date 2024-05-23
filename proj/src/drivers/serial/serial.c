#include "serial.h"

int serial_hook_id = IRQ_COM1;

int (ser_print_conf)(unsigned short base_addr) {
    uint8_t lcr_stat;
    uint8_t ier_stat;

    uint8_t dl_lsb;
    uint8_t dl_msb;
    uint8_t dl_value;

    if(util_sys_inb(base_addr + SER_ADDR_LCR, &lcr_stat) != 0) {
        printf("failed to read LCR from base addr:%u",base_addr);
        return 1;
    }

    uint8_t word_length = 5 + (lcr_stat & (BIT(1) | BIT(0)));
    if(word_length > 8 || word_length < 5){
        printf("invalid word size \n");
        return 1;
    }
    uint8_t no_stop_bits = 1 + ((lcr_stat & SER_LCR_TWO_STOP_BITS) >> 2);
    if(no_stop_bits != 2 && no_stop_bits != 1) {
        printf("invalid no_stop_bits \n");
        return 1;
    } 
    uint8_t parity = (lcr_stat & (BIT(3) | BIT(4) | BIT(5))) >> 3;

    if(parity > 3) {
        printf("invalid parity number \n");
        return 1;
    }
    uint8_t dlab = (lcr_stat & SER_LCR_SEL_DL) >> 7;
    if(util_sys_inb(base_addr + SER_ADDR_IER, &ier_stat) != 0) {
        printf("failed to read IER from base addr: %u",base_addr);
        return 1;
    }
    uint8_t rx_received_int = ier_stat & SER_IER_ENABLE_RX_INT;
    uint8_t tx_empty_int = (ier_stat & SER_IER_ENABLE_TX_EMPTY_INT) >> 1;
    uint8_t rx_lsr_int = (ier_stat & SER_IER_ENABLE_RX_LSR_INT) >> 2;

    if(dlab) {
        if(util_sys_inb(base_addr + SER_ADDR_DL_LSB, &dl_lsb) != 0) {
            printf("failed to read DL LSB from base addr:%u",base_addr);
            return 1;
        }
        if(util_sys_inb(base_addr + SER_ADDR_DL_MSB, &dl_msb) != 0) {
            printf("failed to read DL MSB from base addr:%u",base_addr);
            return 1;
        }
        dl_value = (dl_msb << 8) && dl_lsb;
        printf("\nBIT RATE: %d",SER_MAX_BIT_RATE/dl_value);
    }


    printf("\nWHOLE LCR STAT: %x",lcr_stat);
    printf("\nWHOLE IER STAT: %x",ier_stat);
    printf("\nWORD SIZE: %d\nNUMBER OF STOP BITS: %d\nDLAB: %d\n",word_length,no_stop_bits,dlab); 
    switch (parity)
    {
    case 0: printf("PARITY:NONE\n");break;
    case 1: printf("PARITY:ODD\n");break;
    case 3: printf("PARITY:EVEN\n");break;
    default: break;
    }
    printf("RX_RECEIVE: %d\nTX_EMPTY: %d\nLSR_INT: %d\n",rx_received_int,tx_empty_int,rx_lsr_int);
    
    return 0;
}

int (ser_set)(unsigned short base_addr,unsigned long bits,unsigned long stop,long parity,unsigned long rate) {
    uint8_t lcr_cfg = 0x00;
    switch (bits)
    {
    case 5: lcr_cfg |= SER_LCR_WORD_LENGTH_5; break;
    case 6: lcr_cfg |= SER_LCR_WORD_LENGTH_6; break;
    case 7: lcr_cfg |= SER_LCR_WORD_LENGTH_7; break;
    case 8: lcr_cfg |= SER_LCR_WORD_LENGTH_8; break;
    default: printf("invalid words length\n"); return 1;
    }

    if(stop == 2) lcr_cfg |= SER_LCR_TWO_STOP_BITS;
    if(stop != 2 && stop != 1) {printf("invalid stop bits\n"); return 1;}

    switch (parity)
    {
    case -1: lcr_cfg |= SER_LCR_PARITY_NONE; break;
    case 0: lcr_cfg |= SER_LCR_PARITY_EVEN; break;
    case 1: lcr_cfg |= SER_LCR_PARITY_ODD; break;
    default: printf("invalid parity\n"); return 1;
    }

    if(sys_outb((int )(base_addr + SER_ADDR_LCR),lcr_cfg) != 0) {
        printf("failed to send first control word to LCR\n");
        return 1;
    }

    uint8_t dl_lsb;
    uint8_t dl_msb;   
    util_get_LSB(rate,&dl_lsb);
    util_get_MSB(rate,&dl_msb);

    if(sys_outb((base_addr + SER_ADDR_DL_LSB),dl_lsb) != 0) {
        printf("failed to send DL LSB\n"); return 1;
    }
    if(sys_outb((base_addr + SER_ADDR_DL_MSB),dl_msb) != 0) {
        printf("failed to send DL MSB\n"); return 1;
    }

    lcr_cfg = lcr_cfg & (BIT(7) - 1); 
    if(sys_outb((int)(base_addr + SER_ADDR_LCR),lcr_cfg) != 0){
        printf("failed to reset LCR\n"); return 1;
    }
    return 0;
}

int (ser_send_poll)(unsigned short base_addr,char c){
    uint8_t lsr;
    uint8_t num_tries = SER_MAX_TRIES;
    if(util_sys_inb(base_addr + SER_ADDR_LSR,&lsr) != 0) {
        printf("failed to read LSR.\n"); return 1;
    } else { 
        if( lsr & (SER_LSR_OVERRUN_ERR | SER_LSR_PARITY_ERR | SER_LSR_FRAMING_ERR)) {
            printf("failed via error\n"); return 1;
        }
    }
    while(num_tries && !(lsr & SER_LSR_TX_RDY)) {
        util_sys_inb(base_addr + SER_ADDR_LSR,&lsr);
        tickdelay(micros_to_ticks(DELAY_US));
        num_tries--;
    }
    if(num_tries == 0 && !(lsr & SER_LSR_TX_RDY )) {
        printf("transmit not ready\n");
        return 1;
    } else {
        sys_outb(base_addr + SER_ADDR_TX_BUF,c);
    }
    return 0;
}

int (ser_receive_poll)(unsigned short base_addr,char *c) {
    uint8_t lsr;
    uint8_t value;
    uint8_t num_tries = SER_MAX_TRIES;
    if(util_sys_inb(base_addr + SER_ADDR_LSR,&lsr) != 0) {
        printf("failed to read LSR.\n"); return 1;
    } else { 
        if( lsr & (SER_LSR_OVERRUN_ERR | SER_LSR_PARITY_ERR | SER_LSR_FRAMING_ERR)) {
            printf("failed via error\n"); return 1;
        }
    }
    while(num_tries && !(lsr & SER_LSR_RX_RDY)) {
        util_sys_inb(base_addr + SER_ADDR_LSR,&lsr);
        tickdelay(micros_to_ticks(DELAY_US));
        num_tries--;
    }
    do {
        if(num_tries == 0 && !(lsr & SER_LSR_RX_RDY)) {
            return 2;
        } else {
            util_sys_inb(base_addr + SER_ADDR_RX_BUF, &value);
            *c = (char) value;
        }
    } while(value == BIT_RATE);
    return 0;
}

void (ser_subscribe_int)(uint8_t *bit_no) {
    *bit_no = serial_hook_id; 
    sys_irqsetpolicy(IRQ_COM1,IRQ_REENABLE | IRQ_EXCLUSIVE , &serial_hook_id);
}

void (ser_unsubscribe_int)() {
    sys_irqrmpolicy(&serial_hook_id);
}

int (ser_enable_rx_int)(unsigned short base_addr) {
    uint8_t ier;
    if(util_sys_inb(base_addr + SER_ADDR_IER,&ier) != 0) return 1;
    ier |= (SER_IER_ENABLE_RX_INT | SER_IER_ENABLE_RX_LSR_INT );
    if(sys_outb(base_addr + SER_ADDR_IER,ier) != 0) {
        printf("failed to update ier\n");
        return 1;
    }
    return 0;
}

int (ser_enable_tx_int)(unsigned short base_addr) {
    uint8_t ier;
    if(util_sys_inb(base_addr + SER_ADDR_IER,&ier) != 0) return 1;
    ier |= ( SER_IER_ENABLE_RX_INT );
    if(sys_outb(base_addr + SER_ADDR_IER,ier) != 0) {
        printf("failed to update ier\n");
        return 1;
    }
    return 0;
}

int (ser_ih)(unsigned short base_addr,uint8_t iir1) {
    uint8_t data;
    uint8_t lsr;

    if(util_sys_inb(base_addr + SER_ADDR_IIR,&iir1) != 0) {
        printf("failed to read iir1\n");
        return 1;
    }
    if(iir1 & SER_IIR_CHAR_TIMEOUT) {
        printf("failed to read char\n");
        return 0;
    }
    if(iir1 & SER_IIR_LSR) {
        if(util_sys_inb(base_addr + SER_ADDR_LSR, &lsr) != 0) {
            printf("failed to read lsr\n"); return 1;
        }
        if(lsr & (SER_LSR_OVERRUN_ERR | SER_LSR_PARITY_ERR | SER_LSR_FRAMING_ERR)) {
            printf("LSR ERROR \n"); return 0;
        }
    }
    if(iir1 & SER_IIR_RX_AVAILABLE) {
        if(util_sys_inb(base_addr + SER_ADDR_RX_BUF,&data) != 0) {
            printf("failed to read data\n"); return 1;
        }
        printf("%c",data);
    }
    return 0;
}

int (ser_enable_fifo)(unsigned short base_addr,uint8_t *queue_size) {
    uint8_t fcr = SER_FCR_ENABLE_FIFO | SER_FCR_CLEAR_RX_FIFO | SER_FCR_CREAR_TX_FIFO | SER_FCR_TR_LEVEL_8; 
    *queue_size = QUEUE_SIZE;
    if(sys_outb(base_addr + SER_ADDR_FCR,fcr) != 0){
        printf("failed to enable FIFO\n");
        return 1;
    }
    return 0;
}

int (ser_fifo_ih)(unsigned short base_addr,queue_t *q,uint8_t iir) {
    uint8_t lsr;
    uint8_t data;

    if((iir & SER_IIR_NOT_PENDING)) {
        //printf("iir not pending");
        return 1;
    }
/*     if(iir & SER_IIR_CHAR_TIMEOUT) {
        printf("failed to read char\n");
        return 0;
    } */
    if(iir & SER_IIR_RX_AVAILABLE) {
        if(iir & SER_IIR_ENABLED_FIFO) {
            for(int i = 0; i < QUEUE_SIZE; i++) {
                if(util_sys_inb(base_addr + SER_ADDR_RX_BUF,&data) != 0) {
                    printf("failed to read data\n"); return 1;
                }
                enqueue(q,data);
            }
            return 0;
        } else {
            printf("failed to get fifo data\n"); return 1;
        }
    }
    if(iir & SER_IIR_LSR) {
        if(util_sys_inb(base_addr + SER_ADDR_LSR, &lsr) != 0) {
            printf("failed to read lsr\n"); return 1;
        }
        if(lsr & (SER_LSR_OVERRUN_ERR | SER_LSR_PARITY_ERR | SER_LSR_FRAMING_ERR)) {
            printf("LSR ERROR \n"); return 0;
        }
    }
    return 1;
}
