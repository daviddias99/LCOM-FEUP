#ifndef VIDEOCARD_H
#define VIDEOCARD_H

#include "vbe_macro.h"
#include "videocard_util.h"



enum mov_type {left = 0, right, down, up};

/**
 * @brief initializes the minix graphic mode as the specified mode
 * 
 * @param mode    vbe graphic mode
 * @return        0 if initalization sucessful, different from 0 otherwise
 */
int init_vbe_mode(uint16_t mode);

/**
 * @brief separate the first color into it's color values
 * 
 * @param first     color that is decomposed
 * @param fRed      pointer to a varible where the red component should be stored
 * @param fGreen    pointer to a varible where the green component should be stored
 * @param fBlue     pointer to a varible where the blue component should be stored
 */
void getColorValues(uint32_t first, uint32_t* fRed,  uint32_t* fGreen, uint32_t* fBlue);

/**
 * @brief apply the given formulas to obtain the value of the color of a specified rectangle
 * 
 * @param mode                  graphic mode on which to operate
 * @param no_rectangles         number of rectangles in each direction
 * @param row                   row of the rectangle
 * @param column                column of the rectangle
 * @param first                 color to be used in the first rectangle (the rectangle at the top-left corner of the screen). 
 * @param step 
 * @return                      color of the rectangle
 */
uint32_t getColor(uint16_t mode, uint8_t no_rectangles, uint8_t row, uint8_t column, uint32_t first, uint8_t step);

/**
 * @brief get the horizontal resolution of the graphic window
 * 
 * @return horizontal resolution of the graphic window
 */
uint16_t getHRes();  

/**
 * @brief get the vertical resolution of the graphic window
 * 
 * @return vertical resolution of the graphic window
 */
uint16_t getVRes();  

/**
 * @brief get the number of bits per pixel
 * 
 * @return bits per pixel
 */
unsigned int getBitsPP();

/**
 * @brief get the number of bytes per pixel
 * 
 * @return bytes per pixel
 */
unsigned int getBytesPP();

/**
 * @brief get the pointer to the video memory buffer
 * 
 * @return char* 
 */
char* getVideo_mem();


/**
 * @brief convert the given xpm to a pixmap and draw it with the top left
 * corner in the specified xy position
 * 
 * @param xpm       xpm to draw
 * @param xi        x coordinate of the top left corner of the xpm
 * @param yi        y coordinate of the top left corner of the xpm
 * @return          0 if sucessfull, non-0 otherwise 
 */
int draw_xpm(const char *xpm[], uint16_t xi, uint16_t yi);



void clear_screen();

/**
 * @brief execute VBE BIOS call 0x00 in order to obtain the controller
 * info and put the relevant information in the controller info struct
 * 
 * @param vci_p         pointer to a struct that should contain the relevant controller info
 * @return              0 if sucessfull, non-0 otherwise 
 */
int vbe_controller_info_get(vg_vbe_contr_info_t *vci_p);

#pragma pack(1)

typedef struct Mode_Info_Block {
  char VBESignature[4];    /**< @brief VBE Signature */
  BCD VBEVersion[2];       /**< @brief VBE Version,  */
  uint32_t OEMString;         /**< @brief OEM String */
  uint32_t Capabilities;
  uint32_t VideoModeList; /**< @brief Address of first element in an array of VBE modes supported. 
                                       Last element must have value 0xFF.*/
  uint16_t TotalMemory;    /**< @brief Total Memory in KB */
  uint16_t OEMSoftwareRev;
  uint32_t OEMVendorNamePtr;  /**< @brief OEM Vendor Name string */
  uint32_t OEMProductNamePtr; /**< @brief OEM Product Name string */
  uint32_t OEMProductRevPtr;  /**< @brief OEM Product Revision string */
  uint8_t Reserved[222];
  uint8_t OemData[256];
} Mode_Info_Block_t;

#pragma options align = reset

void clearScreen();

#endif
