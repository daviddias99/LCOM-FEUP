// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "i8254.h"
#include "videocard.h"
#include "videocard_util.h"

extern int gCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  // set the desired graphics mode

  int validInit = init_vbe_mode(mode);

  if (validInit != OK)
    return validInit;

  // wit for the desired ammount of seconds

  //tickdelay(micros_to_ticks(delay * 1000000));

  tickdelay(micros_to_ticks(SECONDS_TO_MICROS(delay)));

  // reset minix to the default mode  (text mode)

  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  // set the desired graphics mode and setup the frame buffer
  void *init_state = vg_init(mode);

  if (init_state == NULL)
    return 1;

  // draw the rectangle in the desired position, with the desired size and color
  int validDraw = vg_draw_rectangle(x, y, width, height, color);

  if (validDraw != OK)
    return validDraw;

  // wait for the ESC key to be pressed before terminating the program
  int validKeyboadOp = waitForEscape();

  // if there was some kind of error with the keyboard handling, terminate the program
  if (validKeyboadOp != OK)
    return validKeyboadOp;

  // reset minix to the default mode  (text mode)
  return vg_exit();
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  // set the desired graphics mode and setup the frame buffer
  void *init_state = vg_init(mode);

  if (init_state == NULL)
    return 1;

  //  get the width and height of each rectangle and the size of the black lines
  //  that are visible when the width and height isn't divisible by the rectangle number

  uint16_t hStripeSize = getHRes() % no_rectangles;
  uint16_t recWidth = (getHRes() - hStripeSize) / no_rectangles;

  uint16_t vStripeSize = getVRes() % no_rectangles;
  uint16_t recHeight = (getVRes() - vStripeSize) / no_rectangles;

  uint32_t color = 0;

  // draw the rectangles
  for (uint8_t v = 0; v < no_rectangles; v++) {

    for (uint8_t h = 0; h < no_rectangles; h++) {

      //get the color of the curent rectangle
      color = getColor(mode, no_rectangles, v, h, first, step);

      //draw the rectangle
      int validDraw = vg_draw_rectangle(recWidth * h, recHeight * v, recWidth, recHeight, color);

      if (validDraw != OK)
        return validDraw;
    }
  }

  // wait for the ESC key to be pressed before terminating the program
  int validKeyboadOp = waitForEscape();

  // if there was some kind of error with the keyboard handling, terminate the program
  if (validKeyboadOp != OK)
    return validKeyboadOp;

  // reset minix to the default mode  (text mode)
  return vg_exit();
}

int(video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y) {

  // set the desired graphics mode and setup the frame buffer
  void *init_state = vg_init(INDEXED_COLOR_MODE);

  if (init_state == NULL)
    return 1;

  // draw the xpm in the desired position
  int validDraw = draw_xpm(xpm, x, y);

  if (validDraw != OK)
    return validDraw;

  // wait for the ESC key to be pressed before terminating the program
  int validKeyboadOp = waitForEscape();

  // if there was some kind of error with the keyboard handling, terminate the program
  if (validKeyboadOp != OK)
    return validKeyboadOp;

  return vg_exit();
}

int(video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit_KBD = 0;
  __uint8_t hookMaskBit_Timer = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = kbd_subscribe_int(&hookMaskBit_KBD);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = timer_subscribe_int(&hookMaskBit_Timer);

  if (validSubscribe != OK)
    return validSubscribe;

  // mask used to check if the desired interrupts occurred
  __uint64_t irq_set_KBD = BIT(hookMaskBit_KBD);
  __uint64_t irq_set_Timer = BIT(hookMaskBit_Timer);

  uint8_t scancode = 0; // variable used to store the scancode«
  uint8_t ih_error = 0;

  vg_init(INDEXED_COLOR_MODE);

  uint32_t freq = sys_hz();

  enum mov_type mt;

  


  if (xf - xi > 0) {
    mt = right;
  }
  else if (xf - xi < 0) {
    mt = left;
  }
  else if (yf - yi > 0) {
    mt = down;
  }
  else if (yf - yi < 0) {
    mt = up;
  }

  uint16_t x = xi;
  uint16_t y = yi;
  draw_xpm(xpm, x, y);

  uint16_t frames_since_update = 0;

  gCounter = 0;

  // while scancode is not ESC breakcode
  while (scancode != ESC_BREAKCODE) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set_KBD) { // check if interrupt comes from the KBC

            kbc_ih(); // get the current scancode using the 'C' keyboard interrupt handler

            ih_error = is_ih_err(); // store the error status of the keyboard interrupt handler

            scancode = get_scancode(); // store the scancode
          }
          if (msg.m_notify.interrupts & irq_set_Timer) { // check if interrupt comes from the Timer

            timer_int_handler();

            if (gCounter % (freq / fr_rate) == 0) {

              if (x != xf || y != yf) {

                if (speed < 0) {
                  if (frames_since_update == abs(speed)) {
                    clear_screen();

                    switch (mt) {
                      case right:
                        x++;
                        break;
                      case left:
                        x--;
                        break;
                      case up:
                        y--;
                        break;
                      case down:
                        y++;
                        break;
                    }

                    draw_xpm(xpm, x, y);
                    frames_since_update = 0;
                  }
                  else {
                    frames_since_update++;
                  }
                }
                else {
                  clear_screen();

                  switch (mt) {
                    case right:
                      if (x + speed > xf)
                        x = xf;
                      else
                        x += speed;
                      break;
                    case left:
                      if (x - speed < xf)
                        x = xf;
                      else
                        x -= speed;
                      break;
                    case up:
                      if (y - speed < yf)
                        y = yf;
                      else
                        y -= speed;
                      break;
                    case down:
                      if (y + speed > yf)
                        y = yf;
                      else
                        y += speed;
                      break;
                  }

                  draw_xpm(xpm, x, y);
                }
              }
            }
          }
          break;
        default:
          break;
      }
    }
  }

  // unsubscribe from the timer and keyboard interrupts
  int validUnsubscribe_KBD = kbd_unsubscribe_int();
  int validUnsubscribe_Timer = timer_unsubscribe_int();
  // return to text mode
  int validVCExit = vg_exit();

  return validUnsubscribe_KBD || validUnsubscribe_Timer || validVCExit; // if one of these values is different than 0, returns different than 0
}

int(video_test_controller)() {

  //  struct where the information about the VBE controller info is stored
  vg_vbe_contr_info_t vbe_ci;

  // initialize the members of the controller info struct
  int validGet = vbe_controller_info_get(&vbe_ci);

  if (validGet != OK)
    return validGet;

  // print the information about the controller
  int validPrint = vg_display_vbe_contr_info(&vbe_ci);

  // if the info display isn't valid, terminate the program
  if (validPrint != OK)
    return validPrint;

  return 0;
}
