#include <lcom/lcf.h>

#include "logical_and_stm.h"

static State_machine stm = {
  .state = INIT,
  .st_actions = { &st_init, &st_l_line, &st_vertex, &st_r_line },
  .x_len = 0,
  .tol = 0,
  .x_dev = 0 };



void check_logical_and(struct mouse_ev *ms_evt, bool *final_st, uint8_t x_len, uint8_t tolerance) {

  // change the x_len and tolerance values to the correct ones 
  stm.x_len = x_len;
  stm.tol = tolerance;
  
  // execute the state transition function
  stm.st_actions[stm.state](ms_evt);

  // raise a flag if the end state was reached
  if (stm.state == END)
    *final_st = true;
}

void st_init(struct mouse_ev *ms_evt) {

  // reset the accumulated x deviation
  stm.x_dev = 0;

  switch (ms_evt->type) {

    case LB_PRESSED:
      stm.state = L_LINE;
      break;
    default:
      stm.state = INIT;
  }
}

void st_l_line(struct mouse_ev *ms_evt) {


  switch (ms_evt->type) {

    case LB_RELEASED:

      // check if the accumulated x deviation minimum was met
      if (stm.x_dev < stm.x_len) {
        stm.state = INIT;
      }
      else {
        stm.state = VERTEX;
      }
      break;

    case MOUSE_MOV:

      // check if the given moves were in the right direction taking in account the tolerance for wrong moves
      if ((ms_evt->delta_x + stm.tol < 0) || (ms_evt->delta_y + stm.tol < 0)) {
        stm.state = INIT;
      }

      // check if the absolute value of the slope of the movement is larger or equal than 1. If delta_x = 0, and y != 0 slope is technically larger than one (infinite slope).

      else if(!( (ms_evt->delta_x == 0) && (ms_evt->delta_y != 0)) && (abs(get_slope(ms_evt->delta_y, ms_evt->delta_x)) < 1)) {
        stm.state = INIT;
      }

      // update the accumulated x deviation
      else {
        stm.x_dev += ms_evt->delta_x;
      }
      break;

    default:
      stm.state = INIT;
  }
}

void st_vertex(struct mouse_ev *ms_evt) {


  stm.x_dev = 0;

  switch (ms_evt->type) {
    case RB_PRESSED:
      stm.state = R_LINE;
      break;

    case LB_PRESSED:
      stm.state = L_LINE;
      break;

    case MOUSE_MOV:

       // check if a given move might be "accidental" taking in account the tolerance
      if (abs(ms_evt->delta_x) > stm.tol || abs(ms_evt->delta_y) > stm.tol)
        stm.state = INIT;
      else 
        stm.state = VERTEX;
      break;
      
    default:
      stm.state = INIT;
  }
}

void st_r_line(struct mouse_ev *ms_evt) {


   switch (ms_evt->type) {


    case RB_RELEASED:

      // check if the accumulated x deviation minimum was met
      if (stm.x_dev < stm.x_len) {
        stm.state = INIT;
      }
      else {
        stm.state = END;
      }
      break;

    case MOUSE_MOV:

      // check if the given moves were in the right direction taking in account the tolerance for wrong moves
      if ((ms_evt->delta_x + stm.tol < 0) || (ms_evt->delta_y - stm.tol > 0)) {
        stm.state = INIT;
      }

      // check if the absolute value of the slope of the movement is larger or equal than 1. If delta_x = 0, and y != 0 slope is technically larger than one (infinite slope).
      else if(!( (ms_evt->delta_x == 0) && (ms_evt->delta_y != 0)) && (abs(get_slope(ms_evt->delta_y, ms_evt->delta_x)) < 1)) {
        stm.state = INIT;
      }

      // update the accumulated x deviation
      else {
        stm.x_dev += ms_evt->delta_x;
      }
      break;

    default:
      stm.state = INIT;
  }
}


int16_t get_slope(int16_t dy, int16_t dx) {

  return dy / dx;
}
