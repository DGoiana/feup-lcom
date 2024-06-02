#include "chat.h"


int (chat_main)() {
  if(initFrameBuffer(0x115) != 0) return 1;
  if(startVideoMode(0x115) != 0) return 1;

  //paint backroung to white and draw rectangles
  if(formatBackground(buffers[current_buffer]) != 0) return 1;

  chat_display();

  if(exitVideoMode() != 0) return 1;
  return 0;
}

int (chat_display)() {

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
  int x_mouse;
  int y_mouse;

  uint8_t keyboard_irq_set;
  keyboard_subscribe_int(&keyboard_irq_set);

  uint8_t timer_irq_set;
  timer_subscribe_int(&timer_irq_set);
  timer_set_frequency(TIMER_SEL0, 30);

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


  int text[MAX_NUM_MESSAGE] = {-1};
  int index = 0;
  bool done = false;
  bool sent_username = false;

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
            if(!in_menu) {
              if(i != -1 || data == 0xe){
                printf("%d\n", i);
                if(data == 0xe) {
                  index = MAX(index--,0);
                } else {
                  text[index] = i;
                  index++;
                }
              } else if (data == 0x1c) {
                char message[MAX_NUM_MESSAGE] = {'\0'};
                for(int i = 0; i < index; i++){
                  message[i] = (char) alphabet[text[i]];
                }
                message[index] = '.';
                message[index + 1] = '\0';
                index++;
                for(int j = 0; j < index; j++) {
                    if(ser_send_poll(COM1_BASE, (char) message[j]) != 0) return 1;
                }
                update_message_buffer(message,true);
                index = 0;
                counter = 0;
                sent_username = true;
              }

            }
            }

            if(msg.m_notify.interrupts & BIT(ser_irq_set) && !in_menu) {
              do {
                  if(util_sys_inb(base_addr + SER_ADDR_IIR,&iir1) != 0) {
                      printf("failed to read iir1\n");
                      return 1;
                  }
                  ser_fifo_ih(base_addr,q,iir1);
                  queue_dequeue_array(q, queue_size, text, &index);
                  char message[MAX_NUM_MESSAGE] = {'\0'};
                  for(int i = 0; i < index; i++) {
                    message[i] = (char) alphabet[text[i]];
                  }
                  message[index] = '\0';
                  index = 0;
                  printf("received message: %s\n",message);
                  update_message_buffer(message,false);
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
              x_mouse = MIN(MAX(pp.delta_x + x_mouse,0),x_res - 10);
              y_mouse = MIN(MAX(abs(pp.delta_y - y_mouse),0),y_res - 10);
              struct collision_box exit_box = {x_res * 0.95, y_res * 0.0175, y_res * 0.04, y_res * 0.04};
              struct collision_box blue_pill = {X_AXIS(0.1),Y_AXIS(0.3),X_AXIS(0.2),Y_AXIS(0.1)};
              struct collision_box red_pill = {X_AXIS(0.4),Y_AXIS(0.3),X_AXIS(0.2),Y_AXIS(0.1)};
              if(in_menu && pp.lb) {
                if(mouse_collision(x_mouse,y_mouse,blue_pill)) {
                  in_menu = false;
                }
                if(mouse_collision(x_mouse,y_mouse,red_pill)) {
                  done = true;
                }
              }
              if(pp.lb && mouse_collision(x_mouse,y_mouse,exit_box)) {
                done = true;
              }
              current_index = 0;
            }
          }

          if(msg.m_notify.interrupts & BIT(timer_irq_set)){
            timer_int_handler();
            update_buffer(text, index, counter > 10 && counter < 30 ? true : false, x_mouse, y_mouse,pp.rb);
            if(counter > 30) counter = 0;
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
