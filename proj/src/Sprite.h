#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <lcom/lcf.h>
#include <lcom/read_xpm.h>
#include "videocard.h"
#include "dl_math.h"
#include "debug.h"

/** @defgroup Sprite Sprite
 * @{
 * Functions to create and manipulate sprites. A sprite is a concept that represents an image that is drawn in a video buffer. The sprite contains information about it's position, speed(change of position as the time passses) and the actual graphics that are drawn on the screen. The Sprite struct serves as a base for other structures used such as AnimatedSprite, Projectile and Enemy. There exist functions that change the graphics if the sprite, update it's speed, position, and check it's conditions on the drawing enviorment such as colisions and out of bounds situations.
 */

/** @name Sprite */
/**@{
 *
 * @brief Sprite that holds the necessary information about a sprite.
 * 
 */
typedef struct Sprite {

  Vector pos;                    /**< current sprite position */
  Vector speed;                  /**< current speeds in the x and y direction */
  xpm_image_t graphics;          /**< the sprites xpm_image, contains width height and the pixmap */

} Sprite;

/**
 * @brief Allocates memory for a new sprite and initializes it at the given position and with (0,0)starting speed. Loads the xpm given as a parameter.
 * 
 * @param xpm               pointer to a xpm that the sprite will use
 * @param position          starting position of the sprite
 * @return Sprite*          new sprite
 */
Sprite * sprite_create(xpm_string_t* xpm, Vector position);

/**
 * @brief Allocates memory for a new sprite and initializes it at the given position and with (0,0)starting speed. Uses the pixmap given as a parameter.
 * 
 * @param pixmap            xpm_image_t that the sprite will use
 * @param position          starting position of the sprite
 * @return Sprite*          new sprite
 */
Sprite * sprite_create_img(xpm_image_t pixmap, Vector position);

/**
 * @brief Change the given sprites speed to the given speed
 * 
 * @param sprite            pointer to the sprite to change the speed
 * @param new_speed         vector representing the new speed(xspeed,yspeed)
 * @return int              zero if sucessful, non-zero otherwise
 */
int sprite_change_vel(Sprite* sprite, Vector new_speed);

/**
 * @brief Change the given sprites position to the given position
 * 
 * @param sprite            pointer to the sprite to change the position
 * @param new_pos           vector representing the new position(x,y)
 * @return int              zero if sucessful, non-zero otherwise
 */
int sprite_change_pos(Sprite* sprite, Vector new_pos);

/**
 * @brief Add the sprite's speed to the sprite's position and draw it
 * 
 * @param sprite            pointer to the sprite to update
 * @return int              zero if sucessful, non-zero otherwise
 */
int sprite_update(Sprite* sprite);

/**
 * @brief Draw's the sprite on the scene buffer
 * 
 * @param sprite            pointer to the sprite to draw
 * @return int 
 */
int sprite_draw(Sprite* sprite);

/**
 * @brief Swaps the pixmap associated with the given sprite
 * 
 * @param sprite             sprite to change graphics
 * @param xpm                new graphics
 * @return int               zero if sucessful, non-zero otherwiuse
 */
int sprite_change_graphics(Sprite* sprite,xpm_image_t xpm);

/**
 * @brief Checks if spriteA will colide with spriteB at their next positions
 * 
 * @param spriteA          pointer to the first sprite
 * @param spriteB          pointer to the second sprite
 * @return                 true of both sprites will colide, false if not                  
 */
bool sprite_checkColision(Sprite* spriteA, Sprite* spriteB);

/**
 * @brief Check if the given sprite is out of the screen's bounds
 * 
 * @param sprite           pointer to the sprite to check
 * @return                 true if the sprite is out of bounds, false if not    
 */
bool sprite_check_out_of_bounds(Sprite* sprite);

/**
 * @brief Check if the given sprite if crossing the screen's bounds.
 * These bounds are given by a four-bit mask that signals which bounds are solid(if 1). The mask is the following: LeftBound|UpperBound|RightBound|LowerBound
 * 
 * @param sprite          pointer to the sprite to check
 * @param boundMask       four bit mask, bit set to 1 means solid
 * @return                true if any bound crossed, false otherwise 
 */
bool sprite_check_bound_crossing(Sprite* sprite, uint8_t boundMask);

/**
 * @brief Frees the dynamically alocated memory for the sprite
 * 
 * @param sprite          pointer to the sprite to destroy
 * @return int            zero if sucessful, non-zero otherwise
 */
int sprite_destroy(Sprite* sprite);


/**@} */
#endif
