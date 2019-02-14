#include "wordWriter.h"

static xpm_image_t chars[NUM_CHARS];


int loadAlphabet() {

  xpm_image_t xpm_info;
  //xpm_image_t xpm_info_colored;
  uint8_t *validLoad;

  for (int i = 0; i < NUM_CHARS; i++) {

    validLoad = xpm_load(font[i], XPM_1_5_5_5, &xpm_info);

    if (validLoad == NULL)
      return -1;

    chars[i] = xpm_info;

  }

  return 0;
}


void deleteAlphabet(){

  for (int i = 0; i < NUM_CHARS; i++) {

    free(chars[i].bytes);

  }


}

int drawWord(char *word, uint16_t x, uint16_t y, uint16_t spacing) {

  int index, i = 0;
  char currentChar = word[i];
  uint16_t currentX = x; 
  xpm_image_t currentXPM;


  while (currentChar != '\0') {

    if (currentChar == ' ') 
      currentX += 4 * spacing;
    else {

      index = (int) currentChar - ASCII_TABLE_OFFSET;
      currentXPM = chars[index];
      draw_xpm(&currentXPM, currentX, y);
      currentX += spacing + currentXPM.width;
    }

    i++;
    currentChar = word[i];
   
  }

  return 0;
}


int getWordWidth(char* word, uint16_t spacing){

  int index,i = 0;
  char currentChar = word[i];
  uint16_t currentX = 0; 
  xpm_image_t currentXPM;

  while (currentChar != '\0') {

    if (currentChar == ' ') {

      currentX += 2 * spacing;
    }
    else {

      index = (int) currentChar - LETTER_BASE_ASCII;
      currentXPM = chars[index];
      currentX += spacing + currentXPM.width;
    }

    i++;
    currentChar = word[i];
  }

  return currentX;
}

int getAlphabetHeigth(){

  return CHAR_HEIGHT;
}

