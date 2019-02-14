#include "Enemy.h"


Enemy * enemy_create(xpm_string_t* xpm, Vector pos, char* letter){

  Enemy* newEnemy = (Enemy*)malloc(sizeof(Enemy));
  Textbox newCaixa;
 
  newEnemy->sprite = sprite_create(xpm,pos);
  newCaixa.word = letter;

  newEnemy->text = newCaixa;
  newEnemy->type = normal;
  newEnemy->shots_to_die = 1;

  return newEnemy;
}

Enemy * enemy_create_img(xpm_image_t pixmap, Vector pos, char* letter){

  Enemy* newEnemy = (Enemy*)malloc(sizeof(Enemy));
  Textbox newCaixa;
 
  newEnemy->sprite = sprite_create_img(pixmap,pos);
  newCaixa.word = letter;

  newEnemy->text = newCaixa;
  newEnemy->type = normal;
  newEnemy->shots_to_die = 1;


  return newEnemy;

}

Enemy* enemy_create_pup(xpm_image_t pixmap, Vector pos, char* letter, int num_shots_to_die){

    Enemy* newEnemy = (Enemy*)malloc(sizeof(Enemy));
  Textbox newCaixa;
 
  newEnemy->sprite = sprite_create_img(pixmap,pos);
  newCaixa.word = letter;

  newEnemy->text = newCaixa;
  newEnemy->type = pup;
  newEnemy->shots_to_die = num_shots_to_die;

  return newEnemy;
}

int enemy_update(Enemy* enemy, bool letterTyped){

  sprite_update(enemy->sprite);

  if(letterTyped){
        //destruir nave;

  }
  
  drawWord(enemy->text.word,(uint16_t) enemy->sprite->pos.x + (uint16_t) enemy->sprite->graphics.width/2 - getWordWidth(enemy->text.word,2)/2,(uint16_t)enemy->sprite->pos.y + 7, 2);
  
 
  return 0;
}

int enemy_change_pos(Enemy* enemy, Vector new_pos){

  return sprite_change_pos(enemy->sprite,new_pos);
}

int enemy_change_vel(Enemy* enemy, Vector vel){

  return sprite_change_vel(enemy->sprite,vel);
}

int enemy_draw(Enemy* enemy){

  return sprite_draw(enemy->sprite);
}

bool enemy_check_colisions(Enemy * enemyA, Enemy* enemyB){

  return sprite_checkColision(enemyA->sprite,enemyB->sprite);
}

bool enemy_check_out_of_bounds(Enemy* enemy){

  return sprite_check_out_of_bounds(enemy->sprite);
}

bool enemy_check_bound_crossing(Enemy* enemy, uint8_t boundMask){

  return sprite_check_bound_crossing(enemy->sprite,boundMask);
}

int enemy_destroy(Enemy* enemy){

  sprite_destroy(enemy->sprite);
  free(enemy->text.word);
  free(enemy);

  return 0;
}


