#ifndef ENEMY_H
#define ENEMY_H

#include <lcom/lcf.h>
#include <lcom/read_xpm.h>
#include "Sprite.h"
#include "wordWriter.h"


/** @defgroup Enemy Enemy
 * @{
 * Functions to manipulate enemies. An enemy is a key part of a game. They move through the screeen and colide a lot not only with each other but with projectiles sent by the player. 
 */

/** @name Type */
/**@{
 *
 * @brief This enum represents the various typed that an enemy can have
 * 
 */
enum Type{ normal       ///<  Normal enemy
          ,pup          ///< Enemy that drops a powerup
          ,frozen       ///< Enemy that is "frozen"
          };   


/** @name Textbox */
/**@{
 *
 * @brief Struct that holds the information about a word
 * 
 */
typedef struct Textbox {

  char* word;               /**< string in the text box */
  int wordHeigth_px;        /**< string height in pixels */
  int wordLength_px;        /**< string width in pixels */

} Textbox;

/** @name Enemy */
/**@{
 *
 * @brief Struct that holds the information about an Enemy. An enemy is a derived concept of a Sprite. It holds the same information that a Sprite does but also stores a Texbox, has a type and stores the number of shot's it must take to die.
 * 
 */
typedef struct Enemy{

  Sprite* sprite;               /**< sprite associated with the enemy */
  Textbox text;                 /**< text written on the enemy */
  enum Type type;               /**< id's if the enemy drops a powerup ir not or if it is frozen */
  int shots_to_die;             /**< number of shots to kill the enemy */
} Enemy;


/**
 * @brief Allocates memory for a new enemy and initializes it at *the given position and with (0,0)starting speed and the given string. The type is defaulted as "normal" and the number of shots to die is 1.
 * 
 * @param xpm               pointer to a xpm that the enemy will use
 * @param pos               starting position of the enemy
 * @param letter            string of the word to be drawn
 * @return Enemy*           new enemy
 */
Enemy * enemy_create(xpm_string_t* xpm, Vector pos, char* letter);

/**
 * @brief Allocates memory for a new enemy and initializes it at *the given position and with (0,0)starting speed and the given string. Uses the pixmap given as a parameter. The type is defaulted as "normal" and the number of shots to die is 1.
 * 
 * @param pixmap            xpm_image_t that the enemy will use
 * @param pos               starting position of the enemy
 * @param letter            string of the word to be drawn
 * @return Enemy*           new enemy
 */
Enemy * enemy_create_img(xpm_image_t pixmap, Vector pos, char* letter);

/**
 * @brief Allocates memory for a new enemy and initializes it at *the given position and with (0,0)starting speed and the given string. Uses the pixmap given as a parameter. The type is set to pup and the number of shots to die is given as an argument.
 * 
 * @param pixmap            xpm_image_t that the enemy will use
 * @param pos               starting position of the enemy
 * @param letter            string of the word to be drawn
 * @param num_shots_to_die  number of projectile hits needed to kill the enemy
 * @return Enemy* 
 */
Enemy* enemy_create_pup(xpm_image_t pixmap, Vector pos, char* letter, int num_shots_to_die);

/**
 * @brief Change the given enemie's speed to the given speed
 * 
 * @param enemy             pointer to the enemy to change the speed
 * @param vel               vector representing the new speed(xspeed,yspeed)
 * @return int              zero if sucessful, non-zero otherwise
 */
int enemy_change_vel(Enemy* enemy, Vector vel);

/**
 * @brief Change the given enemie's position to the given position
 * 
 * @param enemy             pointer to the enemy to change the position
 * @param new_pos           vector representing the new position(x,y)
 * @return int              zero if sucessful, non-zero otherwise
 */
int enemy_change_pos(Enemy* enemy, Vector new_pos);

/**
 * @brief Add the enemie's speed to the enemie's position and draw it and the word
 * 
 * @param enemy             pointer to the enemy to update
 * @param letterTyped       UNUSED
 * @return int              zero if sucessful, non-zero otherwise
 */
int enemy_update(Enemy* enemy, bool letterTyped);

/**
 * @brief Draw's the enemy on the scene buffer(does not draw the word)
 * 
 * @param enemy            pointer to the enemy to draw
 * @return int             zero if sucessful, non-zero otherwise
 */
int enemy_draw(Enemy* enemy);

/**
 * @brief Checks if enemyA will colide with enemyB at their next positions
 * 
 * @param enemyA            pointer to the first enemy
 * @param enemyB            pointer to the second enemy
 * @return                  true of both enemies will colide, false if not                  
 */
bool enemy_check_colisions(Enemy * enemyA, Enemy* enemyB);

/**
 * @brief Check if the given enemy is out of the screen's bounds
 * 
 * @param enemy            pointer to the enemy to check
 * @return                 true if the enemy is out of bounds, false if not    
 */
bool enemy_check_out_of_bounds(Enemy* enemy);

/**
 * @brief Check if the given enemy if crossing the screen's bounds.
 * These bounds are given by a four-bit mask that signals which bounds are solid(if 1). The mask is the following: LeftBound|UpperBound|RightBound|LowerBound
 * 
 * @param enemy           pointer to the enemy to check
 * @param boundMask       four bit mask, bit set to 1 means solid
 * @return                true if any bound crossed, false otherwise 
 */
bool enemy_check_bound_crossing(Enemy* enemy, uint8_t boundMask);

/**
 * @brief Frees the dynamically alocated memory for the enemy, it's sprite and string
 * 
 * @param enemy           pointer to the enemy to destroy
 * @return int            zero if sucessful, non-zero otherwise
 */
int enemy_destroy(Enemy* enemy);

/**@} */
#endif

