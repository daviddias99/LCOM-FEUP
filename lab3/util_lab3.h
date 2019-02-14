#ifndef UTIL_LCOM_H
#define UTIL_LCOM_H

#include <lcom/lcf.h>

#include <stdint.h>
#include "i8042.h"

/** @defgroup util-lab3 util-lab3
 * @{
 *
 * Utility functions used in lab3 for working with the keyboard
 */


/**
 * @brief Processes a scanbyte, checking if it is the escaped sequence of a two byte scancode,if it is make or breakcode and prints it if it is a complete scancode
 *
 * @param byte Array where the bytes of the scancode are stored
 * @param escapedSeq True if the previous scanbyte was the escaped sequence of a two byte scancode
 * @param scanbyte Current scanbyte to be processed
 */ 
void(kbd_process_scancode)(uint8_t byte[], bool *escapedSeq, uint8_t scanbyte);

#endif
