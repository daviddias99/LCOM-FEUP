#ifndef GAME_MACRO_H
#define GAME_MACRO_H

/** @defgroup GameMACRO GameMACRO
 * @{
 *
 * Symbolic constants used to setup the game.
 */

#define FRAME_RATE                        60                /**< @brief frames per second that are generated */

// Array sizes

#define MAX_ENEMIES                       50                /**< @brief  Maximum size of the enemy array*/
#define MAX_PROJECTILES                   50                /**< @brief  Maximum size of the projectile array*/
#define MAX_EXPLOSIONS                    50                /**< @brief  Maximum size of the explosion's array */


// Variable values

#define PUP_DUR                           8                 /**< @brief  Duration in seconds of the powerups*/
#define DEFAULT_PLAYER_LIFE               6                 /**< @brief  The player's starting life*/
#define PUP_ENEMY_LIFE                    5                 /**< @brief  Number of shots needed to destroy a powerup-dropping enemy*/
#define PROJ_SPEED_MODIFIER               10                /**< @brief  Measures the travel velocity of the projectiles */
#define PROJ_TIME_BETWEEN_FRAMES          5                 /**< @brief  Number of game ticks between the frames of the projectile's animation*/
#define PROJ_NUM_FRAMES                   4                 /**< @brief  Number of frames in an projetile's animation cycle*/
#define EXP_NUM_FRAMES                    6                 /**< @brief  Number of frames in an explosion's animation cycle */
#define DAMAGING_HEIGHT                   700               /**< @brief  Damage at which the enemies explode and damage the player*/

// Difficulty settings

#define LVL1_LOWER_XSPD_BOUND             -1                /**< @brief  Lower xspeed bound for level 1 enemies*/
#define LVL1_UPPER_XSPD_BOUND             1                 /**< @brief  Upper xspeed bound for level 1 enemies*/
#define LVL1_LOWER_YSPD_BOUND             1                 /**< @brief  Lower yspeed bound for level 1 enemies*/
#define LVL1_UPPER_YSPD_BOUND             1                 /**< @brief  Upper yspeed bound for level 1 enemies*/

#define LVL2_LOWER_XSPD_BOUND             -2                /**< @brief  Lower xspeed bound for level 2 enemies*/
#define LVL2_UPPER_XSPD_BOUND             2                 /**< @brief  Upper xspeed bound for level 2 enemies*/
#define LVL2_LOWER_YSPD_BOUND             1                 /**< @brief  Lower yspeed bound for level 2 enemies*/
#define LVL2_UPPER_YSPD_BOUND              2                /**< @brief  Upper yspeed bound for level 2 enemies*/

#define LVL3_LOWER_XSPD_BOUND             -3                /**< @brief  Lower xspeed bound for level 3 enemies*/
#define LVL3_UPPER_XSPD_BOUND             3                 /**< @brief  Upper xspeed bound for level 3 enemies*/
#define LVL3_LOWER_YSPD_BOUND             1                 /**< @brief  Lower yspeed bound for level 3 enemies*/
#define LVL3_UPPER_YSPD_BOUND             3                 /**< @brief  Upper yspeed bound for level 3 enemies*/

#define LVL4_LOWER_XSPD_BOUND             -4                /**< @brief  Lower xspeed bound for level 4 enemies*/
#define LVL4_UPPER_XSPD_BOUND             4                 /**< @brief  Upper xspeed bound for level 4 enemies*/
#define LVL4_LOWER_YSPD_BOUND             2                 /**< @brief  Lower yspeed bound for level 4 enemies*/
#define LVL4_UPPER_YSPD_BOUND             4                 /**< @brief  Upper yspeed bound for level 4 enemies*/

#define INITIAL_LEVEL                     1                 /**< @brief  Starting level of the game */
#define SPAWN_RATE_DELTA                  15                /**< @brief  Change in spawn rate between levels(ticks) */
#define LEVEL_UP_TIME_DELTA               1200              /**< @brief  Time between level up (ticks)   */
#define INITIAL_SPAWN_RATE                120               /**< @brief  Initial time between enemy spawns(ticks)*/

// Sprite Screen coordinates
 
#define LIFE_COUNTER_X                    10                /**< @brief  x position of the top left corner of the life bar*/
#define LIFE_COUNTER_Y                    5                 /**< @brief  y position of the top left corner of the life bar*/
#define GAME_OVER_USERNAME_X              470               /**< @brief  x position of the top left corner of username in game over screen*/
#define GAME_OVER_USERNAME_Y              410               /**< @brief  y position of the top left corner of username in game over screen*/
#define POP_UP_X_POS                      186               /**< @brief  x position of the top left corner of the pop up menu*/
#define POP_UP_Y_POS                      112               /**< @brief  y position of the top left corner of the pop up menu*/
#define STAT_BAR_WORD_SPACING             2                 /**< @brief  spacing between words in stat bar */
#define UI_WORD_SPACING                   3                 /**< @brief  spacing between words in UI */
#define BUTTON_SPACING                    20                /**< @brief  spacing between buttons */


// button related symbolic constants

#define NUMBER_BUTTONS                    10                /**< @brief Size of the button array */


#define PLAY_BUTTON_X                     287                /**< @brief  x position of the top left corner of the play button */
#define PLAY_BUTTON_Y                     277                /**< @brief  y position of the top left corner of the play button */
#define HELP_BUTTON_X                     287                /**< @brief  x position of the top left corner of the help button */
#define HELP_BUTTON_Y                     373                /**< @brief  y position of the top left corner of the help button */
#define HIGHSCORES_BUTTON_X               287                /**< @brief  x position of the top left corner of the highscores button */
#define HIGHSCORES_BUTTON_Y               469                /**< @brief  y position of the top left corner of the highscores button */
#define EXIT_BUTTON_X                     287                /**< @brief  x position of the top left corner of the exit button */
#define EXIT_BUTTON_Y                     565                /**< @brief  y position of the top left corner of the exit button */
#define RESUME_BUTTON_X                   287                /**< @brief  x position of the top left corner of the resume button */
#define RESUME_BUTTON_Y                   308                /**< @brief  y position of the top left corner of the resume button */
#define QUIT_BUTTON_X                     287                /**< @brief  x position of the top left corner of the quit button */
#define QUIT_BUTTON_Y                     404                /**< @brief  y position of the top left corner of the quit button */
#define TRY_AGAIN_BUTTON_X                300                /**< @brief  x position of the top left corner of the try again button */
#define TRY_AGAIN_BUTTON_Y                420                /**< @brief  y position of the top left corner of the try again button */
#define MAIN_MENU_BUTTON_X                530                /**< @brief  x position of the top left corner of the main menu button */
#define MAIN_MENU_BUTTON_Y                420                /**< @brief  y position of the top left corner of the main menu button */
#define CANCEL_BUTTON_X                   530                /**< @brief  x position of the top left corner of the cancel button */
#define CANCEL_BUTTON_Y                   470                /**< @brief  y position of the top left corner of the cancel button */
#define SAVE_BUTTON_X                     300                /**< @brief  x position of the top left corner of the save button */
#define SAVE_BUTTON_Y                     470                /**< @brief  y position of the top left corner of the save button */


#endif
