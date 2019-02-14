#ifndef LOGICAL_AND_STM_H_
#define LOGICAL_AND_STM_H_

/** @defgroup logical_and_stm logical_and_stm
 * @{
 *
 * State machine to detect logical and mouse gestures
 */



/**
 * @brief States of the state machine used to check for logical and shaped mouse gesture
 */
typedef enum { 
              INIT = 0,  /**< @brief Initial state of the state machine */
              L_LINE,    /**< @brief State that corresponds to the drawing of the left line */
              VERTEX,    /**< @brief State that represents the vertex of the logical and */
              R_LINE,    /**< @brief State that corresponds to the drawing of the right line */
              END        /**< @brief Final state of the state machine */
              } state_t;


/**
 * @brief Struct containing the necessary attributes to use the state machine that checks for logical and shaped gestures that also has pointers to the functions that process each state 
 */
typedef struct state_machine {

  state_t state;    /**< @brief Current state */
  uint16_t x_dev;   /**< @brief Accumulated x deviation for the line that is being drawn */

  uint8_t x_len;    /**< @brief Minimum x deviation necessary for each line */
  uint8_t tol;      /**< @brief Maximum deviation in the wrong direction tolerated for move event */
 
  void (*st_actions[4])(struct mouse_ev *ms_evt); /**< @brief Array of pointers to the functions that process each state but the final state */

} State_machine;



/*
 * @brief Calls the right functions for the current state of the state machine
 * 
 * @param ms_evt    Pointer to the last mouse event
 * @param final_st  Address to a bool that is set to true if the final state is reached
 * @param x_len     Minimum x deviation necessary for each line
 * @param tolerance Maximum deviation in the wrong direction tolerated for move event
 */
void check_logical_and(struct mouse_ev *ms_evt, bool *final_st, uint8_t x_len, uint8_t tolerance);



/*
 * @brief Processes the mouse event with the state machine in the INIT state, changing the state to the desired one
 * 
 * @param ms_evt  Pointer to a mouse event
 */
void st_init(struct mouse_ev *ms_evt);

/*
 * @brief Processes the mouse event with the state machine in the L_LINE state, changing the state to the desired one
 * 
 * @param ms_evt  Pointer to a mouse event
 */
void st_l_line(struct mouse_ev *ms_evt);

/*
 * @brief Processes the mouse event with the state machine in the VERTEX state, changing the state to the desired one
 * 
 * @param ms_evt  Pointer to a mouse event
 */
void st_vertex(struct mouse_ev *ms_evt);

/*
 * @brief Processes the mouse event with the state machine in the R_LINE state, changing the state to the desired one
 * 
 * @param ms_evt  Pointer to a mouse event
 */
void st_r_line(struct mouse_ev *ms_evt);


/*
 * @brief Computes the slope given dy and dx
 * 
 * @param dy  Deviation in the y direction
 * @param dx  Deviation in the x direction
 */ 
int16_t get_slope(int16_t dy, int16_t dx);

#endif
