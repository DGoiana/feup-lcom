#include "kbc_tests.h"
#include "../../assets/letters.xpm"
#include "../../strings/strings.h"
#include "../../drivers/graphic/graphic.h"
#include "../../drivers/serial/uart.h"
#include "../../drivers/serial/serial.h"
#include "../../drivers/mouse/i8042.h"
#include "../../drivers/utils/utils.h"

extern char* alphabet;

/* FOR TESTING PURPOSES */
int (test_timer)() {
  uint8_t st;
  enum timer_status_field field = tsf_all;

  timer_get_conf(TIMER_SEL0,&st);
    timer_display_conf(TIMER_SEL0,st,field);
  return 0;
}

int (wait_for_esc_key)() {

  alloc_mem_messages_buffer();

  uint8_t queue_size = 1;
  queue_t *q;

  int base_addr = COM1_BASE;
  uint8_t rate = BIT_RATE;
  uint8_t parity = PARITY_EVEN;
  uint8_t bits = WORD_LENGTH_8;
  uint8_t stop = NUM_STOP_2;


  if(ser_set(base_addr,bits,stop,parity,rate) != 0) return 1;
  if(ser_enable_fifo(base_addr,&queue_size) != 0) return 1;

  extern uint8_t data;
  extern int counter;

  extern uint8_t data_mouse;
  extern uint8_t packet[3];
  extern uint32_t num_packet;
  extern int current_index;
  extern struct packet pp;
  extern bool read_error;
  int x;
  int y;

  uint8_t keyboard_irq_set;
  keyboard_subscribe_int(&keyboard_irq_set);

  uint8_t timer_irq_set;
  timer_subscribe_int(&timer_irq_set);
  timer_set_frequency(TIMER_SEL0, 60);

  int ipc_status,r;
  message msg;

  uint8_t iir1;
  uint8_t ser_irq_set;

  ser_subscribe_int(&ser_irq_set);
  ser_enable_rx_int(base_addr);

  uint8_t mouse_irq_set;
  mouse_subscribe_int(&mouse_irq_set);
  mouse_write_register(MOUSE_ENABLE_DATA_REPORTING);


  if((q = new_queue(queue_size)) == NULL) return -1;


  int text[100] = {-1};
  int index = 0;
  bool done = false;

  while(data != ESC_KEY && !done)  {

    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }

    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(keyboard_irq_set)) {
            kbc_ih();
            int i = retrieve_letter(data);
            if(i != -1 || data == 0xe){
              if(data == 0xe) {
                index = MAX(index--,0);
              } else {
                text[index] = i;
                index++;
              }
            } else if (data == 0x1c) {
              char message[100] = {'\0'};
              for(int i = 0; i < index; i++){
                message[i] = (char) alphabet[text[i]];
              }
              message[index] = '.';
              message[index + 1] = '\0';
              index++;
              for(int j = 0; j < index; j++) {
                  if(ser_send_poll(COM1_BASE, (char) message[j]) != 0) return 1;
              }
              index = 0;
              counter = 0;
              update_message_buffer(message);
            }
          }

          if(msg.m_notify.interrupts & BIT(ser_irq_set)) {
              do {
                  if(util_sys_inb(base_addr + SER_ADDR_IIR,&iir1) != 0) {
                      printf("failed to read iir1\n");
                      return 1;
                  }
                  ser_fifo_ih(base_addr,q,iir1);
                  queue_dequeue_array(q, queue_size, text,&index);
                  char message[100] = {'\0'};
                  for(int i =0; i < index; i++) {
                    message[i] = (char) alphabet[text[i]];
                  }
                  message[index] = '\0';
                  index = 0;
                  update_message_buffer(message);
               } while(iir1 & SER_IIR_RX_AVAILABLE);
          }

          if(msg.m_notify.interrupts & BIT(mouse_irq_set)) {
            data_mouse = 0x00;
            mouse_ih();
            if(read_error){
                read_error = 0;
                break;
              }
            mouse_synch_packet();
            if(current_index == 3) {
              mouse_build_packet();
              num_packet++;
              x = MIN(MAX(pp.delta_x + x,0),x_res - 10);
              y = MIN(MAX(abs(pp.delta_y - y),0),y_res - 10);
              struct collision_box exit_box = {x_res * 0.95, y_res * 0.0175, y_res * 0.04, y_res * 0.04};
              if(pp.lb && mouse_collision(x,y,exit_box)) {
                done = true;
              }
              current_index = 0;
            }
          }

          if(msg.m_notify.interrupts & BIT(timer_irq_set)){
            timer_int_handler();
            update_buffer(text, index, false);
            draw_mouse(x,y);
          }


          break;
        default:
          break;
      }
    }
  }

  mouse_write_register(MOUSE_DISABLE_DATA_REPORTING);
  ser_unsubscribe_int();
  keyboard_unsubscribe_int();
  free_message_buffer();
  return 0;
}
