#include <lcom/lcf.h>

#include <lcom/lab2.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8254.h"

extern int gCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {

  uint8_t status; //variable in which the current status of the select timer is stored

  // validGet stores the validity of timer_get_conf to deal with error cases
  int validGet = timer_get_conf(timer, &status);

  if (validGet != OK)
    return validGet;

  // validDisp stores the validity of timer_display_conf to deal with error cases
  int validDisp = timer_display_conf(timer, status, field);

  if (validDisp != OK)
    return validDisp;

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {

  // validSet stores the validity of timer_set_frequency to deal with error cases
  int validSet = timer_set_frequency(timer, freq);
  
  return validSet;
}

int(timer_test_int)(uint8_t time) {

  if (time == 0)
    return 0;

  // Storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = timer_subscribe_int(&hookMaskBit);

  if (validSubscribe != OK)
    return validSubscribe;

  // Mask used to check if the desired interrupt occured
  __uint64_t irq_set = BIT(hookMaskBit);

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // stop condition is if the specified time interval has passed
  while (gCounter< time * 60) {
    // Get a request message

    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) {

        case HARDWARE: // hardware interrupt notification

          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt

            timer_int_handler();

            // check if a second as passed
            if (gCounter % 60 == 0) {
            
              /*
              int validPrint; // validPrint checks if the function timer_print_elapsed_time runs without error
              if ((validPrint = timer_print_elapsed_time()) != OK)
                return validPrint; 

              in the lectures' slides it says that timer_print_elapsed_time doesn't return anything,although in the actual code it is declared as returning an int, which is always 1 even though the function seems to run correctly(doesn't fail the given tests), so we decided not to check the returned value from timer_print_elapsed_time
              */

              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;
      }
    }
  }

  return timer_unsubscribe_int();
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  *lsb = (uint8_t) val; // stores in lsb the least significant byte of val

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {

  *msb = (uint8_t)(val >> 8); // stores in msb the most significant byte of val

  return 0;
}
