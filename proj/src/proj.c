// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/pixmap.h>
#include <lcom/read_xpm.h>

#include "Button.h"
#include "Cursor.h"
#include "Enemy.h"
#include "GameStateMachine.h"
#include "Projectile.h"
#include "Sprite.h"
#include "debug.h"
#include "game.h"
#include "highscores.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "serialport.h"
#include "videocard.h"
#include "wordWriter.h"
#include "xpm_db.h"

/** @defgroup Proj Proj
 * @{
 *
 * Main functions of the project.
 */

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

/**
 * @brief Stub for the driver receive function. (Used to make driver receive appear in the function call diagram)
 * 
 * @param msg                   msg
 * @param ipc_status            ipc_status
 */
void driver_receive_f(message *msg, int *ipc_status);

/**
 * @brief Subscribes the interrupts os the timer, kbd, mouse, RTC and UART. The IRQ Sets are stored in global variables.
 * 
 * @return int          zero if sucessful, non-zero otherwise
 */
int device_interrupt_subscription();

/**
 * @brief Unsubscribes the interrupts os the timer, kbd, mouse, RTC and UART. 
 * 
 * @return int          zero if sucessful, non-zero otherwise
 */
int device_interrupt_unsubscription();

/**
 * @brief Setups settings for the mouse and RTC and initializes the video mode.
 * 
 * @return int          zero if sucessful, non-zero otherwise
 */
int device_setup();

/**
 * @brief  Exits video mode and reverts the settings of the mouse and RTC to the default ones.
 * 
 * @return int          zero if sucessful, non-zero otherwise
 */
int device_reset();

/**
 * @brief Executes the gsm operations that should happen when an keyboard interrupt occurs. (Mainly use the reduce the size of the interrupt loop)
 * 
 */
void kbd_gsm_handler();

/**
 * @brief Executes the gsm operations that should happen when an mouse interrupt occurs. (Mainly use the reduce the size of the interrupt loop)
 * 
 */
void ms_gsm_handler();

/**
 * @brief Executes the gsm operations that should happen when an RTC interrupt occurs. (Mainly use the reduce the size of the interrupt loop)
 * 
 */
void rtc_gsm_handler();

/**
 * @brief Executes the gsm operations that should happen when an UART interrupt occurs. (Mainly use the reduce the size of the interrupt loop)
 * 
 */
void uart_gsm_handler();

extern int timer_global_counter;        /**< @brief global counter updated upon a timer interrupt */
GameStateMachine_t *gsm;                /**< @brief pointer to the state machine of the game */
static __uint64_t irq_set_kbd;          /**< @brief bit mask that has the kbd's interrupt bit set to 1  */
static __uint64_t irq_set_timer;        /**< @brief bit mask that has the timer's interrupt bit set to 1  */
static __uint64_t irq_set_ms;           /**< @brief bit mask that has the mouse's interrupt bit set to 1  */
static __uint64_t irq_set_rtc;          /**< @brief bit mask that has the RTC's interrupt bit set to 1  */
static __uint64_t irq_set_uart;         /**< @brief bit mask that has the UART's interrupt bit set to 1  */

int(proj_main_loop)(int argc, char *argv[]) {

  printf("%s(%d,%p):under construction\n", __func__, argc, argv);

  // initialize srand seed
  seedRnd(time(NULL));

  // enable I/O priveleged operations
  sys_enable_iop(SELF);

  // initializate the UART
  int validInit = uart_init();

  if(validInit != OK)
    return -1;

  // subscribe to MS, KBD, RTC and TIMER interrupts
  int validSubscription = device_interrupt_subscription();

  if(validSubscription != OK)
    return -1;

  // setup the devices
  int validSetup = device_setup();

  if(validSetup != OK)
    return -1;

  // variables used to check for keyboard and interrupt handler errors
  uint8_t kbd_ih_error = 0, ms_ih_error = 0;

  // reset the global timer counter
  timer_global_counter = 0;

  // calculate the display time for each frame
  uint8_t frame_time = sys_hz() / FRAME_RATE;

  ////////////////////
  // LOAD GAME DATA //
  ////////////////////

  // load game assets
  int validLoad;
  validLoad = loadAlphabet();

  if(validLoad != OK)
    return -1;
  
  validLoad = load_game_data();

  if(validLoad != OK)
    return -1;

  // create the state machine that will keep track of the game state
  gsm = gsm_new();

  // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  while (gsm_get_current_state(gsm) != END) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:
          ////////////////////////
          // KEYBOARD INTERRUPT //
          ////////////////////////
          if (msg.m_notify.interrupts & irq_set_kbd) { // check if interrupt comes from the KBC

            kbc_ih();

            kbd_ih_error = is_ih_err(); // store the error status of the keyboard interrupt handler

            if (kbd_ih_error)
              continue;

            kbd_gsm_handler();
           
          }
          /////////////////////
          // MOUSE INTERRUPT //
          /////////////////////
          if (msg.m_notify.interrupts & irq_set_ms) { // check if interrupt comes from the KBC

            mouse_ih(); // get the current scancode using the 'C' keyboard interrupt handler

            ms_ih_error = ms_is_ih_err(); // store the error status of the keyboard interrupt handler

            if (ms_ih_error)
              continue;

            ms_gsm_handler();
          }
          /////////////////////
          // TIMER INTERRUPT //
          /////////////////////
          if (msg.m_notify.interrupts & irq_set_timer) { // check if interrupt comes from the Timer

            timer_int_handler();

            if (timer_global_counter % frame_time == 0)
              gsm_update_game_state(gsm, FRAME_TIME_PASSED);
          }
          ///////////////////
          // RTC INTERRUPT //
          ///////////////////
          if (msg.m_notify.interrupts & irq_set_rtc) {

            rtc_asm_ih_caller();

            rtc_gsm_handler();
          }
          ///////////////////////////////////
          // HANDLE SERIAL PORT INTERRUPTS //
          ///////////////////////////////////
          if (msg.m_notify.interrupts & irq_set_uart) {

            uart_ih();
            uart_parse_msg();

            
            if (!is_msg_complete()) 
              continue;

            

            uart_gsm_handler();
            
          }
          break;
        default:
          break;
      }
    }
  }

  // free dinamically alocated memory
  game_free_data();
  deleteAlphabet();
  
  // delete the game state machine;
  gsm_del(gsm);

  // reset the devices
  int validReset = device_reset();

  // unsubscribe interrupts
  int validUnsubscribe = device_interrupt_unsubscription();

  // clear the buffers to elimate remaining information
  int validKbcClearBuffer = kbc_clear_buffer();
  uart_ih();
 
  return validUnsubscribe || validKbcClearBuffer || validReset;
}


void driver_receive_f(message *msg, int *ipc_status) {
  driver_receive(ANY, msg, ipc_status);
}


int device_interrupt_subscription(){

  /////////////////////////////////
  // SUBSCRIBE DEVICE INTERRUPTS //
  /////////////////////////////////

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit_kbd = 0;
  __uint8_t hookMaskBit_timer = 0;
  __uint8_t hookMaskBit_ms = 0;
  __uint8_t hookMaskBit_rtc = 0;
  __uint8_t hookMaskBit_uart = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = kbd_subscribe_int(&hookMaskBit_kbd);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = timer_subscribe_int(&hookMaskBit_timer);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = ms_subscribe_int(&hookMaskBit_ms);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = rtc_subscribe_int(&hookMaskBit_rtc);

  if (validSubscribe != OK)
    return validSubscribe;

  validSubscribe = uart_subscribe_int(&hookMaskBit_uart);

  if (validSubscribe != OK)
    return validSubscribe;

  // mask used to check if the desired interrupts occurred
  irq_set_kbd = BIT(hookMaskBit_kbd);
  irq_set_timer = BIT(hookMaskBit_timer);
  irq_set_ms = BIT(hookMaskBit_ms);
  irq_set_rtc = BIT(hookMaskBit_rtc);
  irq_set_uart = BIT(hookMaskBit_uart);

  return 0;
}

int device_interrupt_unsubscription(){

  ////////////////////////////
  // UNSUBSCRIBE INTERRUPTS //
  ////////////////////////////

  int validUnsubscribe_KBD = kbd_unsubscribe_int();
  int validUnsubscribe_Timer = timer_unsubscribe_int();
  int validUnsubscribe_MS = ms_unsubscribe_int();
  int validUnsubscribe_RTC = rtc_unsubscribe_int();
  int validUnsubscribe_uart = uart_unsubscribe_int();

  return validUnsubscribe_uart || validUnsubscribe_KBD || validUnsubscribe_Timer || validUnsubscribe_MS || validUnsubscribe_RTC || validUnsubscribe_uart;
}

int device_setup(){

  //////////////////////////
  // SETUP DEVICE DRIVERS //
  //////////////////////////

  // init graphical mode
  vg_init(PROJECT_COLOR_MODE);

  // enable mouse data reporting
  ms_disable_int_not();
  ms_command(EN_DATA_REP);
  ms_enable_int_not();

  // init the rtc settings
  rtc_init();
  rtc_enable_update_int();

  return 0;
}

int device_reset(){

  ///////////////////
  // RESET DEVICES //
  ///////////////////


  // disable data reporting
  ms_disable_int_not();
  ms_command(DIS_DATA_REP);
  // return to text mode
  int validVCExit = vg_exit();
  // revert rtc to its original state
  int validRtcRevert = rtc_revert();

  return validRtcRevert || validVCExit;
}

void kbd_gsm_handler(){

  if (get_scancode() == ESC_BREAKCODE) {
    gsm_update_game_state(gsm, ESC_KEY_RELEASED);
  }
  else {
    gsm_update_game_state(gsm, KEYBOARD_PRESSED);
  }
}

void ms_gsm_handler(){

  ms_process_packet();

  if (is_packet_complete()) {

    struct mouse_ev *ms_evt = mouse_detect_event(get_packet());

    if (gsm_get_current_state(gsm) == GAME_PLAY) {

      int res = check_vert_line_gest(ms_evt);

      if (res == 1) {
        gsm_update_game_state(gsm, LEFT_P_UP);
      }
      else if (res == 2)
        gsm_update_game_state(gsm, RIGHT_P_UP);
    }
    else {

      gsm_update_game_state(gsm, MOUSE_MOVED);

      if (ms_evt->type == LB_PRESSED)
        gsm_update_game_state(gsm, MOUSE_LB_PRESSED);
    }
  }
}

void rtc_gsm_handler(){

  // check if the interrupt was due to an alarm
  if (rtc_ack_alarm_asm()) {
    gsm_update_game_state(gsm, ALARM_RING);
  }

  // check if the interrupt was due to a periodic interrupt
  if (rtc_ack_update_asm()) {
    gsm_update_game_state(gsm, SEC_PASSED);
  }
}

void uart_gsm_handler(){

  struct uart_msg *msg = uart_get_last_msg();

  switch (msg->type) {

    case ACK_JOIN:

      gsm_update_game_state(gsm, START_2P);
      break;

    case JOIN:

      if (gsm_get_current_state(gsm) == GAME_PLAY) {
        uart_send_ack_join_msg();
        gsm_update_game_state(gsm, JOINED_2P);
      }

      break;

    case LEAVE:

      if (gsm_get_current_state(gsm) == GAME_PLAY)
        gsm_update_game_state(gsm, LEFT_2P);

      break;

    case P1_LEAVE:

      

      if (gsm_get_current_state(gsm) == SEC_GAME_PLAY)
        gsm_update_game_state(gsm, LEFT_1P);

      break;

    case KBD_CHAR:

      gsm_update_game_state(gsm, NEW_CHAR_2P);
      break;

    case ADD_CHAR:

      if (gsm_get_current_state(gsm) == SEC_GAME_PLAY) {
        gsm_update_game_state(gsm, SEC_GAME_ADD_CHAR);
      }
      break;
    case REM_CHAR:

      if (gsm_get_current_state(gsm) == SEC_GAME_PLAY) {
        gsm_update_game_state(gsm, SEC_GAME_REM_CHAR);
      }
      break;

    default:
      break;
  }
}
