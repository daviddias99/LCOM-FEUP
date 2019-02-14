#ifndef CURSOR_H
#define CURSOR_H

#include <lcom/lcf.h>

#include "videocard.h"

/** @defgroup Cursor Cursor
 * @{
 * Functions to display and manipulate a mouse cursor
 */
struct Cursor;
typedef struct Cursor Cursor_t;

/**
 * @brief   Creates a new mouse cursor
 * 
 * @param   image    Cursor icon in xpm format
 * @return  Cursor*  Pointer to the newly created cursor 
 */
Cursor_t* cursor_new(xpm_string_t *image);

/**
 * @brief   Frees the memory where the cursor passed as an argument to the fuction is stored
 * 
 * @param   cursor  Cursor stored in the memory we want to free
 * @return  void
 */
void cursor_del(Cursor_t* cursor);

/**
 * @brief   Draws the cursor passed as argument in the video ram buffer with the upper left corner of the cursor icon sprite in the position stored in the Cursor struct
 * 
 * @param   cursor  Cursor we want to draw
 * @return  void
 */
void cursor_draw(Cursor_t* cursor);

/**
 * @brief   Updates the current position of the cursor by the x and y deviation passed as arguments
 * 
 * @param   cursor  Cursor whose position we wish to update 
 * @param   dx      Deviation in the "x" direction
 * @param   dy      Deviation in the "y" direction
 * @return  void
 */
void cursor_update_pos(Cursor_t* cursor, int8_t dx, int8_t dy);

/**
 * @brief   Get the position in the "x" direction of the cursor passed as argument to the function
 * 
 * @param   cursor    Cursor whose "x" position we want to know
 * @return  uint16_t  Position in the "x" direction of the cursor
 */
uint16_t cursor_get_x_pos(Cursor_t* cursor);

/**
 * @brief   Get the position in the "y" direction of the cursor passed as argument to the function
 * 
 * @param   cursor    Cursor whose "y" position we want to know
 * @return  uint16_t  Position in the "y" direction of the cursor
 */
uint16_t cursor_get_y_pos(Cursor_t* cursor);
/**@} */
#endif
