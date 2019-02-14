#include "Button.h"

/** @name Button */
/**@{
 *
 * @brief The button represents a pushable button, holding information about its top left corner's position and itss xpm.
 * 
 */
struct Button {

  uint16_t xpos; /**< x position of the button */
  uint16_t ypos; /**< y position of the button */

  xpm_image_t xpm_info; /**< graphics of the button */
};
/**@} */

Button_t *button_new(xpm_string_t *image, uint16_t x, uint16_t y) {

  Button_t *n_button = malloc(sizeof(Button_t));

  if (n_button == NULL)
    return NULL;

  n_button->xpos = x;
  n_button->ypos = y;

  // load the xpm image corresponding to the button icon
  xpm_load(image, XPM_1_5_5_5, &n_button->xpm_info);

  return n_button;
}

void button_del(Button_t *button) {

  free(button->xpm_info.bytes);
  free(button);

  button = NULL;
}

void button_draw(Button_t *button) {

  draw_xpm(&button->xpm_info, button->xpos, button->ypos);
}

bool button_pressed_in_range(Button_t *button, uint16_t x, uint16_t y) {

  return x >= button->xpos &&
         x <= button->xpos + button->xpm_info.width &&
         y >= button->ypos &&
         y <= button->ypos + button->xpm_info.height;
}

void button_change_pos(Button_t *button, uint16_t x, uint16_t y) {

  button->xpos = x;
  button->ypos = y;
}

xpm_image_t* button_get_xpm_img(Button_t *button) {

  return &button->xpm_info;
}
