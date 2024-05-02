#include "kbc_tests.h"

/* FOR TESTING PURPOSES */
int (test_timer)() {
  uint8_t st;
  enum timer_status_field field = tsf_all;

  timer_get_conf(TIMER_SEL0,&st);
  timer_display_conf(TIMER_SEL0,st,field);
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
            kbd_print_scancode(!(bytes[size] & MAKE_BIT),size+1,bytes);
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
uint32_t cnt  = 100;
extern int current_index;
extern struct packet pp;
extern bool read_error;

int (test_mouse)() {
  uint8_t mouse_irq_set;

  mouse_subscribe_int(&mouse_irq_set);
  mouse_enable_data_reporting();

  int ipc_status,r;
  message msg;

  while( num_packet < cnt )  {
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
              mouse_print_packet(&pp);
              num_packet++;
              current_index = 0;
            } 
          }

          break;
        default:
          break;
      }
    }
  }

  mouse_write_register(MOUSE_DISABLE_DATA_REPORTING);
  if(restore_kbc()!= 0) return 1;
  mouse_unsubscribe_int();

  return 0;
}

int (rtc_test_conf)(void) {
	/* To be completed */
}

int (rtc_test_date)(void) {
	/* To be completed */
}

int (rtc_test_int)(/* to be defined in class */) { 
	/* To be completed */
}
