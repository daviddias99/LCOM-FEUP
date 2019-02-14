#ifndef VIDEOCARD_UTIL_H
#define VIDEOCARD_UTIL_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "i8042.h"
#include "keyboard.h"

/**
 * @brief Get the least significant word of a double word
 * 
 * @param val           double word
 * @param lsw           pointer of a variable where to store the word           
 * @return              0 if sucessfull, non-0 otherwise 
 */
int util_get_LSW(uint32_t val, uint32_t *lsw); 


/**
 * @brief Get the most significant word of a double word
 * 
 * @param val           double word
 * @param msw           pointer of a variable where to store the word           
 * @return              0 if sucessfull, non-0 otherwise 
 */
int util_get_MSW(uint32_t val, uint32_t *msw); 

/**
 * @brief convert a far pointer to a "normal" pointer
 * 
 * @param baseAdd       pointer to the base address of the memory (returned by lm_init)
 * @param farPtr        far pointer to convert
 * @return              converted far pointer 
 */
uint32_t FarPtr_to_Ptr(void* baseAdd, uint32_t farPtr );

/**
 * @brief subscribe keyboard interrupts and wait for the ESC key to be released
 * 
 * @return              0 if sucessfull, non-0 otherwise 
 */
int waitForEscape();
/**
 * @brief generate a mask containing ones or zeros depending on the value of the 
 * mask_of_ones bool
 * 
 * @param numBits         number of bits that should be masked
 * @param mask_of_ones    true if the mask is of ones, false if it is of zeros
 * @return                mask 
 */
uint32_t makeMask(uint8_t numBits, bool mask_of_ones);

#endif
