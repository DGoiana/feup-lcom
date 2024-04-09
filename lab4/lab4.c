// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "i8042.h"
#include "mouse.h"
#include "../lab2/timer.c"
#include "../lab2/i8254.h"
#include "../lab3/keyboard.c"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/g1/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g1/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


uint8_t data;
uint8_t packet[3];
uint8_t size;
uint32_t num_packet;
int mouse_hook_id = MOUSE_IRQ;
int current_index = 0;
struct packet pp;
bool read_error;
extern int counter;
extern enum STATE current_state;

int (util_sys_inb)(int port,uint8_t *value) {
  uint32_t long_value;
  int ret = sys_inb(port,&long_value);
  
  *value = 0xFF & long_value;
  return ret;
  
}

int (mouse_test_packet)(uint32_t cnt) {
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
          break;
        default:
          break;
      }
    }
  }

  mouse_write_register(MOUSE_DISABLE_DATA_REPORTING);
  mouse_unsubscribe_int();

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_irq_set;
  uint8_t timer_irq_set;

  timer_subscribe_int(&timer_irq_set);

  mouse_subscribe_int(&mouse_irq_set);
  mouse_enable_data_reporting();

  int ipc_status,r;
  message msg;

  while( counter < idle_time * 60 )  {
    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & BIT(mouse_irq_set)){
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
            counter = 0;
          }
          if(msg.m_notify.interrupts & BIT(timer_irq_set)) {
            timer_int_handler();
            if(counter % sys_hz() == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;
      }
    }
  }

  mouse_write_register(MOUSE_DISABLE_DATA_REPORTING);
  mouse_unsubscribe_int();

  return 0;
}


/**
 * TODO: WHAT IS X_LEN AND WHY IS IT NEEDED?
*/
int (mouse_test_gesture)(uint8_t x_len,uint8_t tolerance) {
  uint32_t cnt = 100;
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
          data = 0x00;
          mouse_ih();
          if(read_error){
              read_error = 0;
              break;
            }
          mouse_synch_packet();
          if(current_index == 3) {
            mouse_build_packet();
            printf("current state: %d\n",(int)current_state);
            //mouse_print_packet(&pp);
            num_packet++;
            current_index = 0;
            next_state(pp,tolerance);
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

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
