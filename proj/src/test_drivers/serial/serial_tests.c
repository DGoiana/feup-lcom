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
    if(ser_set(base_addr,bits,stop,parity,rate) != 0) return 1;

    if(tx) {
        for(int i = 0; i < stringc;i++) {
            int size = strlen(strings[i]);
            for(int j = 0; j < size; j++) {
                if(ser_send_poll(base_addr,(char) strings[i][j]) != 0) return 1;
            }
            printf("sent: %s\n",strings[i]);
        }
    } else {
        uint8_t iir1;
        uint8_t ser_irq_set;
        int ipc_status,r;
        message msg;

        ser_subscribe_int(&ser_irq_set);
        ser_enable_rx_int(base_addr);

        while(true) {
            if((r=driver_receive(ANY,&msg,&ipc_status))) {
                printf("driver_receive failed with: %d\n",r);
                continue;
            }
            if(is_ipc_notify(ipc_status)) {
                switch (_ENDPOINT_P(msg.m_source))
                {
                case HARDWARE:
                    if(msg.m_notify.interrupts & BIT(ser_irq_set)) {
                        do {
                            if(util_sys_inb(base_addr + SER_ADDR_IIR,&iir1) != 0) {
                                printf("failed to read iir1\n");
                                return 1;
                            }
                            ser_ih(base_addr,iir1);
                        } while(iir1 & SER_IIR_RX_AVAILABLE);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        ser_unsubscribe_int();
    }
    return 0;
}

int ser_test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, 	unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]){
    uint8_t queue_size;
    queue_t *q;
    
    if(ser_set(base_addr,bits,stop,parity,rate) != 0) return 1;
    if(ser_enable_fifo(base_addr,&queue_size) != 0) return 1;

    if((q = new_queue(queue_size)) == NULL) return -1;

    if(tx) {
        for(int i = 0; i < stringc;i++) {
            int size = strlen(strings[i]);
            for(int j = 0; j < size; j++) {
                if(ser_send_poll(base_addr,(char) strings[i][j]) != 0) return 1;
            }
            printf("sent: %s\n",strings[i]);
        }
    } else {
        int tries = 100;
        uint8_t iir1;
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
                        do {
                            if(util_sys_inb(base_addr + SER_ADDR_IIR,&iir1) != 0) {
                                printf("failed to read iir1\n");
                                return 1;
                            }
                            ser_fifo_ih(base_addr,q,iir1);
                            print_queue(q,queue_size);
                        } while(iir1 & SER_IIR_RX_AVAILABLE);
                        
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

int ser_test_queue() {
    queue_t *q;
    unsigned int size = 2;
    char c;

    int str_len = 12;
    char string[] = {'g','o','i','a','n','a',' ','l','i','n','d','o'};

    if((q = new_queue(size)) == NULL) return -1;

    for(int i = 0; i < str_len; i++) {
        if(enqueue(q,string[i]) != 0) {
            printf("queue full\n");
        }
    }

    for(int i = 0; i < str_len;i++) {
        if(dequeue(q,&c) != 0) {
            printf("queue empty\n");
        } else {
            printf("%c",c);
        }
    }

    delete_queue(q);
    return 0;
}
