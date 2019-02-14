#ifndef VIDEOCARD_H
#define VIDEOCARD_H

#include "debug.h"
#include "vbe_macro.h"
#include "videocard_util.h"


/** @defgroup Videocard Videocard
 * @{
 * Functions to handle and manipulate the video memory 
 */

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
 * corner in the specified xy position (of the video_mem)
 * 
 * @param xpm       xpm to draw
 * @param xi        x coordinate of the top left corner of the xpm
 * @param yi        y coordinate of the top left corner of the xpm
 * @return          0 if sucessfull, non-0 otherwise 
 */
int draw_xpm_legacy(const char *xpm[], uint16_t xi, uint16_t yi);


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


/**
 * @brief Sets the value of all pixels in the video mem as 0
 * 
 */
void clear_screen();

/**
 * @brief This function draw's the given xpm (stored in a xpm_image_t format) in the scene buffer. It supports drawing a xpm only partially on the screen(edge cases)
 * 
 * @param xpm             pointer to the sprite to be drawn in the scene
 * @param xi              horizontal position of the top left corner
 * @param yi              vertical position of the top left corner
 * @return int            zero if sucessful, non-zero otherwise
 */
int draw_xpm(xpm_image_t* xpm, int16_t xi, int16_t yi);

/**
 * @brief Returns an array of the pixels contained in rectangular segment of the video memory. WARNING: Does not support edge cases (cases where only a part of the area given is inside the visible area of the video memory)
 * 
 * @param x               horizontal position of the top left corner
 * @param y               vertical position of the top left corner
 * @param width           width of the segment
 * @param height          height of the segment
 * @return uint16_t*      pointer to an array of the pixels of the selected area of the video memory
 */
uint16_t *  getSegment(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * @brief Loads a xpm_string_t into a xpm_image_t and stores it as the scene's background
 * 
 * @param bg               pointer to a xpm
 * @return int             zero if sucessful, non-zero otherwise
 */
int load_background(xpm_string_t* bg);

/**
 * @brief Draw's the background stores in the background variable (see load_background()) in the scene buffer
 * 
 * @return int              zero if sucessful, non-zero otherwise
 */
int draw_background();

/**
 * @brief Draw's the given xpm as the background of the scene buffer. (WARNING: must have appropriate dimensions)
 * 
 * @param bg              pointer to a xpm
 * @return int            zero if sucessful, non-zero otherwise
 */
int draw_background2(xpm_image_t bg);

/**
 * @brief Draws the scene buffer in the video memory buffer. This allows for a implementation of a pseudo-double buffering to avoid flickering while the video_mem is being presented at an quick rate
 * 
 * @return int            zero if sucessful, non-zero otherwise
 */
int display_scene();

/**
 * @brief Get the scene buffer pointer
 * 
 * @return char*          scene buffer pointer
 */
char* get_scene();

/**
 * @brief Get the horizontal resolution of the video memory
 * 
 * @return uint16_t       horizontal resolution of the video memory
 */
uint16_t get_hres();

/**
 * @brief Get the vertical resolution of the video memory
 * 
 * @return uint16_t       vertical resolution of the video memory
 */
uint16_t get_vres();

/**
 * @brief Get the background variable 
 * 
 * @return xpm_image_t    background variable 
 */
xpm_image_t get_background();

/**
 * @brief Returns the value of a pixel at the given position in a given xpm. WARNING: only works for double byted pixels
 * 
 * @param xpm             pointer to the xpm_image_t to check
 * @param x               x position of the pixel
 * @param y               y position of the pixel
 * @return uint16_t       value of the pixel
 */
uint16_t xpm_pixel_at(xpm_image_t xpm, uint16_t x, uint16_t y);
/**
 * @brief Check if a given xpm is out of the bounds of the screen
 * 
 * @param xpm             pointer to the xpm_image_t to check
 * @param x               x position of the top left corner of the xpm
 * @param y               y position of the top left corner of the xpm
 * @return true           if the xpm is out of bounds
 * @return false          if the xpm isn't ouf of bounds
 */
bool xpm_check_out_of_bounds(xpm_image_t* xpm, int16_t x, int16_t y);
/**
 * @brief Draws the contents of the aux_buffer in the scene buffer 
 * 
 * @return int Zero if succesful, non-zero otherwise
 */
int draw_aux_buffer();
/**
 * @brief Moves the contents of the scene to the aux_buffer
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int scene_to_aux_buffer();
/**@} */
#endif
