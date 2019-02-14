#ifndef wordWriter
#define wordWriter

#include "debug.h"
#include "videocard.h"
#include "font_xpm.h"

/** @defgroup WordWriter WordWriter
 * @{
 * Functions to load and write words on the video memory
 */

#define CHAR_HEIGHT 26                /**< @brief Height, in pixels, of the font used.  */
#define LETTER_BASE_ASCII 33          /**< @brief First valid character of the ASCII Table (!) */

/**
 * @brief Loads all the xpms's of the selected alphabet into the chars array(as xpm_image_t). The xpm's must be arranged as described in the font_xpm header file
 * 
 * @return int          zero if sucessful, non-zero otherwise
 */
int loadAlphabet();

/**
 * @brief Free memory alocated to the char's pixmaps.
 * 
 */
void deleteAlphabet();

/**
 * @brief Draw's in the scene buffer the word given as a argument spacing the letter by 'spacing' pixels. An space char is two timer the spacing. WARNING: must execute a valid loadAlphabet() before.
 * 
 * @param word          string of the word(s) to be drawn
 * @param x             x position of the top left corner of the word
 * @param y             y position of the top left corner of the word
 * @param spacing       number of pixels between letters
 * @return int          zero if sucessful, non-zero otherwise
 */
int drawWord(char *word, uint16_t x, uint16_t y, uint16_t spacing);

//int swapColor(xpm_image_t* image, uint16_t colorA, uint16_t colorB);

/**
 * @brief Get the letter height of the loaded alphabet. WARNING: must execute a valid loadAlphabet() before.
 * 
 * @return int          height in pixels of the alphabet
 */
int getAlphabetHeigth();

/**
 * @brief Width in pixels of the word(s) to be drawn
 * 
 * @param word          string of the word(s) to be drawn
 * @param spacing       number of pixels between letters
 * @return int          zero if sucessful, non-zero otherwise
 */
int getWordWidth(char* word, uint16_t spacing);



/**@} */
#endif
