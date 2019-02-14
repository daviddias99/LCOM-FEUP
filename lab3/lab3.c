#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"
#include "util_lab3.h"

extern int num_sysinb_calls;
extern uint8_t scanbyte;
extern uint8_t ih_error;
extern int gCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)(bool assembly) {

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = kbd_subscribe_int(&hookMaskBit);

  if (validSubscribe != OK)
    return validSubscribe;
                                          
  __uint64_t irq_set = BIT(hookMaskBit); // mask used to check if the desired interrupt occurred
  
  bool escapedSeq = false; // variable used to check for two-byte scancodes                       
  uint8_t scancode = 0; // variable used to store the scancode
  uint8_t byte[2]; // byte stores the bytes of a scan code

  // while scancode is not ESC breakcode
  while (scancode != ESC_BREAKCODE) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set) { // check if interrupt comes from the KBC

            if (assembly) {
              kbc_asm_ih(); // get the current scancode using the 'assembly' keyboard interrupt handler

              scancode = scanbyte; // store the scancode
            }
            else {
              kbc_ih(); // get the current scancode using the 'C' keyboard interrupt handler

              ih_error = is_ih_err();    // store the error status of the keyboard interrupt handler
              scancode = get_scancode(); // store the scancode
            }

            if (!ih_error) //if the scancode is valid, process it
              kbd_process_scancode(byte, &escapedSeq, scancode);
          }
          break;
        default:
          break;
      }
    }
  }

  // print the number of 'sys_inb' calls
  if (!assembly)
    kbd_print_no_sysinb(num_sysinb_calls);

  return kbd_unsubscribe_int(); // unsubscribe from the keyboard interrupts
}

int(kbd_test_poll)() {

  
  bool escapedSeq = false; // variable used to check for two-byte scancodes
  uint8_t byte[2]; // byte stores the bytes of a scan code
  uint32_t scancode = 0; // variable used to store the scancode

 
  uint8_t statusByte = 0;  // variable used to store the KBC status byte
  uint8_t commandByteInit = 0; // variable used to store the current command byte of the kbc

  // while scancode is not ESC breakcode
  while (scancode != ESC_BREAKCODE) {

    // get the scancode using polling

    while (true) {

      if (kbd_read_status(&statusByte) != OK) //get the status byte
        continue;

      //if the OBF is full and the information isn't from the mouse, read the scancode
      if ((statusByte & OBF_FULL) && !(statusByte & MS_DATA)) {

        if (sys_inb_cnt(OUT_BUFFER, &scancode) != OK) {
          continue;
        }
        if ((statusByte & (PAR_ERR | TO_ERR)) == OK) //check for parity and timeout errors
          break;
        else
          continue;
      }

      tickdelay(micros_to_ticks(DELAY_US)); // introduce a waiting period to account for KBC internal delays
    }

    kbd_process_scancode(byte, &escapedSeq, (uint8_t) scancode); //process the scancode
  }

  // validCommand stores the validity of the process of writing a command to the KBC
  int validCommand = 0;

  // store the current command byte
  commandByteInit = kbc_command(READ_CMD_BT, 0, &validCommand);

  if (validCommand != OK)
    return validCommand;

  
  // toggle the 'enable interrupt on OBF, from keyboard' bit on the command byte
  uint8_t newCmdByte = commandByteInit | KBD_INT_EN;

  // write the new commmand byte to the KBC
  kbc_command(WRITE_CMD_BT, newCmdByte, &validCommand);

  if (validCommand != OK)
    return validCommand;

  // print the number of 'sys_inb' calls
  kbd_print_no_sysinb(num_sysinb_calls);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

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

 
  bool escapedSeq = false;  // variable used to check for two-byte scancodes
  uint8_t scancode = 0;  // variable used to store the scancode«
  uint8_t byte[2]; // byte stores the bytes of a scan code

  // while scancode is not ESC breakcode
  while ((scancode != ESC_BREAKCODE) && (gCounter < n * 60)) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set_KBD) { // check if interrupt comes from the KBC

            gCounter = 0;
            kbc_ih(); // get the current scancode

            ih_error = is_ih_err();    //store the error status of the keyboard input handler
            scancode = get_scancode(); // store the scancode

            if (!ih_error) //if the scancode is valid, process it
              kbd_process_scancode(byte, &escapedSeq, scancode);
          }
          else if (msg.m_notify.interrupts & irq_set_Timer) { // check if interrupt comes from the Timer

            timer_int_handler(); // increase the global time variable
          }
          break;
        default:
          break;
      }
    }
  }

  // print the number of 'sys_inb' calls
  kbd_print_no_sysinb(num_sysinb_calls);


  // unsubscribe from the timer and keyboard interrupts
  int validUnsubscribe_KBD = kbd_unsubscribe_int();
  int validUnsubscribe_Timer = timer_unsubscribe_int();

  return validUnsubscribe_KBD || validUnsubscribe_Timer; // if one of these values is different than 0, returns different than 0
}
