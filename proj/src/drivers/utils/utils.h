
#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>
#include <stdint.h>

struct collision_box {
  u16_t x;
  u16_t y;
  u16_t width;
  u16_t height;
};

/**
 * @brief This file stores all auxiliary functions to the drivers.\n
*/

/**
 * @brief sends the LSB of a u16_t value.\n
*/
int(util_get_LSB)(uint16_t val, uint8_t *lsb);
/**
 * @brief sends the MSB of a u16_t value.\n
*/
int(util_get_MSB)(uint16_t val, uint8_t *msb);
/**
 * @brief Reads a byte on a register.\n
*/
int (util_sys_inb)(int port, uint8_t *value);

#endif
