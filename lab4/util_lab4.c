#include <lcom/lcf.h>

#include "util_lab4.h"
#include "i8042.h"

void ms_process_packet(struct packet *pp, uint8_t *byteNum, uint8_t byte, uint32_t * packetCnt) {

  if (*byteNum == 0 && !(byte & BIT(3)))
    return;
      
  pp->bytes[*byteNum] = byte;

  if (*byteNum == 2) {

    pp->lb = pp->bytes[0] & L_BUTTON;
    pp->rb = pp->bytes[0] & R_BUTTON;
    pp->mb = pp->bytes[0] & M_BUTTON; 
    pp->x_ov = pp->bytes[0] & X_OVF;
    pp->y_ov = pp->bytes[0] & Y_OVF;

    pp->delta_x = pp->bytes[1];
    pp->delta_y = pp->bytes[2];

    if (pp->bytes[0] & X_SIGN) {
      pp->delta_x |= 0xFF00;
    }
    if (pp->bytes[0] & Y_SIGN)
      pp->delta_y |= 0xFF00;


    mouse_print_packet(pp);
    (*packetCnt)++;
  }

  if (*byteNum == 2) 
    *byteNum = 0;
  else 
    ++(*byteNum);
}

struct mouse_ev * ms_detect_event(struct packet *pp) {

  static States state = ALL_BTNS_REL;
  struct mouse_ev * ms_evt = (struct mouse_ev *) malloc(sizeof(struct mouse_ev));


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
