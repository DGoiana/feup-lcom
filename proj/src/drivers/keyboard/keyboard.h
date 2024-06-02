#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include "../mouse/i8042.h"

/**
 * @brief This file stores all the keyboard related functions.\n
*/

/**
 * @brief Keyboard interrupt handler.\n
*/
void (kbc_ih)();

/**
 * @brief Keyboard Subscription to the interrupts function.\n
*/
void (keyboard_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Keyboard Remove subscription to the interrupts function.\n
*/
void (keyboard_unsubscribe_int)();

/**
 * @brief Timer interrupt handler.\n
*/
void (timer_ih)();

/**
 * @brief Issue a command to the KBC.\n
*/
int (kbc_issue_command)(uint8_t port,uint8_t message);

/**
 * @brief Read value from register in the KBC.\n
*/
int (kbc_read_value)(uint8_t port, uint8_t *message);

/**
 * @brief Restore the state of the KBC.\n
*/
int (restore_kbc)();

#endif
