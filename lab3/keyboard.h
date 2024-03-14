#include <lcom/lcf.h>

void (kbc_ih)();
void (keyboard_subscribe_int)(uint8_t *bit_no);
void (keyboard_unsubscribe_int)();
void (kbc_issue_command)(uint8_t cmd);
void (timer_ih)();
