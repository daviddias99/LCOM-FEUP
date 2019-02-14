#include "Cursor.h"

/** @name Cursor */
/**@{
 *
 * @brief Much like a Sprite, the cursor holds the information about its position and its graphics, the difference being that the cursor position is updated in a way that it tracks the mouse movement
 * 
 */
struct Cursor {

  uint16_t xpos; /**< x position of the cursor */
  uint16_t ypos; /**< y position of the cursor */

  xpm_image_t xpm_info; /**< graphics of the cursor */
};
/**@} */


Cursor_t *cursor_new(xpm_string_t *image) {

  Cursor_t *n_cursor = malloc(sizeof(Cursor_t));

  if (n_cursor == NULL)
    return NULL;

  // set the mouse position to the center of the screen
  n_cursor->xpos = getHRes() / 2;
  n_cursor->ypos = getVRes() / 2;

  // load the xpm image corresponding to the icon of the cursor
  xpm_load(image, XPM_1_5_5_5, &n_cursor->xpm_info);

  return n_cursor;
}

void cursor_del(Cursor_t *cursor) {

  free(cursor->xpm_info.bytes);
  free(cursor);

  cursor = NULL;
}

void cursor_draw(Cursor_t *cursor) {

  draw_xpm(&cursor->xpm_info, cursor->xpos, cursor->ypos);
}

void cursor_update_pos(Cursor_t *cursor, int8_t dx, int8_t dy) {

  if ((int16_t)(cursor->xpos + dx) < 0) // check if the cursor goes out of bounds to the left
    cursor->xpos = 0;
  else if (cursor->xpos + dx > getHRes()) // check if the cursor goes out of bounds to the right
    cursor->xpos = getHRes();
  else
    cursor->xpos += dx; // if the cursor doesn't go out of bounds, then update the x position

  if ((int16_t)(cursor->ypos - dy) < 0) // check if the cursor goes out of bounds to the top
    cursor->ypos = 0;
  else if (cursor->ypos - dy > getVRes()) // check if the cursor goes out of bounds to the bottom
    cursor->ypos = getVRes();
  else
    cursor->ypos -= dy; // if the cursor doesn't go out of bounds, then update the y position
}

uint16_t cursor_get_x_pos(Cursor_t *cursor) {

  return cursor->xpos;
}

uint16_t cursor_get_y_pos(Cursor_t *cursor) {

  return cursor->ypos;
}
