#include "Projectile.h"

/*
Projectile* projectile_create(xpm_string_t* xpm, Vector position, Enemy* enemy, double speed_modifier, char letter){


  // alocate memory for a new projectile
  Projectile* newProj= (Projectile*)malloc(sizeof(Projectile));

  // calculate the initial speed for the projectile. This projectile is guided
  // towards the enemy which he is tracking, so it's velocity is a vector that
  // points to the enemy, scaled down or up by a modifier

  Sprite* tracking_en_sprite = enemy->sprite;
  Vector enemy_center = {tracking_en_sprite->pos.x + floor(tracking_en_sprite->graphics.width/2.0),
                        tracking_en_sprite->pos.y + floor(tracking_en_sprite->graphics.height/2.0),
                        };
  Vector initial_speed = vector_make(position, enemy_center);
  double vec_Norm = vector_getNorm(initial_speed);
  initial_speed = vector_mul_factor(initial_speed,speed_modifier);
  initial_speed = vector_mul_factor(initial_speed,1.0/vec_Norm);

  // create the projectile's sprite
  newProj->ani = sprite_create(xpm,position);

  newProj->tracking_enemy = enemy;
  newProj->speed_modifier = speed_modifier;
  newProj->sprite->speed = initial_speed;
  
  newProj->letter = letter;
  
  return newProj;



} */

Projectile* projectile_create_img(xpm_image_t* pixmap, Vector position, Enemy* enemy, double speed_modifier, char letter,int xpm_count, int time_between){

    // alocate memory for a new projectile
  Projectile* newProj= (Projectile*)malloc(sizeof(Projectile));

  // calculate the initial speed for the projectile. This projectile is guided
  // towards the enemy which he is tracking, so it's velocity is a vector that
  // points to the enemy, scaled down or up by a modifier

  Sprite* tracking_en_sprite = enemy->sprite;
  Vector enemy_center = {tracking_en_sprite->pos.x + floor(tracking_en_sprite->graphics.width/2.0),
                        tracking_en_sprite->pos.y + floor(tracking_en_sprite->graphics.height/2.0),
                        };
  Vector initial_speed = vector_make(position, enemy_center);
  double vec_Norm = vector_getNorm(initial_speed);
  initial_speed = vector_mul_factor(initial_speed,speed_modifier);
  initial_speed = vector_mul_factor(initial_speed,1.0/vec_Norm);

  // create the projectile's sprite
  newProj->an_sprite = animated_sprite_create(pixmap,position,xpm_count,time_between,false);

  newProj->tracking_enemy = enemy;
  newProj->speed_modifier = speed_modifier;
  animated_sprite_change_vel(newProj->an_sprite,initial_speed);
  
  newProj->letter = letter;

  newProj->affects_all = false;

  return newProj;
}

Projectile* projectile_affects_all_create(xpm_image_t* pixmap, Vector position, Vector speed, double speed_modifier, int xpm_count, int time_between) {


  // alocate memory for a new projectile
  Projectile* newProj= (Projectile*)malloc(sizeof(Projectile));

  double vec_Norm = vector_getNorm(speed);
  speed = vector_mul_factor(speed, speed_modifier);
  speed = vector_mul_factor(speed, 1.0/vec_Norm);

  // create the projectile's sprite
  newProj->an_sprite = animated_sprite_create(pixmap,position,xpm_count,time_between,false);

  newProj->affects_all = true;
  newProj->tracking_enemy = NULL;
  newProj->speed_modifier = speed_modifier;
  newProj->letter = 0;
  animated_sprite_change_vel(newProj->an_sprite, speed);

  
  return newProj;
}

bool projectile_affects_all(Projectile* proj) {
  return proj->affects_all;
}

int projectile_draw(Projectile* proj){

  return animated_sprite_draw(proj->an_sprite);
}

int projectile_update_speed(Projectile* proj){

  // make a vector that goes from the projectile to the enemy

  if(proj->tracking_enemy ==  NULL){
    return 0;
  }

  Sprite* tracking_en_sprite = proj->tracking_enemy->sprite;
  Vector enemy_center = {tracking_en_sprite->pos.x + floor(tracking_en_sprite->graphics.width/2.0),
                        tracking_en_sprite->pos.y + floor(tracking_en_sprite->graphics.height/2.0),
                        };
  Vector velocity = vector_make(proj->an_sprite->sprite_obj->pos, enemy_center);

  // normalize the vector
  double vec_Norm = vector_getNorm(velocity);
  velocity = vector_mul_factor(velocity,proj->speed_modifier);
  velocity = vector_mul_factor(velocity,1.0/vec_Norm);

  // update the projectile's speed
  animated_sprite_change_vel(proj->an_sprite,velocity);

  return 0;
}

int projectile_update(Projectile* proj){

 
  animated_sprite_update(proj->an_sprite);
  projectile_update_speed(proj);

  return 0;
}

bool proj_check_colisions(Projectile* proj, Enemy* enemy){

  return sprite_checkColision(proj->an_sprite->sprite_obj,enemy->sprite);

}

bool proj_check_out_of_bounds(Projectile* proj){

  return animated_sprite_check_out_of_bounds(proj->an_sprite);
  
}

int proj_destroy(Projectile* proj){

  animated_sprite_destroy(proj->an_sprite);
  free(proj);
  return 0;
}


