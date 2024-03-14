#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "i8042.h"
#include "keyboard.h"

#include <stdbool.h>
#include <stdint.h>

extern int cnt;
extern int timer_counter;
extern uint8_t data; 
uint8_t make;
int size = 0;
uint8_t bytes[2];


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g1/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g1/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t irq_set;
  cnt = 0;
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
  kbd_print_no_sysinb(cnt);
  return 0;
}

int(kbd_test_poll)() {
  cnt = 0;
  uint8_t byte_command;

  // READ THE COMMAND BYTE
  uint8_t cmd = READ_COMMAND_BYTE;
  kbc_issue_command(cmd);
  util_sys_inb(KBC_OUT_BUF,&byte_command);

  while(data != ESC_KEY) {
    kbc_ih();
    if(data == SCAN_CODE_HEADER){
      bytes[0] = data;
      size += 1;
      break;
    }
    else {
      bytes[size] = data;
    }
    // THIS IS UGLY
    if(bytes[size] != 0x00) {
      kbd_print_scancode(!(bytes[size] & MAKE_BIT),size+1,bytes);
    }
    size = 0;
  }
  kbd_print_no_sysinb(cnt);

  cmd |= WRITE_COMMAND_BYTE;
  byte_command |= ENABLE_OBF_INT_KEYBOARD;
  kbc_issue_command(cmd);

  uint8_t stat;

  while(1) {
    util_sys_inb(KBC_STATUS_PORT,&stat);
    if( (stat & KBC_IBF) == 0 ) {
      sys_outb(KBC_IN_BUF,( uint32_t ) byte_command);
      break;
    }
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t timer_irq_set;
  uint8_t kbc_irq_set;

  keyboard_subscribe_int(&kbc_irq_set);
  timer_subscribe_int(&timer_irq_set);

  int ipc_status,r;
  message msg;

  while(data != ESC_KEY && timer_counter < n * 60) {
    if((r=driver_receive(ANY,&msg,&ipc_status))) {
      printf("driver_receive failed with: %d",r);
      continue;
    }
    if(is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        
        if(msg.m_notify.interrupts & BIT(kbc_irq_set)) {
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
          timer_counter = 0;
        }

        if(msg.m_notify.interrupts & BIT(timer_irq_set)) {
          timer_ih();
        }
        break;
      default:
        break;
      }
    }
  }
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  kbd_print_no_sysinb(cnt);
  return 0;
}
