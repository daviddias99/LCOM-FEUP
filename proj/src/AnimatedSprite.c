#include "AnimatedSprite.h"

AnimatedSprite *animated_sprite_create(xpm_image_t *pixmap, Vector position,int xpm_count, int time_between,bool dead_after_one){

  // alocate memory for a new an_sprite
  AnimatedSprite* newAnSprite = (AnimatedSprite*) malloc(sizeof(AnimatedSprite));

  newAnSprite->sprite_obj = sprite_create_img(pixmap[0],position);
  newAnSprite->time_between_frames = time_between;
  newAnSprite->time_until_next_frame = time_between;
  newAnSprite->xpm_count = xpm_count;
  newAnSprite->xpm_current = 0;
  newAnSprite->xpm_db = pixmap;
  newAnSprite->destroy_after_cycle = dead_after_one;

  return newAnSprite;

}

int animated_sprite_destroy(AnimatedSprite* an_sprite){

  free(an_sprite->sprite_obj);
  free(an_sprite);

  return 0;
}

int animated_sprite_draw(AnimatedSprite *an_sprite) {

  sprite_draw(an_sprite->sprite_obj);
  return 0;
}

int animated_sprite_update(AnimatedSprite *an_sprite) {

  if(an_sprite->xpm_current == -1)
    return 0;

  Sprite* sprite = an_sprite->sprite_obj;

  // update the sprite's position using the speed vector
  sprite->pos = vector_sum(sprite->pos, sprite->speed);

  // draw the sprite
  sprite_draw(sprite);
  an_sprite->time_until_next_frame--;

  if(an_sprite->time_until_next_frame == 0){

    an_sprite->xpm_current++;

    if(an_sprite->xpm_current == an_sprite->xpm_count){

      if(an_sprite->destroy_after_cycle){

          //animated_sprite_destroy(an_sprite);
          an_sprite->xpm_current = -1;
      }
      else
        an_sprite->xpm_current = 0;

    }

    an_sprite->time_until_next_frame = an_sprite->time_between_frames;
    sprite_change_graphics(sprite,an_sprite->xpm_db[an_sprite->xpm_current]);

  }

  return 0;
}

int animated_sprite_change_vel(AnimatedSprite* an_sprite, Vector new_speed) {

  return sprite_change_vel(an_sprite->sprite_obj,new_speed);
}

int animated_sprite_change_pos(AnimatedSprite* an_sprite, Vector new_pos) {

  return sprite_change_pos(an_sprite->sprite_obj,new_pos);
}

bool animated_sprite_checkColision(AnimatedSprite* spriteA, AnimatedSprite* spriteB){

  return sprite_checkColision(spriteA->sprite_obj,spriteB->sprite_obj);
}

bool animated_sprite_check_out_of_bounds(AnimatedSprite* an_sprite){

  return sprite_check_out_of_bounds(an_sprite->sprite_obj);
}

bool animated_sprite_check_bound_crossing(AnimatedSprite* an_sprite, uint8_t boundMask){

  return sprite_check_bound_crossing(an_sprite->sprite_obj,boundMask);
}

