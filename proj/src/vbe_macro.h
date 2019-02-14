#ifndef VBE_MACRO_H_
#define VBE_MACRO_H_ 


/** @defgroup Vbe_macro Vbe_macro
 * @{
 *
 * Constants for working with VBE and the video card.
 */


#ifndef BIT
#define BIT(n)                (0x01<<(n))
#endif
#define SECONDS_TO_MICROS(n)  (1000000 * n) /**< @brief conversion between seconds and microseonds */
#define BIOS_VC_SER           0x10          /**< @brief BIOS videocard service code */
#define VBE_ID                0x4F          /**< @brief BIOS VBE identifier */
#define LIN_FR_BUFF           BIT(14)       /**< @brief Linear frame buffer ID */
#define BITS_PER_BYTE         8             /**< @brief Number of bytes per pixel */


#define VBE_GET_CTLR_INFO     0x00          /**< @brief Get controller info vbe function */
#define VBE_GET_MODE_INFO     0x01          /**< @brief Get mode info vbe function */
#define VBE_SET_MODE          0x02          /**< @brief Set mode  vbe function */
#define VGC_SET_TEXT_MODE     0x03          /**< @brief Set text-mode  vbe function */

#define INDEXED_COLOR_MODE    0x105         /**< @brief Indexed color mode */
#define PROJECT_COLOR_MODE    0x116         /**< @brief Color mode used in the project.(16 bit direct mode with(1 5 5 5) 1024x768 resolution) */                 

/**@} */

#endif
