#include "graphics_test.h"
#include "../../drivers/graphic/graphic.h"
#include "../../drivers/keyboard/keyboard.h"
#include "../../strings/strings.h"

int (test_graphic)(){
  if(initFrameBuffer(0x115) != 0) return 1;
  if(startVideoMode(0x115) != 0) return 1;

  //paint backroung to white and draw rectangles
  if(formatBackground(video_mem) != 0) return 1;

  wait_for_esc_key();

  if(exitVideoMode() != 0) return 1;
  return 0;
}
