// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "keyboard.h"
#include "logical_and_stm.h"
#include "mouse.h"
#include "util_lab4.h"

extern uint32_t gCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit = 0;

  // validCommand stores the validity of the process of writing a command to the mouse
  int validCommand = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = ms_subscribe_int(&hookMaskBit);

  // validIntCmd stores the validity of int . notification enabling and disabling
  int validIntCmd = 0;

  if (validSubscribe != OK)
    return validSubscribe;

  // disable interrupt notifications in order to write a command to the mouse
  validIntCmd = ms_disable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  //enable data reporting
  validCommand = ms_command(EN_DATA_REP);

  if (validCommand != OK)
    return validCommand;

  // enable interrupt notifications
  validIntCmd = ms_enable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  __uint64_t irq_set = BIT(hookMaskBit); // mask used to check if the desired interrupt occurred

  uint8_t packetByte = 0;    // variable used to store the current byte of a packet
  uint8_t packetByteNum = 0; //variable used to store the number of the current byte of the packet
  uint32_t packetCnt = 0;    // variable used to store the number of packets that have been stored
  uint8_t ih_error;          // ih_error stores the validity of the interrupt handler
  struct packet pPacket;     // struct where the informations of a packet are stored

  // while the number of packets doesn't exceed the desired count
  while (packetCnt < cnt) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set) { // check if interrupt comes from the KBC(mouse)

            mouse_ih(); // get the current scancode using the 'mouse interrupt handler

            ih_error = ms_is_ih_err();     // store the error status of the mouse interrupt handler
            packetByte = ms_get_ih_byte(); // store one byte of a the current packet

            if (!ih_error) //if the scancode is valid, process it
              ms_process_packet(&pPacket, &packetByteNum, packetByte, &packetCnt);
          }
          break;
        default:
          break;
      }
    }
  }

  validIntCmd = ms_disable_int_not(); // disable mouse interrupt notifications

  if (validIntCmd != OK)
    return validIntCmd;

  validCommand = ms_command(DIS_DATA_REP); //disable data reporting

  if (validCommand != OK)
    return validCommand;

  return ms_unsubscribe_int(); // unsubscribe from the mouse interrupts
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {

  uint8_t statusByte = 0;    // variable used to store the KBC status byte
  uint32_t packetByte = 0;   // variable used to store the current byte of a packet
  uint8_t packetByteNum = 0; //variable used to store the number of the current byte of the packet
  uint32_t packetCnt = 0;    // variable used to store the number of packets that have been stored

  int validCommand = 0; // validCommand stores the validity of the process of writing a command to the mouse

  struct packet pPacket; // struct where the informations of a packet are stored

  //  Disable data reporting and set remote mode

  validCommand = ms_command(DIS_DATA_REP);

  if (validCommand != OK)
    return validCommand;

  validCommand = ms_command(SET_REMOTE_MD);

  if (validCommand != OK)
    return validCommand;

  // while the number of packets doesn't exceed the desired count
  while (packetCnt < cnt) {

    // in case we are starting the polling for a new packet, send data packet request to the mouse
    if (packetByteNum == 0) {

      validCommand = ms_command(READ_DATA);

      if (validCommand != OK)
        return validCommand;
    }

    while (true) {

      if (kbd_read_status(&statusByte) != OK) //get the status byte
        continue;

      //if the OBF is full and the information is from the mouse, read the packet byte
      if ((statusByte & OBF_FULL) && (statusByte & MS_DATA)) {

        if (sys_inb(OUT_BUFFER, &packetByte) != OK) {
          continue;
        }
        if ((statusByte & (PAR_ERR | TO_ERR)) == OK) //check for parity and timeout errors
          break;
        else
          continue;
      }

      tickdelay(micros_to_ticks(DELAY_US)); // introduce a waiting period to account for KBC internal delays CUIDADO AQUI
    }

    ms_process_packet(&pPacket, &packetByteNum, (uint8_t) packetByte, &packetCnt); //process the packet byte

    // after finishing the processing of a packet, wait for the determined ammount of time
    if (packetByteNum == 0)
      tickdelay(micros_to_ticks(period * 1000));
  }

  //Set stream mode, disable data reporting and reverse the KBC command byte to it's default value

  validCommand = ms_command(SET_STREAM_MD);

  if (validCommand != OK)
    return validCommand;

  validCommand = ms_command(DIS_DATA_REP);

  if (validCommand != OK)
    return validCommand;

  uint8_t commandByteInit = minix_get_dflt_kbc_cmd_byte(); // variable used to store the default command byte of the kbc

  int errByte = 0; // errByte stores the validity of writing a command to the kbc
  kbc_command(WRITE_CMD_BT, commandByteInit, &errByte);

  if (errByte != OK)
    return errByte;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // validIntCmd stores the validity of int . notification enabling and disabling
  int validIntCmd = 0;

  // validCommand stores the validity of the process of writing a command to the mouse
  int validCommand = 0;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit_ms = 0;
  __uint8_t hookMaskBit_timer = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = timer_subscribe_int(&hookMaskBit_timer);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = ms_subscribe_int(&hookMaskBit_ms);

  if (validSubscribe != OK)
    return validSubscribe;

  validIntCmd = ms_disable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  //enable data reporting
  validCommand = ms_command(EN_DATA_REP);

  if (validCommand != OK)
    return validCommand;

  validIntCmd = ms_enable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  __uint64_t irq_set_ms = BIT(hookMaskBit_ms);       // mask used to check if the desired interrupt occurred
  __uint64_t irq_set_timer = BIT(hookMaskBit_timer); // mask used to check if the desired interrupt occurred

  uint8_t packetByte = 0;    // variable used to store the current byte of a packet
  uint8_t packetByteNum = 0; //variable used to store the number of the current byte of the packet
  uint32_t packetCnt = 0;    // variable used to store the number of packets that have been stored
  uint8_t ih_error;          // ih_error stores the validity of the interrupt handler
  struct packet pPacket;     // struct where the informations of a packet are stored

  uint32_t timer_freq = sys_hz();

  // while the number of packets doesn't exceed the desired count
  while (gCounter < idle_time * timer_freq) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & irq_set_ms) { // check if interrupt comes from the KBC(mouse)

            mouse_ih(); // get the current scancode using the mouse interrupt handler

            ih_error = ms_is_ih_err();     // store the error status of the mouse interrupt handler
            packetByte = ms_get_ih_byte(); // store one byte of a the current packet

            if (!ih_error) { //if the scancode is valid, process it

              if (packetByteNum == 2)
                gCounter = 0;

              ms_process_packet(&pPacket, &packetByteNum, packetByte, &packetCnt);
            }
          }
          break;
        default:
          break;
      }
    }
  }

  validIntCmd = ms_disable_int_not(); // disable mouse interrupt notifications

  if (validIntCmd != OK)
    return validIntCmd;

  validCommand = ms_command(DIS_DATA_REP); // disable data reporting

  if (validCommand != OK)
    return validCommand;

  validIntCmd = ms_enable_int_not(); // enable mouse interrupt notifications

  if (validIntCmd != OK)
    return validIntCmd;

  // unsubscribe from the timer and mouse interrupts
  int validUnsubscribe_ms = ms_unsubscribe_int();
  int validUnsubscribe_timer = timer_unsubscribe_int();

  return validUnsubscribe_ms || validUnsubscribe_timer; // if one of these values is different than 0, returns different than 0
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // validIntCmd stores the validity of int . notification enabling and disabling
  int validIntCmd = 0;

  // validCommand stores the validity of the process of writing a command to the mouse
  int validCommand = 0;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = ms_subscribe_int(&hookMaskBit);

  if (validSubscribe != OK)
    return validSubscribe;

  validIntCmd = ms_disable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  //Enable data reporting

  validCommand = ms_command(EN_DATA_REP);

  if (validCommand != OK)
    return validCommand;

  validIntCmd = ms_enable_int_not();

  if (validIntCmd != OK)
    return validIntCmd;

  __uint64_t irq_set = BIT(hookMaskBit); // mask used to check if the desired interrupt occurred

  uint8_t packetByte = 0;    // variable used to store the current byte of a packet
  uint8_t packetByteNum = 0; // variable used to store the number of the current byte of the packet
  uint32_t packetCnt = 0;    // variable used to store the number of packets that have been stored
  uint8_t ih_error;          // ih_error stores the validity of the interrupt handler
  struct packet pPacket;     // struct where the informations of a packet are stored

  uint32_t event = 0; // event mask

  bool final_state = false;

  // while the number of packets doesn't exceed the desired count
  while (!final_state) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set) { // check if interrupt comes from the KBC(mouse)

            mouse_ih(); // get the current scancode using the 'mouse interrupt handler

            ih_error = ms_is_ih_err();     // store the error status of the mouse interrupt handler
            packetByte = ms_get_ih_byte(); // store one byte of a the current packet

            if (!ih_error) { //if the scancode is valid, process it

              if (packetByteNum == 2)
                event |= MOUSE_EVT; // set mouse event bit to 1 in the event mask

              ms_process_packet(&pPacket, &packetByteNum, packetByte, &packetCnt);
            }
          }
          break;
        default:
          break;
      }
    }
    if (event & MOUSE_EVT) {

      /*
       * DETECT MOUSE EVENT 
       */

      struct mouse_ev *ms_evt = ms_detect_event(&pPacket);

      /*
       * HANDLE MOUSE EVENT
       */

      check_logical_and(ms_evt, &final_state, x_len, tolerance);

      free(ms_evt); // free the memory allocated to the ms_evt struct
    }

    event = 0; // reset event mask to 0
  }

  validIntCmd = ms_disable_int_not(); // disable mouse interrupt notifications

  if (validIntCmd != OK)
    return validIntCmd;

  validCommand = ms_command(DIS_DATA_REP); //disable data reporting

  if (validCommand != OK)
    return validCommand;

  return ms_unsubscribe_int(); // unsubscribe from the mouse interrupts
}
