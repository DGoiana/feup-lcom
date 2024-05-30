#include "mouse.h"

uint8_t stat;
uint8_t data_mouse;
uint8_t packet[3];
uint8_t size;
uint32_t num_packet;
int mouse_hook_id = MOUSE_IRQ;
int current_index = 0;
struct packet pp;
bool read_error;


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
  if(kbc_read_register(KBC_OUT_BUF,&data_mouse) != 0) read_error = 1;
}


void mouse_synch_packet() {
  if(current_index == 0 && (data_mouse & BIT(3))) {
    packet[current_index] = data_mouse;
    current_index++;
  } else if (current_index > 0) {
    packet[current_index] = data_mouse;
    current_index++;
  }
}

bool (mouse_collision)(u16_t x,u16_t y,struct collision_box collision_box) {
  u16_t x_diff = x - collision_box.x;
  u16_t y_diff = y - collision_box.y;

  return x_diff < collision_box.width && y_diff - collision_box.height;
}
