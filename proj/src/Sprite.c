#include "Sprite.h"

Sprite *sprite_create(xpm_string_t *xpm, Vector position) {

  // alocate memory for a new sprite
  Sprite *newSprite = (Sprite *) malloc(sizeof(Sprite));

  // set the initial position and speed for the sprite
  Vector initial_speed = {0, 0};
  newSprite->pos = position;
  newSprite->speed = initial_speed;

  // load the xpm
  xpm_load(xpm, XPM_1_5_5_5, &newSprite->graphics);

  return newSprite;
}

Sprite * sprite_create_img(xpm_image_t pixmap, Vector position){

    // alocate memory for a new sprite
  Sprite *newSprite = (Sprite *) malloc(sizeof(Sprite));

  // set the initial position and speed for the sprite
  Vector initial_speed = {0, 0};
  newSprite->pos = position;
  newSprite->speed = initial_speed;

  // load the xpm
  newSprite->graphics = pixmap;

  return newSprite;
}

int sprite_change_graphics(Sprite* sprite, xpm_image_t xpm){

  sprite->graphics = xpm;
  return 0;
}

int sprite_change_vel(Sprite *sprite, Vector new_speed) {

  // change the sprites speedbool sprite_checkColision(Sprite* spriteA, Sprite* spriteB);
  sprite->speed = new_speed;

  return 0;
}

int sprite_change_pos(Sprite* sprite, Vector new_pos) {
  
    sprite->pos = new_pos;
    return 0;
}

int sprite_draw(Sprite *sprite) {

  // draw the xpm on the scene buffer
  draw_xpm(&sprite->graphics, (uint16_t) sprite->pos.x, (uint16_t) sprite->pos.y);
  return 0;
}

int sprite_update(Sprite *sprite) {

  // update the sprite's position using the speed vector
  sprite->pos = vector_sum(sprite->pos, sprite->speed);

  // draw the sprite
  sprite_draw(sprite);

  return 0;
}

bool sprite_checkColision(Sprite *spriteA, Sprite *spriteB) {

  Vector a_newPos = vector_sum(spriteA->pos, spriteA->speed);
  Vector b_newPos = vector_sum(spriteB->pos, spriteB->speed);

  uint16_t a_width = spriteA->graphics.width;
  uint16_t a_height = spriteA->graphics.height;

  uint16_t b_width = spriteB->graphics.width;
  uint16_t b_height = spriteB->graphics.height;

  Vector c_Pos = {0,0};
  Vector cb_rPos = {0,0};
  Vector ca_rPos = {0,0};



  uint16_t c_width = 0;
  uint16_t c_height = 0;

  Vector ab_V = {0,0};

  bool rectangle_overlay = false;

  // check "rectangle colisions"
  // fill in the information about the overllaping rectangle

  if ((a_newPos.x >= b_newPos.x && a_newPos.x <= b_newPos.x + b_width) && 
      (a_newPos.y >= b_newPos.y && a_newPos.y <= b_newPos.y + b_height)) {
    
    rectangle_overlay = true;

    ab_V =  vector_sum(a_newPos,vector_mul_factor(b_newPos,-1));

    c_Pos = a_newPos;
    cb_rPos = ab_V;

    ca_rPos = vector_create(0,0);

    if((a_newPos.y + a_height >= b_newPos.y && a_newPos.y + a_height <= b_newPos.y + b_height))
    {
      c_height = a_height;
    }
    else{

      c_height = b_newPos.y + b_height - a_newPos.y;
    }

    if((a_newPos.x+a_width >= b_newPos.x) && (a_newPos.x+a_width <= b_newPos.x+b_width) ){

      c_width = a_width;
    }
    else
      c_width = b_newPos.x + b_width - a_newPos.x;

   
  }

  else if ((a_newPos.x >= b_newPos.x && a_newPos.x <= b_newPos.x + b_width) && 
      (a_newPos.y + a_height >= b_newPos.y && a_newPos.y + a_height <= b_newPos.y + b_height)) {

    rectangle_overlay = true;

    ab_V =  vector_sum(a_newPos,vector_mul_factor(b_newPos,-1));

    c_Pos = vector_create(a_newPos.x,b_newPos.y);
    cb_rPos = vector_create(ab_V.x,0);

    ca_rPos = vector_create(0,b_newPos.y-a_newPos.y);

    if((a_newPos.y >= b_newPos.y && a_newPos.y <= b_newPos.y + b_height)){

      c_height = a_height;
    }else{

      c_height = a_height - (b_newPos.y-a_newPos.y);
    }

    if((a_newPos.x+a_width >= b_newPos.x) && (a_newPos.x+a_width <= b_newPos.x+b_width) ){

      c_width = a_width;
    }
    else
      c_width = b_newPos.x + b_width - a_newPos.x;
    
    
  }

  else if ((b_newPos.x >= a_newPos.x && b_newPos.x <= a_newPos.x + a_width) && 
      (b_newPos.y >= a_newPos.y && b_newPos.y <= a_newPos.y + a_height)) {
    
    rectangle_overlay = true;

    ab_V =  vector_sum(b_newPos,vector_mul_factor(a_newPos,-1));

    c_Pos = b_newPos;
    cb_rPos = vector_create(0,0);

    ca_rPos = ab_V;

    if((b_newPos.y + b_height >= a_newPos.y && b_newPos.y + b_height <= a_newPos.y + a_height)){

      c_height = b_height;

    }else{
      c_height = a_newPos.y + a_height - b_newPos.y;
    }

    if((b_newPos.x+b_width >= a_newPos.x) && (b_newPos.x+b_width <= a_newPos.x+a_width) ){

      c_width = b_width;
    }
    else
      c_width = a_newPos.x + a_width - b_newPos.x;

  }

  else if ((b_newPos.x >= a_newPos.x && b_newPos.x <= a_newPos.x + a_width) && 
      (b_newPos.y + b_height >= a_newPos.y && b_newPos.y + b_height <= a_newPos.y + a_height)) {

    rectangle_overlay = true;

    ab_V =  vector_sum(b_newPos,vector_mul_factor(a_newPos,-1));

    c_Pos = vector_create(b_newPos.x,a_newPos.y);
    cb_rPos = vector_create(0,a_newPos.y-b_newPos.y);

    ca_rPos = vector_create( ab_V.x,0);

    if(b_newPos.y >= a_newPos.y && b_newPos.y <= a_newPos.y + a_height){

      c_height = b_height;
    }else{

      c_height = b_height - (a_newPos.y-b_newPos.y);
    }

    if((b_newPos.x+b_width >= a_newPos.x) && (b_newPos.x+b_width <= a_newPos.x+a_width) ){

      c_width = b_width;
    }
    else
      c_width = a_newPos.x + a_width - b_newPos.x;

   
    
  }

  // check pixel colisions

  if (rectangle_overlay) {

    for(int i = 0; i < c_height; i++){

      for(int j = 0; j < c_width;j++){

        uint16_t pixelA = xpm_pixel_at(spriteA->graphics,ca_rPos.x+j,ca_rPos.y+i);
        uint16_t pixelB = xpm_pixel_at(spriteB->graphics,cb_rPos.x+j,cb_rPos.y+i);

        if( ( (pixelA & BIT(15)) == 0) && ((pixelB & BIT(15)) == 0)){
          
          return true;
        }
          
      }
    }
    return false;
  }
  else
    return false;
}

bool sprite_check_out_of_bounds(Sprite *sprite) {

  return xpm_check_out_of_bounds(&sprite->graphics, sprite->pos.x, sprite->pos.y);
}

bool sprite_check_bound_crossing(Sprite* sprite, uint8_t boundMask){

    Vector sp_newPos = vector_sum(sprite->pos, sprite->speed);

    if( boundMask & BIT(0)){

      if(sp_newPos.y + sprite->graphics.height >= get_vres())
        return true;

    }
    if( boundMask & BIT(1)){
      
      if(sp_newPos.x + sprite->graphics.width >= get_hres())
        return true;

    }
    if( boundMask & BIT(2)){


      if(sp_newPos.y < 0)
        return true;

    }
    if( boundMask & BIT(3)){


      if(sp_newPos.x < 0)
        return true;

    }

    return false;

}

int sprite_destroy(Sprite* sprite){

  free(sprite);
  return 0;
}


