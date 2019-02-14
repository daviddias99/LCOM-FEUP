#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hookID = 3;
int timer_global_counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {

  // frequency can't be smaller than 19, otherwise the initial count value overflows
  if (freq < 19 || freq > TIMER_FREQ)
    return -1;

  // invalid timer
  if (timer > 2)
    return -2;

  // freq = TIMER_FREQ/initValue
  uint16_t initValue = TIMER_FREQ / freq;
  uint8_t lsb, msb;

  // get timer's current control word
  uint8_t controlWord;

  // validGet stores the validity of get operations to deal with error cases
  int validGet = timer_get_conf(timer, &controlWord);

  if (validGet != OK)
    return validGet;

  // make controlWord's 4 msb 0, while preserving the 4 lsb from the timer's previous control word
  controlWord &= 0x0F;

  // count initialization mode: msb after lsb
  controlWord |= TIMER_LSB_MSB;

  // change the three more significant bits of the control word in order to select the timer
  switch (timer) {
    case 0:
      controlWord |= TIMER_SEL0;
      break;
    case 1:
      controlWord |= TIMER_SEL1;
      break;
    case 2:
      controlWord |= TIMER_SEL2;
      break;
  }

  // store the LSB and MSB of the initial value in seperate variables in order to input to the timer register the initial value (due to MSB after LSB mode)

  validGet = util_get_LSB(initValue, &lsb);

  if (validGet != OK)
    return validGet;

  validGet = util_get_MSB(initValue, &msb);

  if (validGet != OK)
    return validGet;

  uint8_t chosenTimer;

  switch (timer) {
    case 0:
      chosenTimer = TIMER_0;
      break;
    case 1:
      chosenTimer = TIMER_1;
      break;
    case 2:
      chosenTimer = TIMER_2;
      break;
  }

  // validWrite stores the validity of write operations to deal with error cases
  int validWrite;

  // write control word
  validWrite = sys_outb(TIMER_CTRL, (uint32_t) controlWord);

  if (validWrite != OK)
    return validWrite;

  // write count lsb
  validWrite = sys_outb(chosenTimer, (uint32_t) lsb);

  if (validWrite != OK)
    return validWrite;

  // write count msb
  validWrite = sys_outb(chosenTimer, (uint32_t) msb);

  return validWrite;
}

int(timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookID);
}

int(timer_unsubscribe_int)() {

  return sys_irqrmpolicy(&hookID);
}

void(timer_int_handler)() {

  timer_global_counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  // invalid timer
  if (timer > 2)
    return -2;

  // read-back command with count deactivated
  uint8_t readBackCommand = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  // stores the status read with function sys_inb
  uint32_t temp;

  // validGet stores the validity of operations in order to deal with error cases
  int validOp;

  validOp = sys_outb(TIMER_CTRL, (uint32_t) readBackCommand);

  if (validOp != OK)
    return validOp;

  switch (timer) {

    case 0:
      validOp = sys_inb(TIMER_0, &temp);
      break;
    case 1:
      validOp = sys_inb(TIMER_1, &temp);
      break;
    case 2:
      validOp = sys_inb(TIMER_2, &temp);
      break;
  }

  if (validOp != OK)
    return validOp;

  *st = (uint8_t) temp; //stores the status word in the variable st doing the proper conversion

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  // invalid timer
  if (timer > 2)
    return -1;

  union timer_status_field_val stField;

  switch (field) {

    case all: // store the whole byte of the timer config
      stField.byte = st;
      break;
    case initial: // store the initialization mode
      st = ((uint8_t)(st & TIMER_LSB_MSB) >> 4);
      stField.in_mode = st;
      break;
    case mode: // store the couting mode
      st = ((uint8_t)(st >> 1) & 0x07);
      stField.count_mode = st;
      break;
    case base: // store the base, BCD or Binary
      st = st & TIMER_BCD;
      stField.bcd = st;
      break;
    default:
      return -2; // invalid field value
      break;
  }

  // print the timer configuration
  // validPrint stores the validity of timer_print_config to deal with error cases
  int validPrint = timer_print_config(timer, field, stField);

  return validPrint;
}
