#include "dl_math.h"

Vector vector_sum(Vector v1, Vector v2){

  Vector result = v1;

  result.x += v2.x;
  result.y += v2.y;

  return result;
}

Vector vector_make(Point p1, Point p2){

  Vector result = p2;

  result.x -= p1.x;
  result.y -= p1.y;

  return result;
}

double vector_getNorm(Vector v1){

  return sqrt(v1.x * v1.x + v1.y * v1.y);

}

Vector vector_mul_factor(Vector v, double m){

  Vector result = v;

  result.x = round(v.x*m);
  result.y = round(v.y*m);

  return result;
}

int loadTrig(){

  for(int i  = 0; i < 360; i++){

    cos_table[i] = cos(i*M_PI/180);
    sin_table[i] = cos(i*M_PI/180);
    if( (i != 90) && (i != 270))
      tan_table[i] = tan(i*M_PI/180);
    else
      tan_table[i] = 999999999999;

  }
  return 0;
}

int gcd(int A, int B){

  if (B == 0) 
        return A; 
    return gcd(B, A % B);  

}

Vector vector_create(int x, int y){

    Vector v = {x,y};
    return v;
}

// variable used in the generations of random numbers
uint32_t stat;

void seedRnd(uint32_t seed){

  stat = seed;

}

uint32_t xorshift32(uint32_t *state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return x;
}

int randomBetween(int a, int b){
  
  if(stat == 0)
    stat = 1;

  if(a == b){
    return a; 
  }
    
  
  return ( xorshift32(&stat) % (b - a + 1)) + a;
}

