#include "serial_tests.h"

unsigned short base_address;

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
    char response = 0x00;
    if(ser_set(base_addr,bits,stop,parity,rate) != 0){
        printf("failed to set serial config\n");
        return 1;
    }
    if(tx) {
        for(int i = 0; i < stringc;i++) {
            int size = strlen(strings[i]);
            for(int j = 0; j < size; j++) {
                if(ser_send_poll(base_addr,(char) strings[i][j]) != 0) return 1;
            }
        }
    } else {
        int max_tries = SER_MAX_TRIES * 10;
        while(max_tries) {
            int r = ser_receive_poll(base_addr,&response); 
            if(r == 2) {
                max_tries--;
                sleep(1);
            } else {
                printf("%c",response);
            }
        }
        printf("\n");
    }

    return 0;
}

int ser_test_int(unsigned short base_addr, unsigned char tx, unsigned long bits, 
                    unsigned long stop, long parity, unsigned long rate, 
                    int stringc, char *strings[]) { 
    if(ser_set(base_addr,bits,stop,parity,rate) != 0) {
        printf("failed to set serial config\n");
        return 1;
    }
    if(tx) {
        for(int i = 0; i < stringc;i++) {
            int size = strlen(strings[i]);
            for(int j = 0; j < size; j++) {
                if(ser_send_poll(base_addr,(char) strings[i][j]) != 0) return 1;
            }
        }
    } else {
        int tries = 100;
        uint8_t ser_irq_set;
        int ipc_status,r;
        message msg;

        ser_subscribe_int(&ser_irq_set);
        ser_enable_rx_int(base_addr);

        while(tries) {
            if((r=driver_receive(ANY,&msg,&ipc_status))) {
                printf("driver_receive failed with: %d\n",r);
                continue;
            }
            if(is_ipc_notify(ipc_status)) {
                switch (_ENDPOINT_P(msg.m_source))
                {
                case HARDWARE:
                    if(msg.m_notify.interrupts & BIT(ser_irq_set)) {
                        ser_ih(base_addr);
                    }
                    break;
                default:
                    break;
                }
            }
            tries--;
        }

        ser_unsubscribe_int();
    }
    return 0;
}

int ser_test_fifo(/* details to be provided */) {
    /* To be completed */
    return 0;
}
