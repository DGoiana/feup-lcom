#ifndef _LCOM_TESTS_H_
#define _LCOM_TESTS_H_


#include "../drivers/timer/timer.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"
#include "../drivers/graphic/graphic.h"


#define MOUSE_DISABLE_DATA_REPORTING 0xF5

int (test_keyboard)();
int (test_timer)();
int (test_mouse)();
int (test_graphic)();

#endif
