#include <lcom/lcf.h>

#include "i8042.h"
#include "keyboard.h"

/**
 * @brief Bit we wish to set to one in the mask returned by an interrupt
 */
static int hookID = 4;
/**
 * @brief Last scancode read in the interrupt handler
 */
static uint32_t scancode = 0;
/**
 * @brief Flag set to something other than 0 if there is an error in the interrupt handler
 */
static int ih_err = 0;

int num_sysinb_calls = 0;

int(sys_inb_cnt)(port_t port, unsigned int *byte) {
  num_sysinb_calls++;
  return sys_inb(port, byte);
}

int(kbd_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID);
}

int(kbd_unsubscribe_int)(void) {
  return sys_irqrmpolicy(&hookID);
}

int(kbd_read_status)(uint8_t *stByte) {

  uint32_t temp = 0;

  if (sys_inb_cnt(STAT_REG, &temp) != OK)
    return -1;

  *stByte = (uint8_t) temp;

  return 0;
}

uint8_t(get_scancode)(void) {
  return (uint8_t) scancode;
}

int(is_ih_err)(void) {
  return ih_err;
}

void(kbc_ih)(void) {

  uint8_t statusByte = 0;

  if (kbd_read_status(&statusByte) != OK)
    ih_err = -1;
  else {

    // if the output buffer has available data
    if (statusByte & OBF_FULL) {

      if (sys_inb_cnt(OUT_BUFFER, &scancode) != OK)
        ih_err = -3;

      if ((statusByte & (PAR_ERR | TO_ERR)) != OK)
        ih_err = -2;
      else
        ih_err = 0;
    }
  }
}

int(kbc_write_reg)(port_t port, uint8_t byte) {

  uint8_t statusByte;
  unsigned int numTrys = 5; // number of times the program trys to read the port before giving up

  kbd_read_status(&statusByte);

   for(unsigned int i = 0; i < numTrys; i++){

    if ((statusByte & IBF_FULL) == 0) {

      return sys_outb(port, byte);
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  return -1;
}


int(kbc_read_reg)(port_t port, uint8_t *byte) {

  uint8_t statusByte = 0;
  uint32_t tmp = 0; // temporary 32 bit int to use in sys_inb
  unsigned int numTrys = 5; // number of times the program trys to read the port before giving up

  for(unsigned int i = 0; i < numTrys; i++){
    kbd_read_status(&statusByte);

    if (statusByte & OBF_FULL) {

      if (sys_inb_cnt(port, &tmp))
        return -1;

      if ((statusByte & (PAR_ERR | TO_ERR)) != 0)
        return -2;

      *byte = (uint8_t) tmp;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  return -3;
}

bool(isValidCMD)(uint8_t cmd) {

  switch (cmd) {
    case READ_CMD_BT:
      return true;
    case WRITE_CMD_BT:
      return true;
    case KBC_CHECK:
      return true;
    case KBD_IF_CHECK:
      return true;
    case KBD_IF_EN:
      return true;
    case KBD_IF_DIS:
      return true;
    case MS_WRITE_BYTE:
      return true;
    case MS_IF_CHECK:
      return true;
    case MS_DIS:
      return true;
    case MS_EN:
      return true;

    default:
      return false;
  }
}

uint8_t(kbc_command)(uint8_t cmd, uint8_t arg, int *err) {

  uint8_t result = 0;
  
  // check if the given command is valid
  if (!isValidCMD(cmd)) {

    *err = -1;
    return 0x00;
  }

  // validWrite stores the validity of the register-writing operations
  int validWrite = kbc_write_reg(KBC_CMD_REG, cmd); // write the command to the appropriate register

  if (validWrite != OK) {
    *err = -2;
    return 0x00;
  }
  // if the command is the write command byte command, also write the argument in the appropriate register
  if ( (cmd == WRITE_CMD_BT) || (cmd == MS_WRITE_BYTE)) {

    validWrite = kbc_write_reg(KBC_CMD_ARG, arg);
    if (validWrite != OK) {
      *err = -3;
      return 0x00;
    }
  }

  // if the command yields a result, read it
  if (!((cmd == WRITE_CMD_BT) || (cmd == KBD_IF_EN) || (cmd == KBD_IF_DIS) )) {

    int validRead = kbc_read_reg(OUT_BUFFER, &result);
    if (validRead != OK) {

      *err = -4;
      return 0x00;
    }
  }

  return result;
}
