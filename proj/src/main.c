#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

#include "test_drivers/kbc/kbc_tests.h"
#include "test_drivers/serial/serial_tests.h"
#include "drivers/serial/uart.h"
#include "test_drivers/graphics/graphics_test.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g1/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g1/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int(proj_main_loop)(int argc,char* argv[]) {
  //int length = 1;
  //char *strings[] = {"goiana cringe lmao\n"};
  //ser_test_poll(COM1_BASE, atoi(argv[0]) ,WORD_LENGTH_8, NUM_STOP_2, PARITY_EVEN, BIT_RATE, length,strings);
  //ser_test_fifo(COM1_BASE, atoi(argv[0]) ,WORD_LENGTH_8, NUM_STOP_2, PARITY_EVEN, BIT_RATE, length,strings);
  test_graphic();
  //test_mouse();
  return 0;
}

