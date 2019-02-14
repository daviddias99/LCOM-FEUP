#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <lcom/lcf.h>
#include <lcom/read_xpm.h>
#include "videocard.h"
#include "dl_math.h"
#include "Enemy.h"
#include "AnimatedSprite.h"

/** @defgroup Projectile Projectile
 * @{
 * Functions to create and manipulate projectiles. A projectile is sent by the player in order to destroy an enemy. It only destroys an enemy it colides if it as the letter that created the sprite (with exception of projectiles created bny powerups), so it must track the letter that spawned it. It also functions in a different manner of other sprite-variants because it's speed is, usually, changed every update as it tracks an enemy.
 */

/** @name Projectile */
/**@{
 *
 * @brief Struct that holds the information about an projectile. An projectile is a derived concept of a AnimatedSprite and, as a result, a Sprite. The Projectile tracks not only the same information as a animated sprite asweel as the information about the enemy he is tracking, the letter that caused it and it's speed modifier.
 * 
 */
typedef struct Projectile {

  AnimatedSprite* an_sprite;             /**< animated sprite associated with the projectile */
  Enemy* tracking_enemy;                 /**< enemy associated with the projectile */
  double speed_modifier;                 /**< variable that controls the module of the projectile's speed */
  char letter;                           /**< letter of the enemy that the projectile is tracking */
  bool affects_all;                      /**< true if the projectile kills the enemy independently of the letter */

} Projectile;

/*
 * @brief Allocates memory for a new projectile and initializes it at the given position. The starting velocity is calculated as described in "projectile_update_speed".
 * 
 * @param xpm                     pointer to a xpm that the projectile will use
 * @param position                starting position of the projectile
 * @param enemy                   enemy that the sprite will track
 * @param speed_modifier          variable that controls the module of the projectile's speed
 * @param letter                  letter of the enemy that the projectile is tracking
 * @return Projectile*            pointer to a new Projectile
 */
//Projectile* projectile_create(xpm_string_t* xpm, Vector position, Enemy* enemy, double speed_modifier, char letter);

/**
 * @brief Allocates memory for a new projectile and initializes it at the given position. The starting velocity is calculated as described in "projectile_update_speed".Uses the pixmap given as a parameter. The affects_all parameter is initialized with false.
 * 
 * @param pixmap                  array of xpm_image_t that the projectile will use
 * @param position                starting position of the projectile
 * @param enemy                   enemy that the sprite will track
 * @param speed_modifier          variable that controls the module of the projectile's speed
 * @param letter                  letter of the enemy that the projectile is tracking
 * @param xpm_count               number of pixmaps in the array
 * @param time_between            game ticks between frames of the sprite animation
 * @return Projectile*            pointer to a new Projectile
 */
Projectile* projectile_create_img(xpm_image_t* pixmap, Vector position, Enemy* enemy, double speed_modifier, char letter,int xpm_count, int time_between);

/**
 * @brief Allocates memory for a new projectile and initializes it at the given position. This projectile kills any enemy it its, it's velocity is "constant"
 * 
 * @param pixmap                  array of xpm_image_t that the projectile will use
 * @param position                starting position of the projectile
 * @param speed                   the projectile's velocity
 * @param speed_modifier          ariable that controls the module of the projectile's speed
 * @param xpm_count               number of pixmaps in the array
 * @param time_between            game ticks between frames of the sprite animation
 * @return Projectile*            pointer to a new Projectile
 */
Projectile* projectile_affects_all_create(xpm_image_t* pixmap, Vector position, Vector speed, double speed_modifier, int xpm_count, int time_between);


/**
 * @brief Returns the affects_all field of a projectile
 * 
 * @param proj                   pointer to the projectile
 * @return true                  if the projectile kills any enemy
 * @return false                 if the projectile only kills enemies with the same letter as it  
 */
bool projectile_affects_all(Projectile* proj);


/**
 * @brief Draw's the projectile on the scene buffer
 * 
 * @param proj            pointer to the projectile to draw
 * @return int            zero if sucessful, non-zero otherwise
 */
int projectile_draw(Projectile* proj);

/**
 * @brief Update thr projectile's speed. A projectile's always tracks the enemy it is following. So it's speed always points to the "tracking_enemy". If the projectile isn't tracking any enemy, it's speed won't be updated. The actual speed of the projectile is controlled by it's "speed_modifier".
 * 
 * @param proj               pointer to the projectile whose speed will change
 * @return int               zero if sucessful, non-zero otherwise
 */
int projectile_update_speed(Projectile* proj);

/**
 * @brief Add the projectile's speed to the projectile's position and draw it. Then update the projectiles speed
 * 
 * @param proj              pointer to the projectile to update
 * @return int              zero if sucessful, non-zero otherwise
 */
int projectile_update(Projectile* proj);

/**
 * @brief Checks if the projectile will colide with the enemy at their next positions
 * 
 * @param proj              pointer to the projectile
 * @param enemy             pointer to the enemy
 * @return                  true if they colide, false if not                  
 */
bool proj_check_colisions(Projectile* proj, Enemy* enemy);

/**
 * @brief Check if the given projectile is out of the screen's bounds
 * 
 * @param proj               pointer to the projectile to check
 * @return                   true if the projecitle is out of bounds, false if not    
 */
bool proj_check_out_of_bounds(Projectile* proj);

/**
 * @brief Destroys the projectile and the animated sprite associated to hit
 * 
 * @param proj         pointer to the projectile to destroy
 * @return int         zero if sucessful, non-zero otherwise
 */
int proj_destroy(Projectile* proj);
/**@} */
#endif
