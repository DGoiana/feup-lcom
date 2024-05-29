#ifndef _LCOM_KBC_TESTS_H_
#define _LCOM_KBC_TESTS_H_


#include "../../drivers/timer/timer.h"
#include "../../drivers/keyboard/keyboard.h"
#include "../../drivers/mouse/mouse.h"


#define MOUSE_DISABLE_DATA_REPORTING 0xF5

int (test_keyboard)();
int (test_timer)();
int (test_mouse)();
int (wait_for_esc_key)();

#endif
