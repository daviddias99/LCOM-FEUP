#include "GameStateMachine.h"

///////////////////////////////
// GAME STATE MACHINE STRUCT //
///////////////////////////////

/** @name GameStateMachine */
/**@{
 * @brief Struct that holds the game state machine
 */
struct GameStateMachine {

  game_state current_state; /**< Current state of the game state machine */
};
/**@} end of GameStateMachine struct*/

/////////////////////////////////////////////////
// PRIVATE FUNCTIONS OF THE GAME STATE MACHINE //
/////////////////////////////////////////////////

/**
 * @brief Changes the current state of the Game State Machine "object"
 * 
 * @param gsm         GameStateMachine whose state we want to change
 * @param new_state   State to which we wish to change the GameStateMachine
 */
static void change_game_state(GameStateMachine_t *gsm, game_state new_state);
/**
 * @brief Function used to handle the FRAME_TIME_PASSED event, that happens when it's time to draw a new frame
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void frame_time_passed_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the ESC_KEY_RELEASED event, that happens when ESC key is released
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void esc_key_released_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the KEYBOARD_PRESSED event, that happens everytime the keyboard is used
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void keyboard_pressed_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the MOUSE_MOVED event, that happens everytime there is mouse movement
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void mouse_moved_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the MOUSE_LB_PRESSED event, that happens everytime the left button of the mouse is pressed
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void mouse_lb_pressed_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the LEFT_P_UP event, that happens when a downward vertical line mouse gesture while holding the left button is detected
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void left_p_up_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the RIGHT_P_UP event, that happens when a downward vertical line mouse gesture while holding the right button happens
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void right_p_up_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the ALARM_RING event, that happens when a RTC alarm occurs
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void alarm_ring_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the SEC_PASSED event, that happens everytime a second passes
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void sec_passed_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the JOINED_2P event, that happens when a secondary player "helper" joins the main player
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void joined_2p_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the LEFT_2P event, that happens when the secondary player leaves the game
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void left_2p_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the LEFT_1P event, that happens when the main player leaves the game
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void left_1p_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the START_2P event, that happens when the secondary player has "permission" to start the game
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void start_2p_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the NEW_CHAR_2P event, that happens when the main player receives a char from the secondary player
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void new_char_2p_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the SEC_GAME_ADD_CHAR event, that happens when the secondary player receives a new game char from the main player
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void sec_game_add_char_handler(GameStateMachine_t *gsm);
/**
 * @brief Function used to handle the SEC_GAME_REM_CHAR event, that happens when the secondary player receives a a game char from the main player to be removed from the game
 * 
 * @param gsm GameStateMachine that was updated with the event
 */
static void sec_game_rem_char_handler(GameStateMachine_t *gsm);

/**
 * @brief Array of pointers to the functions that handle game state machine events
 */
static void (*game_event_handlers[])(GameStateMachine_t *) = {frame_time_passed_handler,
                                                              esc_key_released_handler,
                                                              keyboard_pressed_handler,
                                                              mouse_moved_handler,
                                                              mouse_lb_pressed_handler,
                                                              left_p_up_handler,
                                                              right_p_up_handler,
                                                              alarm_ring_handler,
                                                              sec_passed_handler,
                                                              joined_2p_handler,
                                                              left_2p_handler,
                                                              left_1p_handler,
                                                              start_2p_handler,
                                                              new_char_2p_handler,
                                                              sec_game_add_char_handler,
                                                              sec_game_rem_char_handler};

//////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF THE PUBLIC FUNCTIONS OF THE GAME STATE MACHINE //
//////////////////////////////////////////////////////////////////////

GameStateMachine_t *gsm_new() {

  GameStateMachine_t *new_gsm = malloc(sizeof(GameStateMachine_t));

  change_game_state(new_gsm, MAIN_MENU);

  return new_gsm;
}

void gsm_del(GameStateMachine_t *gsm) {

  free(gsm);
}

game_state gsm_get_current_state(GameStateMachine_t *gsm) {

  return gsm->current_state;
}

void gsm_update_game_state(GameStateMachine_t *gsm, event evt) {

  (*game_event_handlers[evt])(gsm);
}

///////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF THE PRIVATE FUNCTIONS OF THE GAME STATE MACHINE //
///////////////////////////////////////////////////////////////////////

static void change_game_state(GameStateMachine_t *gsm, game_state new_state) {

  // whenever a game ends, reset all the game content so that a new one may be started
  if (gsm->current_state == GAMEOVER || gsm->current_state == NEW_HIGHSCORE || (gsm->current_state == PAUSED && new_state != GAME_PLAY)) {
    game_reset();
  }

  // if the secondary game ends, reset the secondary game content
  if (gsm->current_state == SEC_GAME_PLAY && new_state != SEC_GAME_PLAY)
    sec_game_reset();

  // if the main player quits the game, warn the second player
  if ((gsm->current_state == GAME_PLAY && new_state != PAUSED) || (gsm->current_state == PAUSED && new_state != GAME_PLAY)) {

    if (helper_exists)
      uart_send_p1_leave();
  }

  switch (new_state) {
    case MAIN_MENU:
      // compose the menu with all its components (buttons and game background) and move it to the aux buffer
      draw_background2(game.game_background);
      draw_xpm(&game.logo_pixmap, get_hres() / 2 - game.logo_pixmap.width / 2, 30);
      button_draw(game.buttons[PLAY_BTN]);
      button_draw(game.buttons[HELP_BTN]);
      button_draw(game.buttons[HIGHSCORES_BTN]);
      button_draw(game.buttons[EXIT_BTN]);
      scene_to_aux_buffer();
      break;
    case HIGHSCORES:
      // compose the menu with all its components (buttons and game background) and move it to the aux buffer
      draw_background2(game.game_background);
      draw_xpm(&game.highscores_image, POP_UP_X_POS, POP_UP_Y_POS);
      draw_highscores();
      display_scene();
      break;
    case PAUSED:
      // compose the menu with all its components (buttons and last frame of the game) and move it to the aux buffer
      draw_xpm(&game.paused_image, POP_UP_X_POS, POP_UP_Y_POS);
      button_draw(game.buttons[RESUME_BTN]);
      button_draw(game.buttons[QUIT_BTN]);
      scene_to_aux_buffer();
      break;
    case GAMEOVER:
      // compose the menu with all its components (buttons and game background) and move it to the aux buffer
      draw_background2(game.game_background);
      draw_xpm(&game.gameover_image, POP_UP_X_POS, POP_UP_Y_POS);
      button_draw(game.buttons[MAIN_MENU_BTN]);
      button_draw(game.buttons[TRY_AGAIN_BTN]);
      draw_gameover_stats();
      scene_to_aux_buffer();
      break;
    case NEW_HIGHSCORE:
      // compose the menu with all its components (buttons and game background) and move it to the aux buffer
      draw_background2(game.game_background);
      draw_xpm(&game.new_highscore_image, POP_UP_X_POS, POP_UP_Y_POS);
      button_draw(game.buttons[SAVE_BTN]);
      button_draw(game.buttons[CANCEL_BTN]);
      draw_gameover_stats();
      scene_to_aux_buffer();
      break;
    default:
      break;
  }

  // change the current state of the state machine
  gsm->current_state = new_state;
}

static void frame_time_passed_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case MAIN_MENU:
    case PAUSED:
    case GAMEOVER:
      // draws the standard menu, wich contains the background,
      // which was "composed" and moved to the aux buffer by means of the change_game_state function,
      // and the cursor
      draw_standard_menu();
      break;
    case GAME_PLAY:
      // check if the game is over, and if so,
      // check if it is a new highscore or not and change the game state machine to the correspondent state
      if (game_player_lost()) {

        if (getGame()->stat_score > get_lowest_highscore(getGame()->highscores))
          change_game_state(gsm, NEW_HIGHSCORE);
        else
          change_game_state(gsm, GAMEOVER);

        break;
      }
      if (game.num_frozen == 0)
        game.freeze_active = false;

      draw_game_play();
      break;

    case SEC_GAME_PLAY:
      draw_sec_game_play();
      break;

    case NEW_HIGHSCORE:
      draw_new_highscore_menu();
      break;
    default:
      break;
  }
}

static void esc_key_released_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case MAIN_MENU:
      change_game_state(gsm, END);
      break;
    case GAME_PLAY:
      change_game_state(gsm, PAUSED);
      break;
    case SEC_GAME_PLAY:
      uart_send_leave_msg();
      change_game_state(gsm, MAIN_MENU);
      break;
    case PAUSED:
      change_game_state(gsm, GAME_PLAY);
      break;
    case HIGHSCORES:
      change_game_state(gsm, MAIN_MENU);
    case GAMEOVER:
    case NEW_HIGHSCORE:
      change_game_state(gsm, MAIN_MENU);
      break;
    default:
      break;
  }
}

static void keyboard_pressed_handler(GameStateMachine_t *gsm) {

  // map the last read scancode byte to the corresponding char
  char input = makecode_to_char(get_scancode());

  // if the input is the NULL char, then do nothing
  if (!input)
    return;

  switch (gsm->current_state) {
    case MAIN_MENU:
      // start a new game if either ENTER or SPACEBAR are pressed
      if (input == ' ' || input == '\n') {
        change_game_state(gsm, GAME_PLAY);
        break;
        case SEC_GAME_PLAY:
          // send the input char to the host player
          uart_send_kbd_char(input);
          break;
        case GAME_PLAY:
          // add a new projectile to the game
          if (input)
            game_check_new_projectiles(input);
          break;
        case PAUSED:
          // resume the game if either ENTER or SPACEBAR are pressed
          if (input == ' ' || input == '\n')
            change_game_state(gsm, GAME_PLAY);
          break;
        case GAMEOVER:
          // start a new game if either ENTER or SPACEBAR are pressed
          if (input == ' ' || input == '\n')
            change_game_state(gsm, GAME_PLAY);
          break;
        case NEW_HIGHSCORE:

          // check if input is the enter key and the username string isn't empty
          // else add the input char to the username
          if (input == '\n') {
            if (getGame()->username[0] != 0) {
              insert_new_highscore(getGame()->highscores, getGame()->username, getGame()->stat_score);
              store_highscores(getGame()->highscores);

              change_game_state(gsm, MAIN_MENU);
            }
          }
          else {
            gameover_set_username(input);

            
          }
          break;
        default:
          break;
      }
  }
}

static void mouse_moved_handler(GameStateMachine_t *gsm) {
  switch (gsm->current_state) {
    case MAIN_MENU:
    case PAUSED:
    case GAMEOVER:
    case NEW_HIGHSCORE:
      // update the position of the mouse cursor
      cursor_update_pos(getGame()->cursor, get_packet()->delta_x, get_packet()->delta_y);
      break;
    default:
      break;
  }
}

static void mouse_lb_pressed_handler(GameStateMachine_t *gsm) {

  // get the position of the upper left corner of the cursor when the left button was pressed
  uint16_t xpos = cursor_get_x_pos(getGame()->cursor);
  uint16_t ypos = cursor_get_y_pos(getGame()->cursor);

  switch (gsm->current_state) {
    case MAIN_MENU:

      if (button_pressed_in_range(getGame()->buttons[PLAY_BTN], xpos, ypos))
        change_game_state(gsm, GAME_PLAY);
      else if (button_pressed_in_range(getGame()->buttons[HIGHSCORES_BTN], xpos, ypos))
        change_game_state(gsm, HIGHSCORES);
      else if (button_pressed_in_range(getGame()->buttons[EXIT_BTN], xpos, ypos))
        change_game_state(gsm, END);
      else if (button_pressed_in_range(getGame()->buttons[HELP_BTN], xpos, ypos)) {
        // try to join the main game of the other user
        uart_send_join_msg();
      }
      break;

    case PAUSED:

      if (button_pressed_in_range(getGame()->buttons[RESUME_BTN], xpos, ypos))
        change_game_state(gsm, GAME_PLAY);
      else if (button_pressed_in_range(getGame()->buttons[QUIT_BTN], xpos, ypos))
        change_game_state(gsm, MAIN_MENU);

      break;

    case GAMEOVER:

      if (button_pressed_in_range(getGame()->buttons[MAIN_MENU_BTN], xpos, ypos)) {
        change_game_state(gsm, MAIN_MENU);
      }
      else if (button_pressed_in_range(getGame()->buttons[TRY_AGAIN_BTN], xpos, ypos)) {
        change_game_state(gsm, GAME_PLAY);
      }

      break;

    case NEW_HIGHSCORE:

      if (button_pressed_in_range(getGame()->buttons[CANCEL_BTN], xpos, ypos)) {
        change_game_state(gsm, MAIN_MENU);
      }
      else if (button_pressed_in_range(getGame()->buttons[SAVE_BTN], xpos, ypos)) {

        // check if the username string isn't empty
        if (getGame()->username[0] != 0) {
          insert_new_highscore(getGame()->highscores, getGame()->username, getGame()->stat_score);
          store_highscores(getGame()->highscores);

          change_game_state(gsm, MAIN_MENU);
        }
      }

      break;

    default:
      break;
  }
}

static void left_p_up_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY:
      if (game.freeze)
        game_activate_power_up(FREEZE);
      break;
    default:
      break;
  }
}

static void right_p_up_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY:

      if (game.splitshot) {
        
        struct RTCFields current_time;
        if (! rtc_get_currentTime(&current_time)) {

          game_activate_power_up(SPLIT);
          // set a RTC alarm to control the duration of the powerup
          rtc_set_alarm(-1, -1, current_time.seconds + PUP_DUR % 60);
        }
      }
      break;
    default:
      break; 
  }
}

static void alarm_ring_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY:

      
      if (getGame()->splitshot_active)
        getGame()->splitshot_active = false;

      
      rtc_disable_alarm_int();

      break;

    default:
      break;
  }
}

static void sec_passed_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY:
      game_increase_game_time();
      break;
    default:
      break;
  }
}

static void joined_2p_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY: {

      int num_enemies = 0;
      Enemy **enemies = get_enemy_array(&num_enemies);

      // send all the chars present in the game's current enemies to the secondary player
      for (int i = 0; i < num_enemies; i++)
        uart_send_add_char(enemies[i]->text.word[0]);

      helper_exists = true;
      break;
    }
    default:
      break;
  }
}

static void left_2p_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {
    case GAME_PLAY:
      helper_exists = false;
      break;
    default:
      break;
  }
}

static void left_1p_handler(GameStateMachine_t *gsm) {
  // if the main player leaves, end the game for the secondary player
  change_game_state(gsm, MAIN_MENU);
}

static void start_2p_handler(GameStateMachine_t *gsm) {
  // start the secondary game
  change_game_state(gsm, SEC_GAME_PLAY);
}

static void new_char_2p_handler(GameStateMachine_t *gsm) {

  // get the sent char
  struct uart_msg *msg = uart_get_last_msg();
  char input = msg->bytes[0];

  switch (gsm->current_state) {
    case GAME_PLAY:
      // add a new projectile to the game
      game_check_new_projectiles(input);
      break;
    default:
      break;
  }
}

static void sec_game_add_char_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {

    case SEC_GAME_PLAY: {
      struct uart_msg *msg = uart_get_last_msg();
      char input = msg->bytes[0];

      // add the game char to the secondary game
      game_add_sec_game_char(input);
    } break;
    default:
      break;
  }
}

static void sec_game_rem_char_handler(GameStateMachine_t *gsm) {

  switch (gsm->current_state) {

    case SEC_GAME_PLAY: {
      struct uart_msg *msg = uart_get_last_msg();
      char input = msg->bytes[0];

      // remove the char from the secondary game
      game_rem_sec_game_char(input);
    } break;
    default:
      break;
  }
}
