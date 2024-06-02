#ifndef _LCOM_TIMER_H_
#define _LCOM_TIMER_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

/**
 * @brief This file stores all the timer related functions.\n
*/

/**
 * @brief Set timer frequency.\n
*/
int (timer_set_frequency)(uint8_t timer, uint32_t freq);
/**
 * @brief Subscribes to timer interrupts.\n
*/
int (timer_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribes to timer interrupts.\n
*/
int (timer_unsubscribe_int)();
/**
 * @brief Timer interrupt handler.\n
*/
void (timer_int_handler)();
/**
 * @brief Get timer current configuration.\n
*/
int (timer_get_conf)(uint8_t timer, uint8_t *st);

#endif
