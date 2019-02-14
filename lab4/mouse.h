#ifndef MOUSE_H
#define MOUSE_H

/** @defgroup mouse mouse
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

#endif
