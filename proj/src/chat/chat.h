#ifndef _LCOM_CHAT_H_
#define _LCOM_CHAT_H_

#include "../drivers/timer/timer.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"
#include "../assets/letters.xpm"
#include "../strings/strings.h"
#include "../drivers/graphic/graphic.h"
#include "../drivers/serial/uart.h"
#include "../drivers/serial/serial.h"
#include "../drivers/mouse/i8042.h"
#include "../drivers/utils/utils.h"

#define MAX_NUM_MESSAGE 1024

extern char* alphabet;
extern bool in_menu;
extern int num_messages;
extern bool *seen;

/**
 * @brief The chat's main loop which uses the different drivers and auxiliary functions\n
 * alphabet[]: array of the available symbols.\n
 * in_menu: boolean to determine if the user is in the menu before chatting.\n
 * num_messages: number of messages.\n
 * seen[]: array that determines if the message was seen.\n
 * 
*/
int (chat_main)();
/**
 * @brief Main display of the program.\n
*/
int (chat_display)();

#endif
