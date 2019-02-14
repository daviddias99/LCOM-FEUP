#include <lcom/lcf.h>

#include "debug.h"
#include "mouse.h"

/**
 * @brief Bit we wish to set to one in the mask returned by an interrupt
 */
static int hookID = 12;
/**
 * @brief Last byte read in the mouse interrupt handler
 */
static uint32_t scanbyte = 0;
/**
 * @brief Flag set to something other than 0 if there is an error in the interrupt handler
 */
static int ih_err = 0;

static uint8_t byteNum = 0; // variable used to store the number of the last byte read from the mouse
static struct packet pp;
static bool complete_packet;

int ms_subscribe_int(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(MS_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID);
}

int ms_unsubscribe_int(void) {

  return sys_irqrmpolicy(&hookID);
}

int ms_disable_int_not(void) {

  return sys_irqdisable(&hookID);
}

int ms_enable_int_not(void) {

  return sys_irqenable(&hookID);
}

void(mouse_ih)(void) {

  uint8_t statusByte = 0;

  if (kbd_read_status(&statusByte) != OK)
    ih_err = -1;
  else {

    // if the output buffer has available data
    if (statusByte & OBF_FULL) {

      if (sys_inb_cnt(OUT_BUFFER, &scanbyte) != OK)
        ih_err = -3;

      if ((statusByte & (PAR_ERR | TO_ERR)) != OK)
        ih_err = -2;
      else
        ih_err = 0;
    }
  }
}

uint8_t ms_get_ih_byte(void) {
  return (uint8_t) scanbyte;
}

int ms_is_ih_err(void) {
  return ih_err;
}

int ms_command(uint8_t command) {

  int errByte;              // store the validity state of kbc command
  uint8_t msAck = 0;        // variable used to store acknowledgements
  unsigned int numTrys = 2; // number of times a command is tried before giving up

  for (unsigned int i = 0; i < numTrys; i++) {

    // write the command to the mouse
    msAck = kbc_command(MS_WRITE_BYTE, command, &errByte);

    // check acknowledgement
    if (msAck == MS_BT_ACK)
      return 0;
    else if (errByte != OK)
      return -1;
  }

  return -2;
}

void ms_process_packet() {

  if (byteNum == 0 && !(scanbyte & BIT(3)))
    return;

  pp.bytes[byteNum] = scanbyte;

  if (byteNum == 2) {

    complete_packet = true;

    pp.lb = pp.bytes[0] & L_BUTTON;
    pp.rb = pp.bytes[0] & R_BUTTON;
    pp.mb = pp.bytes[0] & M_BUTTON;
    pp.x_ov = pp.bytes[0] & X_OVF;
    pp.y_ov = pp.bytes[0] & Y_OVF;

    pp.delta_x = pp.bytes[1];
    pp.delta_y = pp.bytes[2];

    if (pp.bytes[0] & X_SIGN) {
      pp.delta_x |= 0xFF00;
    }
    if (pp.bytes[0] & Y_SIGN)
      pp.delta_y |= 0xFF00;
  }
  else {
    complete_packet = false;
  }

  if (byteNum == 2)
    byteNum = 0;
  else
    byteNum++;
}

bool is_packet_complete() {
  return complete_packet;
}

struct packet *get_packet() {
  return &pp;
}

struct mouse_ev *ms_detect_event(struct packet *pp) {

  static States state = ALL_BTNS_REL;
  struct mouse_ev *ms_evt = (struct mouse_ev *) malloc(sizeof(struct mouse_ev));

  switch (state) {
    case ALL_BTNS_REL:

      if (!(pp->rb || pp->lb || pp->mb)) {
        ms_evt->type = MOUSE_MOV;
        ms_evt->delta_x = pp->delta_x;
        ms_evt->delta_y = pp->delta_y;
        return ms_evt;
      }
      else if (pp->lb && !(pp->rb || pp->mb)) {
        state = LEFT_BTN_PR;
        ms_evt->type = LB_PRESSED;
        return ms_evt;
      }
      else if (pp->rb && !(pp->lb || pp->mb)) {
        state = RIGHT_BTN_PR;
        ms_evt->type = RB_PRESSED;
        return ms_evt;
      }
      else {
        state = OTHER_BTN_COMB;
        ms_evt->type = BUTTON_EV;
        return ms_evt;
      }

    case LEFT_BTN_PR:

      if (!(pp->rb || pp->lb || pp->mb)) {
        state = ALL_BTNS_REL;
        ms_evt->type = LB_RELEASED;
        return ms_evt;
      }
      else if (pp->lb && !(pp->rb || pp->mb)) {
        ms_evt->type = MOUSE_MOV;
        ms_evt->delta_x = pp->delta_x;
        ms_evt->delta_y = pp->delta_y;
        return ms_evt;
      }
      else {
        state = OTHER_BTN_COMB;
        ms_evt->type = BUTTON_EV;
        return ms_evt;
      }

    case RIGHT_BTN_PR:

      if (!(pp->rb || pp->lb || pp->mb)) {
        state = ALL_BTNS_REL;
        ms_evt->type = RB_RELEASED;
        return ms_evt;
      }
      else if (pp->rb && !(pp->lb || pp->mb)) {
        ms_evt->type = MOUSE_MOV;
        ms_evt->delta_x = pp->delta_x;
        ms_evt->delta_y = pp->delta_y;
        return ms_evt;
      }
      else {
        state = OTHER_BTN_COMB;
        ms_evt->type = BUTTON_EV;
        return ms_evt;
      }

    case OTHER_BTN_COMB:

      if (!(pp->rb || pp->lb || pp->mb))
        state = ALL_BTNS_REL;
      else if (pp->rb && !(pp->lb || pp->mb))
        state = RIGHT_BTN_PR;
      else if (pp->lb && !(pp->rb || pp->mb))
        state = LEFT_BTN_PR;

      ms_evt->type = BUTTON_EV;
      return ms_evt;
  }
}

typedef enum { INIT,
               LB_DOWN,
               RB_DOWN } pup_sm_states;



int check_vert_line_gest(struct mouse_ev *ms_evt) {
 
  static pup_sm_states state = INIT;
  static int y = 0;

  switch (state) {

    case INIT:

      switch (ms_evt->type) {
        case LB_PRESSED:
          y = 0;
          state = LB_DOWN;
          break;
        case RB_PRESSED:
          y = 0;
          state = RB_DOWN;
          break;
        default:
          break;
      }

      break;

    case LB_DOWN:

      switch (ms_evt->type) {

        case MOUSE_MOV:

          if (abs(ms_evt->delta_x) > TOL || ms_evt->delta_y > TOL)
            state = INIT;
          else {
            y -= ms_evt->delta_y;
          }

          break;
        case LB_RELEASED:

          if (y > MIN_LENGTH) {
            
            return 1;
          }
          else
            state = INIT;

          break;

        default:

          state = INIT;
          break;
      }
      break;

    case RB_DOWN:

      switch (ms_evt->type) {

        case MOUSE_MOV:

          if (abs(ms_evt->delta_x) > TOL || ms_evt->delta_y > TOL)
            state = INIT;
          else
            y -= ms_evt->delta_y;

          break;
        case RB_RELEASED:

          if (y > MIN_LENGTH)
            return 2;
          else
            state = INIT;

          break;

        default:

          state = INIT;
          break;
      }
      break;
  }

  return 0;
}
