#include "kbc_tests.h"
#include "../assets/letters.xpm"

/* FOR TESTING PURPOSES */
int (test_timer)() {
  uint8_t st;
  enum timer_status_field field = tsf_all;

  timer_get_conf(TIMER_SEL0,&st);
    timer_display_conf(TIMER_SEL0,st,field);
  return 0;
}

int (wait_for_esc_key)() {
  extern uint8_t data;

  uint8_t irq_set;
  keyboard_subscribe_int(&irq_set);

  int ipc_status,r;
  message msg;
  uint index = 0;

  int text[100] = {-1};

  while(data != ESC_KEY)  {
    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(irq_set)) {
            kbc_ih();
            int i = retrieve_letter(data);
            if(i != -1 || data == 0xe){
              if(data == 0xe) {
                index--;
              } else {
                text[index] = i;
                index++;
              }
              update_buffer(text, index);
            }
          }
          break;
        default:
          break;
      }
    }
  }
  keyboard_unsubscribe_int();
  return 0;
}

int (test_keyboard)() {
  extern uint8_t data;
  int size = 0;
  uint8_t bytes[2];

  uint8_t irq_set;
  keyboard_subscribe_int(&irq_set);

  int ipc_status,r;
  message msg;


  while(data != ESC_KEY)  {
    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(irq_set)) {
            kbc_ih();
            if(data == SCAN_CODE_HEADER){
              bytes[0] = data;
              size += 1;
              break;
            }
            else {
              bytes[size] = data;
            }
            //kbd_print_scancode(!(bytes[size] & MAKE_BIT),size+1,bytes);
            if(!(bytes[size] & MAKE_BIT)) printf("%c", retrieve_letter(bytes[size]));
            size = 0;
          }
          break;
        default:
          break;
      }
    }
  }
  keyboard_unsubscribe_int();
  return 0;
}

extern uint8_t data;
extern uint8_t packet[3];
extern uint8_t size;
extern uint32_t num_packet;
uint32_t cnt  = 500;
extern int current_index;
extern struct packet pp;
extern bool read_error;

int (test_mouse)() {
  uint8_t mouse_irq_set;

  mouse_subscribe_int(&mouse_irq_set);
  mouse_enable_data_reporting();


  int ipc_status,r;
  message msg;
  int x;
  int y;

  if(initFrameBuffer(0x115) != 0) return 1;
  if(startVideoMode(0x115) != 0) return 1;

  while( num_packet < cnt && data != ESC_KEY)  {
    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(mouse_irq_set)) {
            data = 0x00;
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
              y = MIN(MAX(abs(pp.delta_y - y),0),y_res - 110);
              //printf("%d %d\n",x,y);
              reset_screen();
              draw_mouse(x,y);
              current_index = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  free(frame_buffer);
  if(exitVideoMode() != 0) return 1;


  mouse_write_register(MOUSE_DISABLE_DATA_REPORTING);
  if(restore_kbc()!= 0) return 1;
  mouse_unsubscribe_int();

  return 0;
}

int (test_graphic)(){
  if(initFrameBuffer(0x115) != 0) return 1;
  if(startVideoMode(0x115) != 0) return 1;

  //paint backroung to white and draw rectangles
  if(formatBackground(video_mem) != 0) return 1;

  wait_for_esc_key();

  if(exitVideoMode() != 0) return 1;
  return 0;
}
