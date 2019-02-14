#ifndef GAME_ST_MACHINE_H
#define GAME_ST_MACHINE_H

#include <lcom/lcf.h>

#include "game.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "serialport.h"

/** @defgroup GameStateMachine GameStateMachine
 * @{
 * Functions to control the state of the game by using a state machine
 */

/**
 * @brief GameStateMachine struct
 */
struct GameStateMachine;
typedef struct GameStateMachine GameStateMachine_t;

/**
 * @brief GameStateMachine possible states
 */
typedef enum game_state { MAIN_MENU = 0,
                          GAME_PLAY,
                          SEC_GAME_PLAY,
                          PAUSED,
                          GAMEOVER,
                          NEW_HIGHSCORE,
                          HIGHSCORES,
                          END } game_state;

/**
 * @brief Game State Machine possible events
 */
typedef enum event { FRAME_TIME_PASSED = 0,
                     ESC_KEY_RELEASED,
                     KEYBOARD_PRESSED,
                     MOUSE_MOVED,
                     MOUSE_LB_PRESSED,
                     LEFT_P_UP,
                     RIGHT_P_UP,
                     ALARM_RING,
                     SEC_PASSED,
                     JOINED_2P,
                     LEFT_2P,
                     LEFT_1P,
                     START_2P,
                     NEW_CHAR_2P,
                     SEC_GAME_ADD_CHAR,
                     SEC_GAME_REM_CHAR } event;

/**
 * @brief Returns a pointer a new instance of the GameStateMachine "class"
 * 
 * @return GameStateMachine*  Pointer to the new GameStateMachine "object"
 */
GameStateMachine_t *gsm_new();

/**
 * @brief Frees the allocated space for a GameStateMachine instance
 * 
 * @param gsm   Pointer to the GameStateMachine "object" we wish to delete
 */
void gsm_del(GameStateMachine_t *gsm);

/**
 * @brief Get the current state of a GameStateMachine object
 *
 * @param   gsm         GameStateMachine objet whose current_state we wish to know
 * @return  game_state  Current state of the GameStateMachine
 */
game_state gsm_get_current_state(GameStateMachine_t *gsm);

/**
 * @brief Updates a GameStateMachine "object" with and event passed as argument
 * 
 * @param gsm   GameStateMachine to be updated
 * @param evt   Event which we want to update de state machine with
 */
void gsm_update_game_state(GameStateMachine_t *gsm, event evt);

/**@} */
#endif
