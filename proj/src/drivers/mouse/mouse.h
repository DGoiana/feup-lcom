#ifndef _LCOM_MOUSE_H
#define _LCOM_MOUSE_H

#include <lcom/lcf.h>
#include "i8042.h"
#include "../../drivers/utils/utils.h"


/**
 * @brief This file stores all the mouse related functions.\n
*/
int (kbc_write_register)(uint8_t port,uint8_t message);
int (kbc_read_register)(uint8_t port, uint8_t *message);
int (mouse_write_register)(uint8_t command);
int (mouse_read_command_byte)(uint8_t *value);
void (mouse_build_packet)();
int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
void (mouse_synch_packet)();
bool (mouse_collision)(u16_t x,u16_t y,struct collision_box collision_box);

#endif
