

#include <lcom/lcf.h>
#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(val == 0){
    return 1;
  }
  *lsb = (uint8_t) (val & 0x00FF);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(val == 0){
    return 1;
  }
  *msb = (uint8_t) ((val & 0xFF00) >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t long_value;
  int ret = sys_inb(port,&long_value);
  *value = 0xFF & long_value;
  return ret;
}

