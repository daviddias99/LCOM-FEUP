#include "game.h"

bool helper_exists = false;

// entity storage
Enemy *enemy_array[MAX_ENEMIES];               /**< @brief array of enemies*/
Projectile *proj_array[MAX_PROJECTILES];       /**< @brief array of projectiles*/
AnimatedSprite *explosions_array[MAX_ENEMIES]; /**< @brief array of explosions(animated sprites)*/
char sec_game_chars[MAX_ENEMIES];
int enemy_count = 0;     /**< @brief size of the enemy array*/
int proj_count = 0;      /**< @brief size of the projectile array*/
int explosion_count = 0; /**< @brief size of the explosion array*/
int sec_game_chars_count = 0;

// letter queues
CharLinkedList letter_queue_dif1; /**< @brief Letter queue of the difficulty 1 (lower case) */
CharLinkedList letter_queue_dif2; /**< @brief Letter queue of the difficulty 2 (lower case, upper case)*/
CharLinkedList letter_queue_dif3; /**< @brief Letter queue of the difficulty 3 (lower case, upper case and numbers)*/
CharLinkedList letter_queue_dif4; /**< @brief Letter queue of the difficulty 4 (lower case, upper case, numbers and punctuation)*/

/** @brief array of letter queues */
CharLinkedList *letter_queues[4] = {&letter_queue_dif1,
                                    &letter_queue_dif2,
                                    &letter_queue_dif3,
                                    &letter_queue_dif4};

CharLinkedList *current_queue; /**< @brief current letter queue being used */

// enemy speed bounds
Bounds lvl1_xspeed = {LVL1_LOWER_XSPD_BOUND, LVL1_UPPER_XSPD_BOUND}; /**< @brief xspeed bounds for level 1 */
Bounds lvl1_yspeed = {LVL1_LOWER_YSPD_BOUND, LVL1_UPPER_YSPD_BOUND}; /**< @brief yspeed bounds for level 1 */
Bounds lvl2_xspeed = {LVL2_LOWER_XSPD_BOUND, LVL2_UPPER_XSPD_BOUND}; /**< @brief xspeed bounds for level 2 */
Bounds lvl2_yspeed = {LVL2_LOWER_YSPD_BOUND, LVL2_UPPER_YSPD_BOUND}; /**< @brief yspeed bounds for level 2 */
Bounds lvl3_xspeed = {LVL3_LOWER_XSPD_BOUND, LVL3_UPPER_XSPD_BOUND}; /**< @brief xspeed bounds for level 3 */
Bounds lvl3_yspeed = {LVL3_LOWER_YSPD_BOUND, LVL3_UPPER_YSPD_BOUND}; /**< @brief yspeed bounds for level 3 */
Bounds lvl4_xspeed = {LVL4_LOWER_XSPD_BOUND, LVL4_UPPER_XSPD_BOUND}; /**< @brief xspeed bounds for level 4 */
Bounds lvl4_yspeed = {LVL4_LOWER_YSPD_BOUND, LVL4_UPPER_YSPD_BOUND}; /**< @brief yspeed bounds for level 4 */
Bounds *xbounds[4] = {&lvl1_xspeed,
                      &lvl2_xspeed,
                      &lvl3_xspeed,
                      &lvl4_xspeed}; /**< @brief array of xspeed bounds */
Bounds *ybounds[4] = {&lvl1_yspeed,
                      &lvl2_yspeed,
                      &lvl3_yspeed,
                      &lvl4_yspeed}; /**< @brief array of yspeed bounds */

Bounds *current_xbound; /**< @brief current xspeed bounds */
Bounds *current_ybound; /**< @brief current yspeed bounds */

int game_add_enemy(Enemy *newEnemy) {

  if (helper_exists) {
    
    uart_send_add_char(newEnemy->text.word[0]);
  }

  if (enemy_count == MAX_ENEMIES)
    return 1;

  enemy_array[enemy_count] = newEnemy;
  enemy_count++;
  return 0;
}

int game_add_projectile(Projectile *newProjectile) {

  if (proj_count == MAX_PROJECTILES)
    return 1;

  proj_array[proj_count] = newProjectile;
  proj_count++;
  return 0;
}

int game_remove_enemy(Enemy *enemy, int arrayPos) {

  if (helper_exists) {
    
    uart_send_rem_char(enemy->text.word[0]);
  }

  
  llist_insert(current_queue, enemy->text.word[0], randomBetween(0, current_queue->size));

  // create an explosion
  AnimatedSprite *newEx = animated_sprite_create(game.explosions, enemy->sprite->pos, game.num_explosion_frames, 8, true);
  game_add_explosion(newEx);

  enemy_destroy(enemy);

  // make useless projectiles continue their path
  for (int k = 0; k < proj_count; k++) {

    if (proj_array[k]->tracking_enemy == enemy) {
      proj_array[k]->tracking_enemy = NULL;
    }
  }

  // make the spawning slot free
  for (int k = 0; k < spawn_slots.slot_count; k++) {

    if (spawn_slots.slots[k] == enemy) {
      spawn_slots.slots[k] = NULL;
    }
  }

  // update the enemy array
  for (int iter = arrayPos; iter < enemy_count - 1; iter++) {

    enemy_array[iter] = enemy_array[iter + 1];
  }

  enemy_count--;
  
  return 0;
}

int game_remove_proj(Projectile *proj, int arrayPos) {

  //delete the projectile that hit the ememy
  for (int iter = arrayPos; iter < proj_count - 1; iter++) {

    proj_array[iter] = proj_array[iter + 1];
  }

  proj_destroy(proj);
  proj_count--;

  return 0;
}

int game_remove_explosion(AnimatedSprite *exp, int arrayPos) {

  // remove explosion from the array
  for (int iter = arrayPos; iter < explosion_count - 1; iter++) {

    explosions_array[iter] = explosions_array[iter + 1];
  }

  // destroy sprite and update count
  animated_sprite_destroy(exp);
  explosion_count--;

  return 0;
}

int game_add_explosion(AnimatedSprite *newExplosion) {

  if (proj_count == MAX_EXPLOSIONS)
    return 1;

  explosions_array[explosion_count] = newExplosion;
  explosion_count++;
  return 0;
}

int game_handle_colision(Enemy *enemyA, Enemy *enemyB) {

  Enemy *topEnemyY;

  if (enemyA->sprite->speed.y > enemyB->sprite->speed.y)
    topEnemyY = enemyA;
  else if (enemyA->sprite->speed.y < enemyB->sprite->speed.y)
    topEnemyY = enemyB;
  else
    topEnemyY = enemyA;

  Enemy *topEnemyX;
  if (abs(enemyA->sprite->speed.x) > abs(enemyB->sprite->speed.x))
    topEnemyX = enemyA;
  else if (abs(enemyA->sprite->speed.x) < abs(enemyB->sprite->speed.x))
    topEnemyX = enemyB;
  else
    topEnemyX = enemyA;

  if ((float) enemyA->sprite->speed.x * (float) enemyB->sprite->speed.x <= 0) {

    Vector new_Vel1 = vector_create(enemyA->sprite->speed.x * -1, topEnemyY->sprite->speed.y);
    Vector new_Vel2 = vector_create(enemyB->sprite->speed.x * -1, topEnemyY->sprite->speed.y);
    enemy_change_vel(enemyB, new_Vel2);
    enemy_change_vel(enemyA, new_Vel1);
  }
  else {

    Vector new_Vel1 = vector_create(topEnemyX->sprite->speed.x * -1, topEnemyY->sprite->speed.y);
    enemy_change_vel(topEnemyX, new_Vel1);
  }

  return 0;
}

int game_check_col_enemies() {

  // check for colisions between enemies. If two enemies colide, their x speed is changed to bounce
  // them back
  for (int i = 0; i < enemy_count; i++) {

    for (int j = i + 1; j < enemy_count; j++) {

      if (enemy_check_colisions(enemy_array[i], enemy_array[j])) {

        game_handle_colision(enemy_array[i], enemy_array[j]);
      }
    }
  }
  return 0;
}

int game_check_col_proj_enemies() {

  // check colisions between enemies and projectiles
  for (int i = 0; i < enemy_count; i++) {

    for (int j = 0; j < proj_count; j++) {

      if (enemy_count == 0)
        break;

      if (i >= enemy_count)
        break;

      Enemy *currEn = enemy_array[i];
      Projectile *currPj = proj_array[j];

      if (currEn == NULL)
        continue;

      // check if the enemy was hit by the projectile
      if (proj_check_colisions(currPj, currEn)) {

        char letterStr[2] = "\0";
        letterStr[0] = currPj->letter;

        // check if the projectile was meant for that specific enemy

        if (strcmp(currEn->text.word, letterStr) == 0 || projectile_affects_all(currPj)) {

          // remove the projectile
          game_remove_proj(currPj, j);
          j--;

          // update stat count
          game.stat_total_hit_letters++;

          // powerup handling
          if (!projectile_affects_all(currPj) && game.splitshot_active && (currEn->shots_to_die == 1)) {

            Point enemy_center = vector_create(currEn->sprite->graphics.width / 2, currEn->sprite->graphics.height / 2);

            enemy_center = vector_sum(enemy_center, currEn->sprite->pos);

            game_add_split_projectiles(enemy_center);
          }

          // update the enemie's "life"
          currEn->shots_to_die--;

          // destroy the enemy
          if (currEn->shots_to_die == 0) {

            // give the player a powerup
            if (currEn->type == pup) {

              if (game.freeze && !game.splitshot)
                game.splitshot = true;
              else if (!game.freeze && game.splitshot && !game.freeze_active)
                game.freeze = true;
              else if (!game.freeze && !game.splitshot && !game.freeze_active) {

                int random = randomBetween(1, 2);

                if (random == 1) {
                  game.freeze = true;
                }
                else {
                  game.splitshot = true;
                }
              }

              game.stat_total_powerups++;
            }

            if (currEn->type == frozen)
              game.num_frozen--;

            game_remove_enemy(currEn, i);
            if (j == proj_count - 1)
              i--;

            // update stat count
            game.stat_total_enemies_killed++;
          }
        }
        else {
          // update the projectile array, the projectile is destroyed
          // even if he hit the wrong enemy

          game_remove_proj(currPj, j);
          j--;
        }
      }
    }
  }

  return 0;
}

int game_check_col_enemies_player() {
  // check if an enemy makes the player lose life
  for (int i = 0; i < enemy_count; i++) {
    Enemy *currEn = enemy_array[i];
    if ((sprite_checkColision(currEn->sprite, game.player_sprite)) || (currEn->sprite->pos.y + currEn->sprite->graphics.height > DAMAGING_HEIGHT)) {
      game.stat_player_life -= 1;
      game_update_life_counter();

      game_remove_enemy(currEn, i);
      i--;
    }
  }
  return 0;
}

int game_check_col_enemies_bounds() {

  // check for enemies that are crossing the bounds if the screen and change their xspeed
  // bounce them back. The bounds that are solid are given by a mask of four bits that signalas
  // LeftBound|UpperBound|RightBound|LowerBound, in this case 1010
  for (int i = 0; i < enemy_count; i++) {

    Enemy *currEn = enemy_array[i];

    if (enemy_check_bound_crossing(currEn, 0xA)) {

      Vector new_Vel = vector_create(currEn->sprite->speed.x * -1, currEn->sprite->speed.y);
      enemy_change_vel(currEn, new_Vel);
    }
  }

  return 0;
}

int game_check_out_of_bounds() {

  /*
  
  // check if enemies are out of bounds, this feature is commented out because enemies already spawn out of bounds

  for (int i = 0; i < enemy_count; i++) {

    if (enemy_check_out_of_bounds(enemy_array[i])) {

      game_remove_enemy(enemy_array[i],i);
      i--;
    }
  }
  */

  // check for projectiles that are out of bounds and delete them if so
  for (int i = 0; i < proj_count; i++) {

    if (proj_check_out_of_bounds(proj_array[i])) {

      // delete projectile
      game_remove_proj(proj_array[i], i);
      i--;
    }
  }

  return 0;
}

int game_check_colisions() {

  game_check_col_enemies_player();

  game_check_out_of_bounds();

  game_check_col_enemies_bounds();

  game_check_col_enemies();

  game_check_col_proj_enemies();

  return 0;
}

int game_update() {

  // increase the game timer. If the spawning is disabled don't increase the time
  // this is done to discourage "making time" while the next "round" doesn't start
  if (game.spawning_enabled)
    game.timer_tick++;

  // start level up process
  if (game.timer_tick % game.level_up_time == 0) {

    if (game.current_difficulty != 4) {
      game.spawning_enabled = false;            // disable spawning
      game.spawn_rate -= game.spawn_rate_delta; // increase the spawn rate
      game.current_difficulty++;                // increment the difficulty

      game.timer_tick++;
    }
  }

  // when there aren't any more enemies left, change the difficulty
  // and enable spawning
  if (!game.spawning_enabled) {

    if (enemy_count == 0) {
      current_queue = letter_queues[game.current_difficulty - 1];
      current_xbound = xbounds[game.current_difficulty - 1];
      current_ybound = ybounds[game.current_difficulty - 1];
      game.spawning_enabled = true;
    }
  }

  // spawn a new enemy
  if (game.timer_tick % game.spawn_rate == 0) {

    if (game.spawning_enabled) {
      bool isAvailable = game_check_available_slot();

      if (isAvailable) {

        // get the char for the new Enemy
        char *newChar = (char *) malloc(sizeof(char) * 2);
        newChar[0] = llist_pop(current_queue);
        newChar[1] = '\0';

        // get the starting speed for the new enemy
        int randomXSpeed = randomBetween(current_xbound->lowerBound, current_xbound->upperBound);
        int randomYSpeed = randomBetween(current_ybound->lowerBound, current_ybound->upperBound);
        Vector speed = vector_create(randomXSpeed, randomYSpeed);

        // create a new enemy, randomize if the enemy will drop a powerup
        int randomInt = randomBetween(0, 5);

        Enemy *newEn;

        if (randomInt == 3)
          newEn = enemy_create_pup(game.enemy_pup_pixmap, vector_create(0, 0), newChar, PUP_ENEMY_LIFE);
        else
          newEn = enemy_create_img(game.enemy_pixmap, vector_create(0, 0), newChar);

        // change the new enemy's speed
        enemy_change_vel(newEn, speed);

        spawn_slots.slots[spawn_slots.slot_current] = newEn;
        spawn_slots.slot_current = (spawn_slots.slot_current + 1) % spawn_slots.slot_count;

        // get the starting position for the new enemy
        int randomXPos = spawn_slots.slot_width * spawn_slots.slot_current;
        int randomYPos = spawn_slots.slot_ypos;
        Vector pos = vector_create(randomXPos, randomYPos);
        enemy_change_pos(newEn, pos);

        // add the new enemy to the game
        game_add_enemy(newEn);
      }
    }
  }

  // update enemies
  for (int i = 0; i < enemy_count; i++) {

    // store in curr_enemy the current enemy to be processed
    Enemy *curr_enemy = enemy_array[i];

    // update the enemies position (colision checking must be done previously)
    enemy_update(curr_enemy, false);
  }

  // update projectiles
  for (int i = 0; i < proj_count; i++) {
    projectile_update(proj_array[i]);
  }

  // update explosions
  for (int i = 0; i < explosion_count; i++) {

    AnimatedSprite *current = explosions_array[i];

    if (current->xpm_current == -1) {

      animated_sprite_destroy(current);

      for (int iter = i; iter < explosion_count - 1; iter++)
        explosions_array[iter] = explosions_array[iter + 1];

      i--;
      explosion_count--;
    }
  }

  for (int i = 0; i < explosion_count; i++) {

    AnimatedSprite *current = explosions_array[i];

    animated_sprite_update(current);
  }

  draw_stat_bar();

  return 0;
}

int game_load_queue() {

  // level 4 queue (lower,upper,number,symbols)
  letter_queue_dif4.size = 0;
  letter_queue_dif4.header = malloc(sizeof(CharLinkedListNode));
  letter_queue_dif4.header->next = NULL;
  llist_insert(&letter_queue_dif4, '!', 0);

  for (int i = 34; i < 123; i++) {

    // don't add char's that don't belong to this queue
    if ((i == 60) || (i == 62) || (i == 64) || (i == 91) || (i == 92) || (i == 93) || (i == 94) || (i == 96))
      continue;

    int randomInt = randomBetween(0, letter_queue_dif4.size);
    llist_insert(&letter_queue_dif4, (char) i, randomInt);
  }

  // level 3 queue (lower,upper,number)

  letter_queue_dif3.size = 0;
  letter_queue_dif3.header = malloc(sizeof(CharLinkedListNode));
  letter_queue_dif3.header->next = NULL;
  llist_insert(&letter_queue_dif3, '0', 0);

  for (int i = 49; i < 123; i++) {

    // don't add char's that don't belong to this queue
    if ((i == 58) || (i == 59) || (i == 60) || (i == 61) || (i == 62) || (i == 63) || (i == 64) || (i == 91) || (i == 92) || (i == 93) || (i == 94) || (i == 95) || (i == 96))
      continue;

    int randomInt = randomBetween(0, letter_queue_dif3.size);
    llist_insert(&letter_queue_dif3, (char) i, randomInt);
  }

  // level 2 queue (lower,upper)

  letter_queue_dif2.size = 0;
  letter_queue_dif2.header = malloc(sizeof(CharLinkedListNode));
  letter_queue_dif2.header->next = NULL;
  llist_insert(&letter_queue_dif2, 'A', 0);

  for (int i = 66; i < 123; i++) {

    // don't add char's that don't belong to this queue
    if ((i == 91) || (i == 92) || (i == 93) || (i == 94) || (i == 95) || (i == 96))
      continue;

    int randomInt = randomBetween(0, letter_queue_dif2.size);
    llist_insert(&letter_queue_dif2, (char) i, randomInt);
  }

  // level 1 queue(lower)

  letter_queue_dif1.size = 0;
  letter_queue_dif1.header = malloc(sizeof(CharLinkedListNode));
  letter_queue_dif1.header->next = NULL;
  llist_insert(&letter_queue_dif1, 'a', 0);

  for (int i = 98; i < 123; i++) {

    int randomInt = randomBetween(0, letter_queue_dif1.size);
    llist_insert(&letter_queue_dif1, (char) i, randomInt);
  }

  return 0;
}

int game_destroy_queues() {

  llist_empty(&letter_queue_dif1);
  free(letter_queue_dif1.header);
  llist_empty(&letter_queue_dif2);
  free(letter_queue_dif2.header);
  llist_empty(&letter_queue_dif3);
  free(letter_queue_dif3.header);
  llist_empty(&letter_queue_dif4);
  free(letter_queue_dif4.header);

  return 0;
}

int game_check_new_projectiles(char letter) {

  // construct a string using the pressed key
  char letterStr[2] = "\0";
  letterStr[0] = letter;
  letterStr[1] = '\0';

  if (letter != '\0')
    game.stat_total_typed_letters++;

  // create a new projectile if a given letter is pressed
  for (int i = 0; i < enemy_count; i++) {

    // store in curr_enemy the current enemy to be processed
    Enemy *curr_enemy = enemy_array[i];

    if (letter != '\0') {

      if (strcmp(curr_enemy->text.word, letterStr) == 0) {

        int proj_starting_X = get_hres() / 2 - 65;
        int proj_starting_Y = get_vres() - game.player_pixmap.height + 65;

        // create the projectile
        Projectile *proj = projectile_create_img(game.projectiles, vector_create(proj_starting_X, proj_starting_Y), enemy_array[i], PROJ_SPEED_MODIFIER, letter, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

        // add the projectile to the game
        game_add_projectile(proj);
      }
    }
  }

  return 0;
}

bool game_check_available_slot() {

  Enemy *enemy_at_slot = spawn_slots.slots[spawn_slots.slot_current];

  if (enemy_at_slot == NULL) {
    return true;
  }

  bool empty_slot = false;

  if (enemy_at_slot->sprite->pos.y >= 0)
    empty_slot = true;

  return empty_slot;
}

Game *getGame() {
  return &game;
}

int load_game_data() {

  load_highscores(game.highscores);
  //insert_new_highscore(game.highscores, "novo", 8000);
 
  //store_highscores(game.highscores);

  // load sprites
  xpm_load(sp_bg_xpm, XPM_1_5_5_5, &game.game_background);
  xpm_load(paused_screen_xpm, XPM_1_5_5_5, &game.paused_image);
  xpm_load(gameover_xpm, XPM_1_5_5_5, &game.gameover_image);
  xpm_load(new_highscore_xpm, XPM_1_5_5_5, &game.new_highscore_image);
  xpm_load(highscores_menu_xpm, XPM_1_5_5_5, &game.highscores_image);
  xpm_load(spaceship_pup_xpm, XPM_1_5_5_5, &game.enemy_pup_pixmap);
  xpm_load(spaceship2_xpm, XPM_1_5_5_5, &game.enemy_pixmap);
  xpm_load(logo_xpm, XPM_1_5_5_5, &game.logo_pixmap);
  xpm_load(caps_off2_xpm, XPM_1_5_5_5, &game.caps_off);
  xpm_load(caps_on2_xpm, XPM_1_5_5_5, &game.caps_on);
  xpm_load(pup_both_xpm, XPM_1_5_5_5, &game.pup_both);
  xpm_load(pup_freeze_xpm, XPM_1_5_5_5, &game.pup_freeze);
  xpm_load(pup_split_xpm, XPM_1_5_5_5, &game.pup_split);
  xpm_load(pup_none_xpm, XPM_1_5_5_5, &game.pup_none);
  xpm_load(player_spaceship_xpm, XPM_1_5_5_5, &game.player_pixmap);

  game.num_explosion_frames = EXP_NUM_FRAMES;
  game.explosions = malloc(sizeof(xpm_image_t) * game.num_explosion_frames);

  for (int i = 0; i < game.num_explosion_frames; i++)
    xpm_load(explosions[i], XPM_1_5_5_5, &game.explosions[i]);

  game.num_projectiles_frames = PROJ_NUM_FRAMES;
  game.projectiles = malloc(sizeof(xpm_image_t) * game.num_projectiles_frames);

  for (int i = 0; i < game.num_projectiles_frames; i++)
    xpm_load(projectile_orange_animation[i], XPM_1_5_5_5, &game.projectiles[i]);

  game.life_total_frames = DEFAULT_PLAYER_LIFE;
  game.life_counter = malloc(sizeof(xpm_image_t) * game.life_total_frames);

  for (int i = 0; i <= game.life_total_frames; i++)
    xpm_load(life_counter[i], XPM_1_5_5_5, &game.life_counter[i]);

  // create cursor
  game.cursor = cursor_new(cursor_xpm);

  for (size_t i = 0; i < NUMBER_BUTTONS; i++) {

    game.buttons[i] = button_new(button_xpm[i], button_x_pos[i], button_y_pos[i]);
  }

  // setup spawning slots
  spawn_slots.slot_count = get_hres() / game.enemy_pixmap.width;
  spawn_slots.slot_width = game.enemy_pixmap.width;
  spawn_slots.slot_ypos = -game.enemy_pixmap.height - 5;
  spawn_slots.slot_current = 0;
  spawn_slots.slots = malloc(sizeof(Enemy *) * spawn_slots.slot_count);

  for (int i = 0; i < spawn_slots.slot_count; i++) {
    spawn_slots.slots[i] = 0;
  }

  // load the letter queues
  game_load_queue();
  current_queue = &letter_queue_dif1;

  // setup difficulty settings
  game.current_difficulty = 1;
  current_xbound = xbounds[0];
  current_ybound = ybounds[0];

  // setup statBar;
  Vector lc_position = {LIFE_COUNTER_X, LIFE_COUNTER_Y};
  Vector pups_position = {20 + game.life_counter[6].width, 5};
  Vector caps_position = {10 + pups_position.x + game.pup_none.width, 5};

  stat_sprites.caps_lock = sprite_create_img(game.caps_off, caps_position);
  stat_sprites.powerups = sprite_create_img(game.pup_none, pups_position);
  stat_sprites.life_counter = sprite_create_img(game.life_counter[6], lc_position);

  // init variables

  game.freeze = false;
  game.splitshot = false;

  game.current_difficulty = INITIAL_LEVEL;
  game.stat_total_enemies_killed = 0;
  game.stat_total_hit_letters = 0;
  game.stat_total_powerups = 0;
  game.stat_total_typed_letters = 0;
  game.stat_default_player_life = DEFAULT_PLAYER_LIFE;
  game.stat_player_life = game.stat_default_player_life;
  game.default_spawn_rate = INITIAL_SPAWN_RATE;
  game.spawn_rate = game.default_spawn_rate;
  game.spawn_rate_delta = SPAWN_RATE_DELTA;
  game.level_up_time = LEVEL_UP_TIME_DELTA;
  game.time_sec = 0;
  game.timer_tick = 0;
  game.spawning_enabled = true;
  game.num_frozen = 0;

  // setup the player sprite
  game.player_sprite = sprite_create_img(game.player_pixmap, vector_create(get_hres() / 2 - game.player_pixmap.width / 2 - 20, get_vres() - game.player_pixmap.height / 2 - 35));

  return 0;
}

int game_update_life_counter() {

  if (game.stat_player_life >= 0)
    sprite_change_graphics(stat_sprites.life_counter, game.life_counter[(int) ceil(game.stat_player_life)]);
  return 0;
}

void draw_standard_menu() {

  draw_aux_buffer();
  cursor_draw(getGame()->cursor);

  display_scene();
}

void draw_sec_game_play() {

  draw_background2(game.game_background);

  char str[2];
  str[1] = 0;

  int xpos = 100;
  int ypos = 300;

  for (size_t i = 0; i < (size_t) sec_game_chars_count; i++) {

    if (xpos > get_hres() - 100) {
      xpos = 100;
      ypos += 200;
    }

    draw_xpm(&game.enemy_pixmap, xpos, ypos);
    str[0] = sec_game_chars[i];
    drawWord(str, xpos + 48, ypos + 6, 2);

    xpos += game.enemy_pixmap.width + 20;
  }

  display_scene();
}

void draw_game_play() {

  game_check_colisions();
  draw_background2(game.game_background);
  game_update();
  sprite_draw(game.player_sprite);
  display_scene();
}

void draw_new_highscore_menu() {

  draw_aux_buffer();

  drawWord(game.username, GAME_OVER_USERNAME_X, GAME_OVER_USERNAME_Y, 3);
  cursor_draw(game.cursor);

  display_scene();
}

void draw_gameover_stats() {

  char numbers[30];
  sprintf(numbers, "Score: %d", game.stat_score);
  drawWord(numbers, 305, 250, 4);

  sprintf(numbers, "Chars/minute: %d", game.stat_cpm);
  drawWord(numbers, 305, 280, 4);

  sprintf(numbers, "Accuracy: %d%%", game.stat_acc);
  drawWord(numbers, 305, 310, 4);

  sprintf(numbers, "Game time: %d'%d\"", game.time_sec / 60, game.time_sec & 60);
  drawWord(numbers, 305, 340, 4);
}

void draw_highscores() {
  for (size_t i = 0; i < NUMBER_HIGHSCORES; i++) {

    char highscore_line[4];

    if (game.highscores[i] == NULL)
      break;

    highscore_line[0] = i + 1 + 48;
    highscore_line[1] = '.';
    highscore_line[2] = ' ';
    highscore_line[3] = 0;

    drawWord(highscore_line, POP_UP_X_POS + 20, POP_UP_Y_POS + 125 + (i * 50), 2);
  }

  for (size_t i = 0; i < NUMBER_HIGHSCORES; i++) {

    if (game.highscores[i] == NULL)
      break;

    drawWord(game.highscores[i]->username, POP_UP_X_POS + 60, POP_UP_Y_POS + 125 + (i * 50), 2);
  }

  for (size_t i = 0; i < NUMBER_HIGHSCORES; i++) {

    char numbers[10];

    if (game.highscores[i] == NULL)
      break;

    sprintf(numbers, "%d", game.highscores[i]->score);

    drawWord(numbers, POP_UP_X_POS + 230, POP_UP_Y_POS + 125 + (i * 50), 2);
  }

  for (size_t i = 0; i < NUMBER_HIGHSCORES; i++) {

    char numbers[11];

    if (game.highscores[i] == NULL)
      break;

    sprintf(numbers, "%02d/%02d/%04d", game.highscores[i]->timestamp.day,
            game.highscores[i]->timestamp.month, 2000 + game.highscores[i]->timestamp.year);

    drawWord(numbers, POP_UP_X_POS + 375, POP_UP_Y_POS + 125 + (i * 50), 2);
  }

  for (size_t i = 0; i < NUMBER_HIGHSCORES; i++) {

    char numbers[6];

    if (game.highscores[i] == NULL)
      break;

    sprintf(numbers, "%02d:%02d", game.highscores[i]->timestamp.hours, game.highscores[i]->timestamp.minutes);

    drawWord(numbers, POP_UP_X_POS + 550, POP_UP_Y_POS + 125 + (i * 50), 2);
  }
}

void draw_stat_bar() {

  if (get_caps_status())
    sprite_change_graphics(stat_sprites.caps_lock, game.caps_on);
  else
    sprite_change_graphics(stat_sprites.caps_lock, game.caps_off);

  if (game.splitshot && game.freeze) {
    sprite_change_graphics(stat_sprites.powerups, game.pup_both);
  }
  else if (!game.splitshot && !game.freeze) {
    sprite_change_graphics(stat_sprites.powerups, game.pup_none);
  }
  else if (game.splitshot) {
    sprite_change_graphics(stat_sprites.powerups, game.pup_split);
  }
  else if (game.freeze) {
    sprite_change_graphics(stat_sprites.powerups, game.pup_freeze);
  }

  sprite_draw(stat_sprites.life_counter);
  sprite_draw(stat_sprites.caps_lock);
  sprite_draw(stat_sprites.powerups);

  // score
  uint16_t scoreXPos = 8 * get_hres() / 10;
  drawWord("Score:", scoreXPos, 5, 2);
  game_calculate_score();
  char score_str[12];
  sprintf(score_str, "%d", game.stat_score);

  scoreXPos += getWordWidth("Score:", 2) + 4;
  drawWord(score_str, scoreXPos, 5, 2);

  // char per minute

  uint16_t CPMXPos = 8 * get_hres() / 10;
  drawWord("CPM:", CPMXPos, 5 + getAlphabetHeigth() + 2, STAT_BAR_WORD_SPACING);
  game_calculate_cpm();
  char cpm_str[12];
  sprintf(cpm_str, "%d", game.stat_cpm);

  CPMXPos += getWordWidth("Score:", 2) + 4;
  drawWord(cpm_str, CPMXPos, 5 + getAlphabetHeigth() + 2, STAT_BAR_WORD_SPACING);

  // Accuracy
  uint16_t accXPos = 8 * get_hres() / 10;
  drawWord("Acc:", accXPos, 5 + 2 * getAlphabetHeigth() + 4, STAT_BAR_WORD_SPACING);
  game_calculate_acc();
  char acc_str[12];
  sprintf(acc_str, "%d", game.stat_acc);

  accXPos += getWordWidth("Score:", 2) + 4;
  drawWord(acc_str, accXPos, 5 + 2 * getAlphabetHeigth() + 4, STAT_BAR_WORD_SPACING);
  drawWord("%", accXPos + getWordWidth(acc_str, 2) + 4, 5 + 2 * getAlphabetHeigth() + 4, STAT_BAR_WORD_SPACING);

  // Game time
  uint16_t gtXPos = 8 * get_hres() / 10;
  int gt_min = game.time_sec / 60;
  char gt_min_str[12];
  sprintf(gt_min_str, "%d", gt_min);
  drawWord(gt_min_str, gtXPos, 5 + 3 * getAlphabetHeigth() + 6, STAT_BAR_WORD_SPACING);

  gtXPos += getWordWidth(gt_min_str, 2) + 2;
  drawWord(":", gtXPos, 5 + 3 * getAlphabetHeigth() + 6, STAT_BAR_WORD_SPACING);
  gtXPos += getWordWidth(":", 2) + 2;
  int gt_sec = game.time_sec % 60;
  char gt_sec_str[12];

  sprintf(gt_sec_str, "%d", gt_sec);
  if (gt_sec < 9) {

    gt_sec_str[2] = gt_sec_str[1];
    gt_sec_str[1] = gt_sec_str[0];
    gt_sec_str[0] = '0';
  }

  drawWord(gt_sec_str, gtXPos, 5 + 3 * getAlphabetHeigth() + 6, STAT_BAR_WORD_SPACING);
}

int game_calculate_score() {

  int score = game.time_sec / 10 + (game.stat_total_enemies_killed + game.stat_total_hit_letters + game.stat_total_powerups) * 25;

  game.stat_score = score;

  return score;
}

int game_calculate_cpm() {

  if (game.time_sec == 0) {
    game.stat_cpm = 0;
    return 0;
  }

  int cpm = game.stat_total_hit_letters * 60 / game.time_sec;
  game.stat_cpm = cpm;
  return cpm;
}

int game_calculate_acc() {

  if (game.stat_total_typed_letters == 0) {
    game.stat_acc = 100;
    return 100;
  }

  int acc = floor(((double) game.stat_total_hit_letters / game.stat_total_typed_letters) * 100);
  game.stat_acc = acc;
  return acc;
}

void game_activate_power_up(power_up up) {

  if (up == SPLIT || up == BOTH) {
    game.splitshot_active = true;
    game.splitshot = false;
  }

  if (up == FREEZE || up == BOTH) {
    game.freeze_active = true;
    game.freeze = false;

    for (int i = 0; i < enemy_count; i++) {

      Enemy *currEn = enemy_array[i];

      if (currEn->type == pup)
        break;

      Vector currVel = currEn->sprite->speed;

      if (currVel.y > 1)
        currVel.y -= 1;

      if (currVel.x < 1)
        currVel.x += 1;
      else if (currVel.x > 1)
        currVel.x -= 1;

      currEn->sprite->speed = currVel;
      currEn->type = frozen;
      game.num_frozen++;
    }
  }
}

void game_add_split_projectiles(Point pos) {

  Projectile *newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(0, -1), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(1, -1), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(-1, -1), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(-1, 1), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(1, 1), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(1, 0), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);

  newProj = projectile_affects_all_create(game.projectiles, pos, vector_create(-1, 0), PROJ_SPEED_MODIFIER, game.num_projectiles_frames, PROJ_TIME_BETWEEN_FRAMES);

  game_add_projectile(newProj);
}

int game_reset() {

  game_reset_username();

  // empty enemy array
  while (enemy_count != 0) {
    game_remove_enemy(enemy_array[0], 0);
  }

  // empty projectile array
  while (proj_count != 0) {
    game_remove_proj(proj_array[0], 0);
  }

  // empty explosion array
  while (explosion_count != 0) {
    game_remove_explosion(explosions_array[0], 0);
  }

  // destroy letter queues
  game_destroy_queues();

  // reset necessary variables
  game.current_difficulty = INITIAL_LEVEL;
  game.stat_total_enemies_killed = 0;
  game.stat_total_hit_letters = 0;
  game.stat_total_powerups = 0;
  game.stat_total_typed_letters = 0;
  game.stat_default_player_life = DEFAULT_PLAYER_LIFE;
  game.stat_player_life = game.stat_default_player_life;
  game.default_spawn_rate = INITIAL_SPAWN_RATE;
  game.spawn_rate = game.default_spawn_rate;
  game.spawn_rate_delta = SPAWN_RATE_DELTA;
  game.level_up_time = LEVEL_UP_TIME_DELTA;
  game.time_sec = 0;
  game.timer_tick = 0;
  game.spawning_enabled = true;
  game.freeze = false;
  game.splitshot = false;
  game.freeze_active = false;
  game.splitshot_active = false;
  game.num_frozen = 0;

  current_xbound = xbounds[0];
  current_ybound = ybounds[0];

  // reset life counter graphics
  sprite_change_graphics(stat_sprites.life_counter, game.life_counter[6]);

  // reset spawning slots
  spawn_slots.slot_current = 0;
  for (int i = 0; i < spawn_slots.slot_count; i++) {
    spawn_slots.slots[i] = 0;
  }

  // reload letter queues
  game_load_queue();
  current_queue = &letter_queue_dif1;

  return 0;
}

void game_increase_game_time() {

  game.time_sec++;
}

bool game_player_lost() {
  return game.stat_player_life == 0;
}

void game_reset_username() {

  game.username[0] = 0;
  game.usr_name_curr_index = 0;
}

void gameover_set_username(char new_char) {

  if (game.usr_name_curr_index <= USERNAME_MAX_SIZE) {

    if (new_char == '\b') {

      if (game.usr_name_curr_index != 0) {
        game.usr_name_curr_index--;
        game.username[game.usr_name_curr_index] = 0;
      }
    }
    else if (game.usr_name_curr_index < USERNAME_MAX_SIZE) {

      game.username[game.usr_name_curr_index] = new_char;
      game.usr_name_curr_index++;
      game.username[game.usr_name_curr_index] = 0;
    }
  }
}

void game_add_sec_game_char(char c) {

  sec_game_chars[sec_game_chars_count] = c;
  sec_game_chars_count++;
}

void game_rem_sec_game_char(char c) {

  size_t i = 0;

  for (i = 0; i < (size_t) sec_game_chars_count; i++) {
    if (sec_game_chars[i] == c)
      break;
  }

  if (i == (size_t) sec_game_chars_count)
    return;

  for (; i < (size_t) sec_game_chars_count - 1; i++) {
    sec_game_chars[i] = sec_game_chars[i + 1];
  }

  sec_game_chars_count--;
}

Enemy **get_enemy_array(int *num_enemies) {

  *num_enemies = enemy_count;

  return enemy_array;
}

void sec_game_reset() {

  sec_game_chars_count = 0;
}

void game_free_data(){

  // free sprite pixmaps
  free(game.game_background.bytes);
  free(game.paused_image.bytes);
  free(game.gameover_image.bytes);
  free(game.new_highscore_image.bytes);
  free(game.highscores_image.bytes);
  free(game.enemy_pup_pixmap.bytes);
  free(game.enemy_pixmap.bytes);
  free(game.logo_pixmap.bytes);
  free(game.caps_off.bytes);
  free(game.caps_on.bytes);
  free(game.pup_both.bytes);
  free(game.pup_freeze.bytes);
  free(game.pup_split.bytes);
  free(game.pup_none.bytes);
  free(game.player_pixmap.bytes);

  game.num_explosion_frames = EXP_NUM_FRAMES;
  
  // free explosions frames
  for (int i = 0; i < game.num_explosion_frames; i++)
    free(game.explosions[i].bytes);
  free(game.explosions);
  game.num_projectiles_frames = PROJ_NUM_FRAMES;
  
  // free projectile frames
  for (int i = 0; i < game.num_projectiles_frames; i++)
    free(game.projectiles[i].bytes);
  free(game.projectiles);
  game.life_total_frames = DEFAULT_PLAYER_LIFE;
  
  // free life counter frames
  for (int i = 0; i <= game.life_total_frames; i++)
    free(game.life_counter[i].bytes);
  free(game.life_counter);

  // delete curspr
  cursor_del(game.cursor);

  // delete buttons
  for (size_t i = 0; i < NUMBER_BUTTONS; i++) {

    button_del(game.buttons[i]);
  }
}

