

#include "utils.h"

int cnt = 0;

int (util_sys_inb)(int port,uint8_t *value) {
  uint32_t long_value;
  int ret = sys_inb(port,&long_value);
  cnt++;
  
  *value = 0xFF & long_value;
  return ret;
  
}
