#ifndef GAME_H
#define GAME_H

#include "AnimatedSprite.h"
#include "Button.h"
#include "Cursor.h"
#include "Enemy.h"
#include "Projectile.h"
#include "dl_math.h"
#include "game_macro.h"
#include "highscores.h"
#include "linked_list.h"
#include "serialport.h"
#include "videocard.h"
#include "xpm_db.h"

/** @defgroup Game Game
 * @{
 * Functions to manipulate the game. The game consists of an player that must shoot down descending enemies through typing the letters that they contain. To implement it we have functions that control the colisions, the spawning, the game's calculations, the sprite movement, the game's statistics and the game's assets.
 */

/**
 * @brief Names of the buttons that exist in the game
 */
typedef enum game_buttons {
  PLAY_BTN = 0,
  HELP_BTN,
  HIGHSCORES_BTN,
  EXIT_BTN,
  RESUME_BTN,
  QUIT_BTN,
  TRY_AGAIN_BTN,
  MAIN_MENU_BTN,
  CANCEL_BTN,
  SAVE_BTN
} game_buttons;
/**
 * @brief Position in the x direction of the buttons that exist in the game
 */
static const uint16_t button_x_pos[] = {
  PLAY_BUTTON_X,
  HELP_BUTTON_X,
  HIGHSCORES_BUTTON_X,
  EXIT_BUTTON_X,
  RESUME_BUTTON_X,
  QUIT_BUTTON_X,
  TRY_AGAIN_BUTTON_X,
  MAIN_MENU_BUTTON_X,
  CANCEL_BUTTON_X,
  SAVE_BUTTON_X};
/**
 * @brief Position in the y direction of the buttons that exist in the game
 */
static const uint16_t button_y_pos[] = {
  PLAY_BUTTON_Y,
  HELP_BUTTON_Y,
  HIGHSCORES_BUTTON_Y,
  EXIT_BUTTON_Y,
  RESUME_BUTTON_Y,
  QUIT_BUTTON_Y,
  TRY_AGAIN_BUTTON_Y,
  MAIN_MENU_BUTTON_Y,
  CANCEL_BUTTON_Y,
  SAVE_BUTTON_Y};
/**
 * @brief Images of the buttons of the game in xpm format
 */
static const char **button_xpm[] = {
  play_button_xpm,
  help_friend_xpm,
  highscores_button_xpm,
  exit_button_xpm,
  resume_button_xpm,
  quit_button_xpm,
  try_again_button_xpm,
  main_menu_button_xpm,
  cancel_button_xpm,
  save_button_xpm};

/** @name Bounds */
/**@{
 *
 * @brief Struct representing an integer interval.
 * 
 */
typedef struct Bounds {

  int lowerBound; /**< @brief lowerBound of the interval */
  int upperBound; /**< @brief upperBound of the interval */

} Bounds;

/** @name Game */
/**@{
 *
 * @brief Struct that holds the assets of the game.
 * 
 */
typedef struct Game {

  xpm_image_t game_background;     /**< @brief general background of the game */
  xpm_image_t paused_image;        /**< @brief image of the paused screen */
  xpm_image_t gameover_image;      /**< @brief image of the gameover screen */
  xpm_image_t new_highscore_image; /**< @brief image of the new_highscore screen */
  xpm_image_t highscores_image;    /**< @brief image of the highscores screen */
  xpm_image_t player_pixmap;       /**< @brief pixamp of the player */
  xpm_image_t enemy_pixmap;        /**< @brief pixmap of the enemy */
  xpm_image_t enemy_pup_pixmap;    /**< @brief pixmap of an enemy that drops a power up */
  xpm_image_t logo_pixmap;         /**< @brief pixmap of the game logo */
  xpm_image_t *explosions;         /**< @brief "array" of pixmaps for the explosions */
  xpm_image_t *projectiles;        /**< @brief "array" of pixmaps for the projectile animation */
  xpm_image_t *life_counter;       /**< @brief "array" of pixmaps for the life bar */
  xpm_image_t caps_on;             /**< @brief Caps on symbol */
  xpm_image_t caps_off;            /**< @brief Caps off symbol */
  xpm_image_t pup_both;            /**< @brief Both powerups active symbol */
  xpm_image_t pup_freeze;          /**< @brief Freeze powerup active symbol */
  xpm_image_t pup_split;           /**< @brief Split powerup active symbol */
  xpm_image_t pup_none;            /**< @brief None powerups active symbol */
  xpm_image_t pause_screen;        /**< @brief Game state for paused background */

  int num_explosion_frames;   /**< @brief Number of explosion animation frames */
  int num_projectiles_frames; /**< @brief Number of projectile animation frames */
  int life_total_frames;      /**< @brief Number of life bar state frames */

  Cursor_t *cursor;                  /**< @brief Cursor used in the game */
  Button_t *buttons[NUMBER_BUTTONS]; /**< @brief Array of pointers to the Button objects of the game */

  Sprite *player_sprite;

  // spawning parameters
  int current_difficulty; /**< @brief current game difficulty */
  int spawn_rate_delta;   /**< @brief change in spawning rate per level */
  int level_up_time;      /**< @brief number of ticks to level up */
  bool spawning_enabled;  /**< @brief spawning enabled flag */
  int timer_tick;         /**< @brief game ticks since game start */
  int time_sec;           /**< @brief seconds since game start */
  int spawn_rate;         /**< @brief ticks between enemy spawning */
  int default_spawn_rate; /**< @brief level 1 spawn rate */

  // player statistics
  int stat_total_typed_letters;   /**< @brief Number of typed letters */
  int stat_total_hit_letters;     /**< @brief Number of correctly hit letters*/
  int stat_total_enemies_killed;  /**< @brief Number of destroyed enemies */
  int stat_total_powerups;        /**< @brief Number of powerups gotten */
  float stat_default_player_life; /**< @brief Starting health of the player */
  float stat_player_life;         /**< @brief Current life of the player */
  int stat_cpm;
  int stat_acc;
  uint32_t stat_score;

  bool freeze;           /**< @brief True if the player has the freeze powerup */
  bool splitshot;        /**< @brief True if the player has the splitshot powerup */
  bool freeze_active;    /**< @brief True if the freeze powerup is active */
  bool splitshot_active; /**< @brief True if the splitshot powerup is active */
  int num_frozen;        /**< @brief Number of enemies that are frozen at a given moment */

  char username[USERNAME_MAX_SIZE + 1]; /**< @brief Username of the player who got a highscore */
  uint8_t usr_name_curr_index;          /**< @brief Index to where the next char must be written in the username */

  Highscore *highscores[NUMBER_HIGHSCORES]; /**< @brief Highscores of the game */

} Game;
/**@} */

extern bool helper_exists;

/**
 * @brief Powerups that the player can have at a given moment
 */
typedef enum power_up { FREEZE, ///< The freeze powerup slows enemies down
                        SPLIT,  ///< The splishot powerup fires radial projectiles from the location of the next dead enemy
                        BOTH    ///< Represents both powerups descibred above
} power_up;

/** @name SpawnSlotManager */
/**@{
 *
 * @brief Struct that stores information for handling the enemy spawning to avoid overlaping
 * 
 */
typedef struct SpawnSlotManager {

  int slot_ypos;    /**< @brief y position of the slots */
  int slot_count;   /**< @brief number of slots*/
  int slot_width;   /**< @brief width in pixels of the slots */
  int slot_current; /**< @brief current free slot */
  Enemy **slots;    /**< @brief "array" of the enemies in the slots */

} SpawnSlotManager;
/**@} */

/** @name StatBarSprites */
/**@{
 *
 * @brief Struct that holds the sprites of the stat bar
 * 
 */
typedef struct StatBarSprites {

  Sprite *caps_lock;    /**< @brief sprite of the caps lock icon */
  Sprite *powerups;     /**< @brief sprite of the powerups icon */
  Sprite *life_counter; /**< @brief sprite of the life bar*/

} StatBarSprites;
/**@} */

Game game;                    /**< @brief game struct */
SpawnSlotManager spawn_slots; /**< @brief spawn slots struct */
StatBarSprites stat_sprites;  /**< @brief stat bar sprites struct */

/**
 * @brief Game updating functions. Handles the level changing, the enemy spawning, the enemy,explosion and projectile update calls and stat bar drawing
 * 
 * @return int                zero if sucessful, non-zero otherwise
 */
int game_update();

/**
 * @brief Change the graphics of the heath bar according to the player health
 * 
 * @return int               zero if sucessful, non-zero otherwise
 */
int game_update_life_counter();

/**
 * @brief Adds a new enemy to the enemy vector, increases the enemy count
 * 
 * @param newEnemy          pointer of the new enemy
 * @return int              zero if sucessful, non-zero otherwise
 */
int game_add_enemy(Enemy *newEnemy);

/**
 * @brief Adds a new projectile to the projectile vector, increases the projectile count
 * 
 * @param newProjectile     pointer of the new projectile
 * @return int              zero if sucessful, non-zero otherwise
 */
int game_add_projectile(Projectile *newProjectile);

/**
 * @brief Adds a new explosion to the explosion array, increases the explosion count
 * 
 * @param newExplosion      pointer of the new explosion
 * @return int              zero if sucessful, non-zero otherwise
 */
int game_add_explosion(AnimatedSprite *newExplosion);

/**
 * @brief Removes an enemy from the enemy array, descreases enemy count. Destroys the enemy. Adds the enemy letter into the current letter queue, makes useless projectiles continue their path and adds a new explosion.
 * 
 * @param enemy             pointer to the enemy to remove
 * @param arrayPos          position of the enemy in the enemy array
 * @return int              zero if sucessful, non-zero otherwise
 */
int game_remove_enemy(Enemy *enemy, int arrayPos);

/**
 * @brief Removes an projectile from the projectile array, descreases projectile count. Destroys the projectile.
 * 
 * @param proj             pointer to the projectile to remove
 * @param arrayPos         position of the projectile in the projectile array
 * @return int              zero if sucessful, non-zero otherwise
 */
int game_remove_proj(Projectile *proj, int arrayPos);

/**
 * @brief Removes an explosion from the explosion array, descreases explosion count. Destroys the explosion.
 * 
 * @param exp              pointer to the explosion to remove
 * @param arrayPos         position of the explosion in the explosion array
 * @return int             zero if sucessful, non-zero otherwise
 */
int game_remove_explosion(AnimatedSprite *exp, int arrayPos);

/**
 * @brief Resolves the colision between to enemies. The xspeeds of both enemies is multiplied by -1 and the yspeed of both is set to be equal to the largest of the yspeeds.
 * 
 * @param enemyA           pointer to the first enemy
 * @param enemyB           pointer to the seconds enemy
 * @return int             zero if sucessful, non-zero otherwise
 */
int game_handle_colision(Enemy *enemyA, Enemy *enemyB);

/**
 * @brief Creates a new projectile if a letter of a alive enemy is pressed. Adds it to the projectile array.
 * 
 * @param letter           letter that was pressed in that tick('\0' if none was pressed) 
 * @return int             zero if sucessful, non-zero otherwise
 */
int game_check_new_projectiles(char letter);

/**
 * @brief Checks colisions between projectiles and enemies. Kills enemies hit and creates new projectiles in the cases of the powerups.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_col_proj_enemies();

/**
 * @brief Checks colisions between enemies and players. Kills enemies that colide, makes the player loose one life point and updates the life counter.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_col_enemies_player();

/**
 * @brief Checks colisions between enemies. Calls the handle colisions function.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_col_enemies();

/**
 * @brief Checks if the enemies hit the game's bounds(solid bounds are the right and left one). If enemies hit the bounds they bounce back in the x direction.
 *  
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_col_enemies_bounds();

/**
 * @brief Checks all of the game's colisions. Calls the respective functions of the colisions Enemy/Enemy Enemy/Player Enemy/Projectile Enemy/Bounds. Also checks for out of bounds sprites.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_colisions();

/**
 * @brief Checks if projectiles are out of bounds. Removes them if so.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_check_out_of_bounds();

/**
 * @brief Check if the next spawning slot if available.
 * 
 * @return true           the slot if available
 * @return false          the slot if unavailable
 */
bool game_check_available_slot();

/**
 * @brief Loads the letter queues of the different levels
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_load_queue();

/**
 * @brief Destroys the letter queues
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_destroy_queues();

/**
 * @brief Change the powerup-signal variables accoding to the powerup given.
 * 
 * @param up              powerup to activate
 */
void game_activate_power_up(power_up up);

/**
 * @brief Create 7 new projectiles at the given position that go in radial directions
 * 
 * @param pos             position where the projetiles will spawn
 */
void game_add_split_projectiles(Point pos);

/**
 * @brief Calculates the game score. 
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_calculate_score();

/**
 * @brief Calculates the chars per minute statistic.
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_calculate_cpm();

/**
 * @brief Calculates the player accuracy. (letters_hit/letters_typed)
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_calculate_acc();

/**
 * @brief Reset the necessary variables for a new game. Doesn't reset sprites, fonts to be efficient.
 *  
 * @return int            zero if sucessful, non-zero otherwise
 */
int game_reset();

/**
 * @brief Get the game's assets struct
 * 
 * @return Game*          pointer to the game's assets
 */
Game *getGame();

/**
 * @brief Load's the game's assets. Loads the sprites, setups the spawning slots, loads the letter queues, creates the buttons and initializes the game's variables.
 * 
 * @return int 
 */
int load_game_data();

/**
 * @brief Draws a standard menu consisting of background and cursor
 * 
 */
void draw_standard_menu();

/**
 * @brief Check's for colisions, updates the game and draw's it. (uses double buffering to avoid flickering)
 */
void draw_game_play();

/**
 * @brief Draws the screen of the secondary game
 */
void draw_sec_game_play();

/**
 * @brief Draws the menu that appears when the player gets a new highscore
 */
void draw_new_highscore_menu();

/**
 * @brief Draws the stats that appear in the screen after the game
 */
void draw_gameover_stats();

/**
 * @brief Draws the game's stat bar
 * 
 */
void draw_stat_bar();

/**
 * @brief Draws the higshcores screen
 */
void draw_highscores();

/**
 * @brief Increases the game time (in seconds) by one
 * 
 */
void game_increase_game_time();

/**
 * @brief Check if the player has lost the game
 * 
 * @return true           the player lost the game
 * @return false          the player has not lost the name
 */
bool game_player_lost();

/**
 * @brief Reset the player's username to an empty string
 * 
 */
void game_reset_username();

/**
 * @brief Gets the game's array of enemies, updating the num_enemies param with the number of enemies contained in the array
 * 
 * @param   num_enemies  Pointer to the number of enemies contained in the array
 * @return Enemy**       First position of the array of enemies
 */
Enemy **get_enemy_array(int *num_enemies);

/**
 * @brief Adds a char to the username. If the char is '\b', removes a char from the array
 * 
 * @param new_char  Char to be added to the username
 */
void gameover_set_username(char new_char);

/**
 * @brief Adds a char to the secondary game 
 * 
 * @param c   Char to be added to the secondary game
 */
void game_add_sec_game_char(char c);

/**
 * @brief Removes a char from the secondary game
 * 
 * @param c   Char to be removed from the secondary game
 */
void game_rem_sec_game_char(char c);

/**
 * @brief Resets the secondary game, by clearing the array of chars
 */
void sec_game_reset();

/**
 * @brief Frees the alocated memory from sprites, buttons, letters, etc.
 * 
 */
void game_free_data();

/**@} */
#endif
