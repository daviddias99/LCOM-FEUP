#ifndef MOUSE_H
#define MOUSE_H

#include "keyboard.h"
#include "i8042.h"
#include "GameStateMachine.h"

/** @defgroup Mouse Mouse
 * @{
 *
 * Functions for using the mouse
 */


/**
 * @brief Subscribes and enables KBC's mouse interrupts while unsubscribing MINIX's KBC interrupt handlers
 *
 * @param bit_no Address of memory to be initialized with the bit number to be set in the mask  returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int ms_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC's mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int ms_unsubscribe_int();


/**
 * Returns the last byte read by the interrupt handler (must use the mouse_ih function and check if it was successful first so that the returned value is a desired byte)
 * 
 * @return Last scancode read
 */
uint8_t ms_get_ih_byte(void);

/**
 * @brief Checks if the mouse_ih function ran successfully
 * 
 * @return Return 0 if mouse_ih was succesful and non-zero otherwise
 */
int ms_is_ih_err(void);

/**
 * @brief Disable the mouse interrupt notifications
 * 
 * @return Return 0 if the process was succesful and non-zero otherwise
 */
int ms_disable_int_not(void);

/**
 * @brief Enable the mouse interrupt notifications
 * 
 * @return Return 0 if the process was succesful and non-zero otherwise
 */
int ms_enable_int_not(void);

/**
 * @brief Writes a command to the mouse using the 0xD4 (Write byte to mouse) kbc command
 * checking afterwards for acknowledgements
 *
 * @param command mouse command to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int ms_command(uint8_t command);

/**
 * @brief Processes the bytes given from the mouse in order to assemble a mouse packet
 */
void ms_process_packet();

/**
 * @brief Checks if the current packet being read is complete
 * 
 * @return true if the packet is complete, false otherwise
 */
bool is_packet_complete();

/**
 * @brief Returns a pointer to the last packet read the mouse
 * 
 * @return struct packet* Last packet read
 */
struct packet* get_packet();


#define MIN_LENGTH 50 /**< @brief Min length of the vertical gesture */
#define TOL 30 /**< @brief Tolerance  of the vertical gesture (length of movemente allowed in undesired directions) */


/**
 * @brief States of the state machine used to detect relevant events for mouse gesture detection
 */
typedef enum { ALL_BTNS_REL = 0,  /**< @brief State where no button is pressed */
               LEFT_BTN_PR,       /**< @brief State where only the left button is pressed */
               RIGHT_BTN_PR,      /**< @brief State where only the right button is pressed */
               OTHER_BTN_COMB     /**< @brief State where a different combination of buttons is pressed */
               } States;

/**
 * @brief Processes a mouse packet in order to detect which mouse event occurred. (event types are: LB_PRESSED,LB_RELEASED,RB_PRESSED,RB_RELEASED,BUTTON_EV and MOUSE_MOV)
 * 
 * @param pp                      pointer to a mouse packet struct
 * @return struct mouse_ev*       struct containing the mous event
 */
struct mouse_ev* ms_detect_event(struct packet *pp);

/**
 * @brief State machine that detects, through mouse events, if a vertical line was drawn while holding either the Right or left mouse button.
 * 
 * @param ms_evt 
 * @return int 
 */
int check_vert_line_gest(struct mouse_ev* ms_evt);
/**@} */
#endif
