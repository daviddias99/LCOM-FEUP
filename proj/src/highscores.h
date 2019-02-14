#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <lcom/lcf.h>

#include "rtc.h"

/** @defgroup Highscores Highscores
 * @{
 * Functions to display,load and manipulate highscores
 */

#define HIGHSCORES_FILE_PATH "/home/lcom/labs/proj/highscores.txt" /**< @brief Path of the text file containing the highscores */
#define USERNAME_MAX_SIZE 10  /**< @brief Max size of the username char arry */
#define NUMBER_HIGHSCORES 5  /**< @brief Max number of highscores to store */

typedef struct Highscore {

  struct RTCFields timestamp;
  char username[USERNAME_MAX_SIZE + 1];
  uint32_t score;

} Highscore;

/**
 * @brief Loads the highscores in the HIGHSCORES_FILE_PATH to the array of highscores passed as argument
 * 
 * @param   scores  Array of highscores to be written in the file
 * @return  int     Zero if successful, non-zero otherwise
 */
int load_highscores(Highscore *scores[]);

/**
 * @brief Writes the highscores in the HIGHSCORES_FILE_PATH file, in the format: USERNAME(at)score year month day hours minutes seconds
 * 
 * @param   scores  Array of highscores to be written in the file
 * @return  int     Zero if successful, non-zero otherwise
 */
int store_highscores(Highscore *scores[]);

/**
 * @brief Inserts a new highscore in the highscores array passed as argument, with the time of the highscore being the current time of the RTC
 * 
 * @param scores    Array of that points to the highscores
 * @param usrname   Name of the username who got the new highscore
 * @param score     Score of the new highscore
 */
void insert_new_highscore(Highscore *scores[], char usrname[USERNAME_MAX_SIZE], uint32_t score);

/**
 * @brief Get the lowest highscore in the array of highscores passed as argument
 * 
 * @param   scores    Array of pointers to highscores 
 * @return  uint32_t  Value of the lowest highscore, if there is an empty spot in the highscore array, returns 0
 */
uint32_t get_lowest_highscore(Highscore *scores[]);

/**@} */

#endif
