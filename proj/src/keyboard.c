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
/**
 * @brief Auxiliary global variable that keeps the status of caps lock
 */
static bool caps = false;
/**
 * @brief Auxiliary global variable set to true if left shift is pressed
 */
static bool lshift = false;
/**
 * @brief Auxiliary global variable set to true if righ shift is pressed
 */
static bool rshift = false;

/** @name kb_key */
/**@{
 *
 * @brief Struct that holds the chars correspondent to a key, whether caps is on or off
 */
typedef struct kb_key {
  char upper; /**< Char when caps is on */
  char lower; /**< Char when caps is off */
} kb_key_t;
/**@} */

/**
 * @brief Array holding the keys corresponding to a keyboard makecode
 */
kb_key_t keys[] = {{.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = '!', .lower = '1'},
                   {.upper = '"', .lower = '2'},
                   {.upper = '#', .lower = '3'},
                   {.upper = '$', .lower = '4'},
                   {.upper = '%', .lower = '5'},
                   {.upper = '&', .lower = '6'},
                   {.upper = '/', .lower = '7'},
                   {.upper = '(', .lower = '8'},
                   {.upper = ')', .lower = '9'},
                   {.upper = '=', .lower = '0'},
                   {.upper = '?', .lower = '\''},
                   {.upper = 0, .lower = 0},
                   {.upper = '\b', .lower = '\b'},
                   {.upper = '\t', .lower = '\t'},
                   {.upper = 'Q', .lower = 'q'},
                   {.upper = 'W', .lower = 'w'},
                   {.upper = 'E', .lower = 'e'},
                   {.upper = 'R', .lower = 'r'},
                   {.upper = 'T', .lower = 't'},
                   {.upper = 'Y', .lower = 'y'},
                   {.upper = 'U', .lower = 'u'},
                   {.upper = 'I', .lower = 'i'},
                   {.upper = 'O', .lower = 'o'},
                   {.upper = 'P', .lower = 'p'},
                   {.upper = '*', .lower = '+'},
                   {.upper = 0, .lower = 0},
                   {.upper = '\n', .lower = '\n'},
                   {.upper = 0, .lower = 0},
                   {.upper = 'A', .lower = 'a'},
                   {.upper = 'S', .lower = 's'},
                   {.upper = 'D', .lower = 'd'},
                   {.upper = 'F', .lower = 'f'},
                   {.upper = 'G', .lower = 'g'},
                   {.upper = 'H', .lower = 'h'},
                   {.upper = 'J', .lower = 'j'},
                   {.upper = 'K', .lower = 'k'},
                   {.upper = 'L', .lower = 'l'},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = 'Z', .lower = 'z'},
                   {.upper = 'X', .lower = 'x'},
                   {.upper = 'C', .lower = 'c'},
                   {.upper = 'V', .lower = 'v'},
                   {.upper = 'B', .lower = 'b'},
                   {.upper = 'N', .lower = 'n'},
                   {.upper = 'M', .lower = 'm'},
                   {.upper = ';', .lower = ','},
                   {.upper = ':', .lower = '.'},
                   {.upper = '_', .lower = '-'},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = 0, .lower = 0},
                   {.upper = ' ', .lower = ' '}};

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

  kbd_read_status(&statusByte);

  while (1) {

    if ((statusByte & IBF_FULL) == 0) {

      return sys_outb(port, byte);
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }
}

int(kbc_read_reg)(port_t port, uint8_t *byte) {

  uint8_t statusByte = 0;
  uint32_t tmp = 0; // temporary 32 bit int to use in sys_inb

  while (1) {
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
  if (cmd == WRITE_CMD_BT) {

    validWrite = kbc_write_reg(KBC_CMD_ARG, arg);
    if (validWrite != OK) {
      *err = -3;
      return 0x00;
    }
  }

  // if the command yields a result, read it
  if (!((cmd == WRITE_CMD_BT) || (cmd == KBD_IF_EN) || (cmd == KBD_IF_DIS))) {

    int validRead = kbc_read_reg(OUT_BUFFER, &result);
    if (validRead != OK) {

      *err = -4;
      return 0x00;
    }
  }

  return result;
}

bool get_caps_status() {

  return caps;
}

char makecode_to_char(uint8_t sc_byte) {

  if (sc_byte == CAPS_LCK_MAKECODE) {
    if (caps)
      caps = false;
    else
      caps = true;
  }
  else if (sc_byte == L_SHIFT_MAKECODE) {
    lshift = true;
  }
  else if (sc_byte == L_SHIFT_BREAKCODE) {
    lshift = false;
  }
  else if (sc_byte == R_SHIFT_MAKECODE) {
    rshift = true;
  }
  else if (sc_byte == R_SHIFT_BREAKCODE) {
    rshift = false;
  }
  else if (ih_err || sc_byte >= NUM_MAPPED_KEYS) {
    return 0;
  }
  else {
    if (caps) {
      if (lshift || rshift) {
        
        return keys[sc_byte].lower;
      }
      else {
        
        return keys[sc_byte].upper;
      }
    }
    else {
      if (lshift || rshift)
        return keys[sc_byte].upper;
      else
        return keys[sc_byte].lower;
    }
  }

  return 0;
}

int kbc_clear_buffer() {

  uint8_t statusByte = 0;
  uint32_t byte = 0;
  int err = 0;

  if (kbd_read_status(&statusByte) != OK)
    err = -1;
  else {
    // if the output buffer has available data
    if (statusByte & OBF_FULL) {

      if (sys_inb_cnt(OUT_BUFFER, &byte) != OK)
        err = -3;
      if ((statusByte & (PAR_ERR | TO_ERR)) != OK)
        err = -2;
      else
        err = 0;
    }
  }

  return err;
}
