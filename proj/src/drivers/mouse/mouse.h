#ifndef _LCOM_MOUSE_H
#define _LCOM_MOUSE_H

#include <lcom/lcf.h>
#include "i8042.h"
#include "../../drivers/utils/utils.h"


/**
 * @brief This file stores all the mouse related functions.\n
*/

/**
 * @brief Writes to KBC register.\n
*/
int (kbc_write_register)(uint8_t port,uint8_t message);
/**
 * @brief Reads from KBC register.\n
*/
int (kbc_read_register)(uint8_t port, uint8_t *message);
/**
 * @brief Write to the Mouse KBC register.\n
*/
int (mouse_write_register)(uint8_t command);
/**
 * @brief Reads from Mouse KBC register.\n
*/
int (mouse_read_command_byte)(uint8_t *value);
/**
 * @brief Build a mouse packet.\n
*/
void (mouse_build_packet)();
/**
 * @brief Subscribes to Mouse interrupts.\n
*/
int (mouse_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribes to Mouse interrupts.\n
*/
int (mouse_unsubscribe_int)();
/**
 * @brief Mouse Interrupt handler.\n
*/
void (mouse_ih)();
/**
 * @brief Synchronizes mouse packets.\n
*/
void (mouse_synch_packet)();
/**
 * @brief Checks for mouse collision.\n
*/
bool (mouse_collision)(u16_t x,u16_t y,struct collision_box collision_box);

#endif
