#ifndef ANIM_SPRITE_H
#define ANIM_SPRITE_H

#include "Sprite.h"

/** @defgroup AnimatedSprite AnimatedSprite
 * @{
 *    Functions to create and manipulate animated sprites. An animated sprite is a concept that is derived from a normal sprite. The difference between the two objects are that the Animated Sprite will have graphics that change as the type occurs. As such, it contains, in addition to the information contained in a sprite object information about the number of frames that are presented in an animation cycle, the time between frames and if the frames are presented for only a cycle. The sprite holds an array containing all the pixmaps he uses.
 */

/** @name AnimatedSprite */
/**@{
 *
 * @brief Struct that holds the information regarding needed to implment an animated sprite. This struct is used as an Object.
 * 
 */
typedef struct AnimatedSprite {

  Sprite* sprite_obj;           /**< sprite object associated with the animated sprite */
  xpm_image_t *xpm_db;          /**< "array" containing the frames of the sprite's animation */
  int xpm_count;                /**< number of frames in the array */
  int xpm_current;              /**< current frame */
  int time_between_frames;      /**< game ticks between frames */
  int time_until_next_frame;    /**< number of ticks until next frame */
  bool destroy_after_cycle;     /**< true if the sprite is destroyed after one cycle */

} AnimatedSprite;

/**
 * @brief Allocates memory for a new animated sprite and initializes it at the given position. if dead_after_one is true, the sprite will be destroyed after one animation cycle.
 * 
 * @param pixmap                   array of xpm_image_t that the projectile will use
 * @param position                 starting position of the sprite
 * @param xpm_count                number of frames in the array
 * @param time_between             game ticks between frames
 * @param dead_after_one           true if the sprite is destroyed after one cycle
 * @return AnimatedSprite*         pointer to the new Animated Sprite
 */
AnimatedSprite *animated_sprite_create(xpm_image_t *pixmap, Vector position,int xpm_count, int time_between, bool dead_after_one);

/**
 * @brief Destroy the alocated memory for the animated sprite and for the associated sprite object
 * 
 * @param an_sprite               pointer to the animated sprite to destroy
 * @return int                    zero if sucessful, non-zero otherwise
 */
int animated_sprite_destroy(AnimatedSprite* an_sprite);

/**
 * @brief Draw the animated sprite
 * 
 * @param an_sprite 
 * @return int 
 */
int animated_sprite_draw(AnimatedSprite *an_sprite);

/**
 * @brief update the position of the animated sprite and the displayed graphics
 * 
 * @param an_sprite             pointer to the sprite to update
 * @return int                  zero if sucessful, non-zero otherwise
 */
int animated_sprite_update(AnimatedSprite *an_sprite);

/**
 * @brief Change the velocity of the animated sprite
 * 
 * @param an_sprite             pointer to the sprite to update
 * @param new_speed             vector representing the new speed
 * @return int                  zero if sucessful, non-zero otherwise
 */
int animated_sprite_change_vel(AnimatedSprite* an_sprite, Vector new_speed);

/**
 * @brief Change the position of the animated sprite
 * 
 * @param an_sprite             pointer to the sprite to update
 * @param new_pos               vector representing the new position
 * @return int                  zero if sucessful, non-zero otherwise
 */
int animated_sprite_change_pos(AnimatedSprite* an_sprite, Vector new_pos);

/**
 * @brief Check colisions between two animated sprites. (see sprite_check_colisions)
 * 
 * @param spriteA                pointer to first sprite
 * @param spriteB                pointer to second sprite
 * @return true                  if the sprites will colide
 * @return false                 if the sprited won't colide
 */
bool animated_sprite_checkColision(AnimatedSprite* spriteA, AnimatedSprite* spriteB);

/**
 * @brief Check if the animated sprite is out of bounds
 * 
 * @param an_sprite             pointer to the animated sprite
 * @return true                 if the sprite is out of bounds
 * @return false                if the sprite is in bounds
 */
bool animated_sprite_check_out_of_bounds(AnimatedSprite* an_sprite);

/**
 * @brief Check if the animated sprite is crossing the solid bounds. (See sprite_check_bound_crossing).
 * 
 * @param an_sprite             pointer to the animated sprite       
 * @param boundMask             four bit mask, bit set to 1 means solid
 * @return true                 if a solid bound was crossed
 * @return false                if no solid bound was crossed
 */
bool animated_sprite_check_bound_crossing(AnimatedSprite* an_sprite, uint8_t boundMask);

/**@} */
#endif
