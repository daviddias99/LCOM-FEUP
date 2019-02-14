#ifndef UTIL_LAB4_H
#define UTIL_LAB4_H



/** @defgroup util-lab4 util-lab4
 * @{
 *
 * Utility functions used in lab4 for working with the mouse
 */ 



/**
 * @brief States of the state machine used to detect relevant events for mouse gesture detection
 */
typedef enum { ALL_BTNS_REL = 0,  /**< @brief State where no button is pressed */
               LEFT_BTN_PR,       /**< @brief State where only the left button is pressed */
               RIGHT_BTN_PR,      /**< @brief State where only the right button is pressed */
               OTHER_BTN_COMB     /**< @brief State where a different combination of buttons is pressed */
               } States;



/**
 * @brief Processes the bytes given from the mouse in order to assemble a mouse packet and prints it when all the bytes of a packet have been read.
 *
 * @param pp        pointer to a packet struct which holds the information for a packet
 * @param byteNum   pointer to a byte that indicates which byte of the packet the current byte represents
 * @param byte      current byte to be a part of the packet
 * @param packetCnt total number of packets processed by the program at the time
 */
void ms_process_packet(struct packet *pp, uint8_t *byteNum, uint8_t byte, uint32_t * packetCnt);

/**
 * @brief Detects the mouse event that occurred given a mouse packet
 *
 * @param pp   pointer to a packet struct which holds the information for a packet
 * @return     pointer to a mouse_ev struct which holds the information about the event that occurred
 */
struct mouse_ev * ms_detect_event(struct packet *pp);




#endif
