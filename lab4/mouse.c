#include "mouse.h"

uint8_t stat;
extern uint8_t data;
extern uint8_t packet[3];
extern uint8_t size;
extern uint32_t num_packet;
extern int mouse_hook_id;
extern int current_index;
extern struct packet pp;
extern bool read_error;




enum STATE current_state = INITIAL;
int slope = 2;
int current_x = 0;
int current_y = 0;

/**
 * State Machine:
 * 1. Initial State
 * 1-> 2 via click left
 * 2 -> 2 while abs(expected - current) <= tolerance & left is pressed
 * 2 -> 1 if abs(expected - current) > tolerance
 * 2 -> 3 if left releasedd
 * 3 -> 3 while right is not clicked
 * 3 -> 1 if button other than right is clicked or abs(expected - current) > tolerance
 * 3 -> 4 if right is clicked and only right
 * 4 -> 4 while abs(expected - current) <= tolerance & right is pressed
 * 4 -> 1 if right release, if abs(expected - current) <= tolerance or another button is clicked
*/


bool (check_inbound)(int *x,int *y, int x_offset, int y_offset, int tolerance) {
  int new_x; int new_y;

  if(x_offset == 0 || y_offset == 0) return true;

  new_x = *x + x_offset;
  new_y = *y + y_offset;

  int lower_bound = abs(slope*(new_x)) - tolerance;
  int upper_bound = abs(slope*(new_x)) + tolerance;

  if((new_y >= lower_bound) && (new_y <= upper_bound) ) {
    *x += new_x;
    *y += new_y;
    return true;
  } else {
    *x = 0;
    *y = 0;
    return false;
  }
}

int (next_state)(struct packet pp, int tolerance){
  switch (current_state)
  {
  case INITIAL:
    if(pp.lb && !pp.mb && !pp.rb) {
      current_x += pp.delta_x;
      current_y += pp.delta_y;

      current_state = FIRST;
    }
    break;
  case FIRST: // release left button
    if(check_inbound(&current_x,&current_y,pp.delta_x,pp.delta_y,tolerance)) {
      if(pp.mb) {
        current_state = INITIAL;
      } else if(!pp.lb) {
        current_state = SECOND;
      } else {
        current_state = FIRST;  // just for clarity
      }
    } else {
      current_state = INITIAL;
    }
    break;
  case SECOND:
    if(check_inbound(&current_x,&current_y,pp.delta_x,pp.delta_y,tolerance)) {
      if(!pp.rb && !pp.mb && !pp.lb) current_state = SECOND;    // just for clarity
      if(pp.mb || pp.lb) current_state = INITIAL;
      if(pp.rb) current_state = THIRD;
    } else {
      current_state = INITIAL;
    }
    break;
  case THIRD:
    if(check_inbound(&current_x,&current_y,pp.delta_x,pp.delta_y,tolerance)) {
      if(pp.mb || pp.lb) {
        current_state = INITIAL;
      } else if (!pp.rb) {
        current_state = INITIAL;
      }
      break;
    } else {
      current_state = INITIAL;
      break;
    }
    current_state = THIRD;  // just for clarity
  default:
    break;
  }
  return 0;
}

int (kbc_write_register)(uint8_t port,uint8_t message) {
  int num_tries = MAX_NUM_TRIES;
  while(num_tries) {
    util_sys_inb(KBC_STATUS_PORT,&stat);
    if(((stat & KBC_IBF) == 0)) {
      sys_outb(port, (uint32_t) message);
      return 0;
    }
    num_tries--;
    // tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (kbc_read_register)(uint8_t port, uint8_t *message) {
  int num_tries = MAX_NUM_TRIES;
  while(num_tries) {
    if(util_sys_inb(KBC_STATUS_PORT,&stat) != 0) return 1;
    if((stat &(KBC_PAR_ERR | KBC_TO_ERR)) != 0) return 1;
    if((stat & KBC_OBF) && (stat &(KBC_PAR_ERR | KBC_TO_ERR)) == 0 && (stat & KBC_AUX_BYTE)) {
      if(util_sys_inb(port,message) != 0) return 1;
      return 0;
    }
    num_tries--;
    // tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int (mouse_write_register)(uint8_t command) {
  int num_tries = MAX_NUM_TRIES;
  uint8_t ack;
  while(num_tries) {
    if(kbc_write_register(KBC_COMMAND_PORT,KBC_MOUSE_COMMAND) != 0) return 1;
    if(kbc_write_register(KBC_IN_BUF,command) != 0) return 1;
    kbc_read_register(KBC_OUT_BUF,&ack);
    if(ack == MOUSE_ACK) return 0;
    num_tries--;
  }
  return 1;
}

int (mouse_read_command_byte)(uint8_t *value) {
  while(mouse_write_register(MOUSE_READ_COMMAND_BYTE) != 0){
    if(kbc_read_register(KBC_OUT_BUF,value) != 0) return 1;
  }
  return 0;
}

void (mouse_build_packet)() {
  /*
  struct packet {
    uint8_t bytes[3]; // mouse packet raw bytes
    bool rb, mb, lb;  // right, middle and left mouse buttons pressed
    int16_t delta_x;  // mouse x-displacement: rightwards is positive
    int16_t delta_y;  // mouse y-displacement: upwards is positive
    bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
  };
  */
  pp.bytes[0] = packet[0]; 
  pp.bytes[1] = packet[1];
  pp.bytes[2] = packet[2]; 

  pp.rb = (packet[0] & MOUSE_RIGHT_BUTTON);
  pp.mb = (packet[0] & MOUSE_MIDDLE_BUTTON);
  pp.lb = (packet[0] & MOUSE_LEFT_BUTTON);

  pp.delta_x = (packet[0] & MOUSE_MSB_X_DELTA) ? (0xFF00 | packet[1]) : packet[1];
  pp.delta_y = (packet[0] & MOUSE_MSB_Y_DELTA) ? (0xFF00 | packet[2]) : packet[2];

  pp.x_ov = (packet[0] & MOUSE_X_OVERFLOW);
  pp.y_ov = (packet[0] & MOUSE_Y_OVERFLOW);

}

int (mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hook_id;
  if(sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) return 1;
  return 0;
}

int (mouse_unsubscribe_int)() {
  if(sys_irqrmpolicy(&mouse_hook_id) != 0) return 1;
  return 0;
}

void (mouse_ih)() {
  if(kbc_read_register(KBC_OUT_BUF,&data) != 0) read_error = 1;
}


void mouse_synch_packet() {
  if(current_index == 0 && (data & BIT(3))) {
    packet[current_index] = data;
    current_index++;
  } else if (current_index > 0) {
    packet[current_index] = data;
    current_index++;
  }
}
