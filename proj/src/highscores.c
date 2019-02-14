#include "highscores.h"

int load_highscores(Highscore *scores[]) {

  FILE *f;
  f = fopen(HIGHSCORES_FILE_PATH, "r");

  if (f == NULL) {
    
    return 1;
  }

  
  size_t j;

  for (j = 0; j < NUMBER_HIGHSCORES; j++) {

    
    scores[j] = malloc(sizeof(Highscore));

    if ((scores[j]->username[0] = fgetc(f)) == EOF)
      break;

    

    size_t i = 1;

    do {

      scores[j]->username[i] = fgetc(f);
      

      i++;

    } while (scores[j]->username[i - 1] != '@');

    

    scores[j]->username[i - 1] = 0;

    uint year = 0;
    uint month = 0;
    uint day = 0;
    uint hours = 0;
    uint minutes = 0;
    uint seconds = 0;

    fscanf(f, "%d %d %d %d %d %d %d\n", &scores[j]->score, &year, &month,
           &day, &hours, &minutes, &seconds);

    scores[j]->timestamp.year = (uint8_t) year;
    scores[j]->timestamp.month = (uint8_t) month;
    scores[j]->timestamp.day = (uint8_t) day;
    scores[j]->timestamp.hours = (uint8_t) hours;
    scores[j]->timestamp.minutes = (uint8_t) minutes;
    scores[j]->timestamp.seconds = (uint8_t) seconds;
  }

  

  for (; j < NUMBER_HIGHSCORES; j++) {
    scores[j] = NULL;
  }

  

  for (j = 0; j < NUMBER_HIGHSCORES; j++) {

    if (scores[j] == NULL)
      break;

   
  }

  

  fclose(f);

  return 0;
}

int store_highscores(Highscore *scores[]) {

  FILE *f = fopen(HIGHSCORES_FILE_PATH, "w");

  if (f == NULL) {
    return 1;
  }

  for (size_t j = 0; j < NUMBER_HIGHSCORES; j++) {

    if (scores[j] == NULL)
      break;

    fprintf(f, "%s@%d %d %d %d %d %d %d\n", scores[j]->username, scores[j]->score, scores[j]->timestamp.year, scores[j]->timestamp.month,
            scores[j]->timestamp.day, scores[j]->timestamp.hours, scores[j]->timestamp.minutes, scores[j]->timestamp.seconds);
  }

  fclose(f);

  return 0;
}

void insert_new_highscore(Highscore *scores[], char usrname[USERNAME_MAX_SIZE], uint32_t score) {

  struct RTCFields highscore_time;
  rtc_get_currentTime(&highscore_time);


  for (int8_t i = NUMBER_HIGHSCORES - 1; i >= 0; i--) {

    

    if (i == 0) {

      if (scores[i] == NULL)
        scores[i] = malloc(sizeof(Highscore));

      scores[i]->timestamp = highscore_time;
      strcpy(scores[i]->username, usrname);
      scores[i]->score = score;
      break;
    }

    if (scores[i] == NULL && scores[i - 1] != NULL) {

      if (score < scores[i - 1]->score) {

        scores[i] = malloc(sizeof(Highscore));

        scores[i]->timestamp = highscore_time;
        strcpy(scores[i]->username, usrname);
        scores[i]->score = score;

        break;
      }
      else {

        scores[i] = malloc(sizeof(Highscore));

        

        scores[i]->timestamp = scores[i - 1]->timestamp;

        

        scores[i]->score = scores[i - 1]->score;

        

        strcpy(scores[i]->username, scores[i - 1]->username);

        
      }
    }
    else if (scores[i] != NULL) {

      if (score > scores[i - 1]->score) {

        scores[i]->timestamp = scores[i - 1]->timestamp;
        scores[i]->score = scores[i - 1]->score;
        strcpy(scores[i]->username, scores[i - 1]->username);
      }
      else {

        scores[i]->timestamp = highscore_time;
        strcpy(scores[i]->username, usrname);
        scores[i]->score = score;

        break;
      }
    }
  }

  
}

uint32_t get_lowest_highscore(Highscore *scores[]) {

  // if there's no highscore in the last position of the table, return 0, else, return the score of the last highscore
  if (scores[NUMBER_HIGHSCORES - 1] == NULL)
    return 0;
  else
    return scores[NUMBER_HIGHSCORES - 1]->score;
}
