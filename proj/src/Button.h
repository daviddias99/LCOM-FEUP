#ifndef BUTTON_H
#define BUTTON_H

#include "videocard.h"
#include <lcom/lcf.h>

/** @defgroup Button Button
 * @{
 * Functions to manipulate buttons
 */

struct Button;
typedef struct Button Button_t;

/**
 * @brief   Creates a new button with the the xpm passed as argument as icon with the upper left corner in the "x" and "y" position passed as arguments
 * 
 * @param   image     Button icon in xpm format
 * @param   x         Position in the "x" direction of the upper left corner of the button
 * @param   y         Position in the "y" direction of the upper left corner of the button
 * @return  Button*   Pointer to the newly created button
 */
Button_t *button_new(xpm_string_t *image, uint16_t x, uint16_t y);

/**
 * @brief   Frees the memory where the button passed as an argument to the fuction is stored
 * 
 * @param   button   Button stored in the memory we want to free
 * @return  void
 */
void button_del(Button_t *button);

/**
 * @brief   Draws the button passed as argument in the video ram buffer
 * 
 * @param   button  Button we want to draw
 * @return  void
 */
void button_draw(Button_t *button);

/**
 * @brief   Checks if the button contains the position "x" and "y" passed as argument 
 * 
 * @param   button  Button we wish to check if it contains the position 
 * @param   x       Position in the "x" direction 
 * @param   y       Position in the "y" direction 
 * @return  bool    True if the position belongs to the button, false otherwise
 */
bool button_pressed_in_range(Button_t *button, uint16_t x, uint16_t y);

/**
 * @brief   Changes the position of a previously created Button "object"
 * 
 * @param   button  Button whose position we wish to change
 * @param   x       New button position in the "x" direction
 * @param   y       New button position in the "y" direction
 */
void button_change_pos(Button_t *button, uint16_t x, uint16_t y);

/**
 * @brief   Get the xpm image of the button passed as argument
 * 
 * @param   button        Button whose image we want to get
 * @return  xpm_image_t*  Pointer to the button xpm image
 */
xpm_image_t* button_get_xpm_img(Button_t *button); 
/**@} */
#endif
