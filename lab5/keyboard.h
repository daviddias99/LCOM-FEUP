#ifndef KEYBOARD_H
#define KEYBOARD_H

/** @defgroup heyboard keyboard
 * @{
 *
 * Functions for using keyboard
 */

#ifdef LAB3
/**
 * @brief Integer type used to count the number of sys_inb calls (only used if LAB3 is defined)
 */
extern int num_sysinb_calls;
/**
 * @brief Function that has the same functionality has sys_inb but also increments a global variable (only used if LAB3 is defined)
 *
 * @param port I/O port address we want to read
 * @param byte Address of memory where we want to write the information read from the port
 * @return Return 0 upon success and non-zero otherwise
 */
int sys_inb_cnt(port_t port, unsigned int *byte);
#else
#  define sys_inb_cnt(p, q) sys_inb(p, q)
#endif

/**
 * @brief Subscribes and enables KBC interrupts while unsubscribing MINIX's KBC interrupt handlers
 *
 * @param bit_no Address of memory to be initialized with the bit number to be set in the mask  returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_unsubscribe_int)();

/**
 * @brief Read the status byte of the status register of the KBC
 *
 * @param stByte Address of memory where we want to store the read status byte
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_read_status)(uint8_t *stByte);

/**
 * @brief Returns the last scancode read (must use the kbc_ih function and check if it was successful first so that the returned value is an actual scancode)
 * 
 * @return Last scancode read
 */
uint8_t(get_scancode)();

/**
 * @brief Checks if the kbc_ih function ran successfully
 * 
 * @return Return 0 if kbc_ih was succesful and non-zero otherwise
 */
int(is_ih_err)();

/**
 * @brief Checks if the given KBC command is valid (more valid commands will be added for lab4)
 * 
 * @return Return true if it's valid, false otherwise
 */
bool(isValidCMD)(uint8_t cmd);

/**
 * @brief Issue the given command to the KBC
 * 
 * @param cmd Command to be given to the KBC
 * @param arg Argument of the command
 * @param err Pointer to a variable where the validity of the operation will be stored
 * @return Resulting byte yielded by the command. Care must be taken with this function, because even if the given command doesn't yield a result, the function will still output a byte (0x00)
 */
uint8_t(kbc_command)(uint8_t cmd, uint8_t arg, int *err);

/**
 * @brief Write a byte to a given port
 * 
 * @param port I/O port where the byte will be written
 * @param byte Byte to be written at the port
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_write_reg)(port_t port, uint8_t byte);

/**
 * @brief Read a byte from a given port
 * 
 * @param port I/O port from where the byte will be read
 * @param byte Pointer to a variable where the byte will be stored
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_read_reg)(port_t port, uint8_t *byte);

#endif
