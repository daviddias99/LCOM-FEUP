#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC.
 * 
 */

#define BIT(n) (0x01<<(n))

#define KBD_IRQ 1 /**< @brief KEYBOARD IRQ line */
#define MS_IRQ 12 /**< @brief MOUSE IRQ line */
 
/* Useful scancodes */

#define ESC_BREAKCODE       0x81 /**< @brief Esc key breakcode */
#define CAPS_LCK_MAKECODE   0x3a /**< @brief Caps lock key makecode */
#define L_SHIFT_MAKECODE    0x2a /**< @brief Left shift key makecode */
#define L_SHIFT_BREAKCODE   0xaa /**< @brief Left shift key breakcode */
#define R_SHIFT_MAKECODE    0x36 /**< @brief Right shift key makecode */
#define R_SHIFT_BREAKCODE   0xb6 /**< @brief Right shift key breakcode */

#define TWOBYTE_SC_ID 0xe0 /**< @brief First byte of two byte scancodes */

#define DELAY_US 20000 /**< @brief Time used to wait for KBC and KBC comunication in microseconds */


/* I/O port addresses */

#define OUT_BUFFER   0x60 /**< @brief Output buffer register */
#define KBC_CMD_ARG  0x60 /**< @brief Input buffer register for command arguments */
#define STAT_REG     0x64 /**< @brief Status register */
#define KBC_CMD_REG  0x64 /**< @brief KBC command register */


/* Status byte */

#define OBF_FULL     BIT(0) /**< @brief Output buffer is full */
#define IBF_FULL     BIT(1) /**< @brief Input buffer is full */
#define SYS_FLAG     BIT(2) /**< @brief System flag */
#define A2_INPUT     BIT(3) /**< @brief A2 input line */
#define INH_FLAG     BIT(4) /**< @brief Inhibit flag */
#define MS_DATA      BIT(5) /**< @brief Mouse data */
#define TO_ERR       BIT(6) /**< @brief Timeout error */
#define PAR_ERR      BIT(7) /**< @brief Parity error */


/* Mouse packet bytes */

#define L_BUTTON     BIT(0) /**< @brief Pressed left button */
#define R_BUTTON     BIT(1) /**< @brief Pressed right button */
#define M_BUTTON     BIT(2) /**< @brief Pressed middle button */
#define X_SIGN       BIT(4) /**< @brief Sign of X deviation */
#define Y_SIGN       BIT(5) /**< @brief Sign of Y deviation */
#define X_OVF        BIT(6) /**< @brief X deviation overflow */
#define Y_OVF        BIT(7) /**< @brief Y deviation overflow */


/* KBC commands */

#define READ_CMD_BT   0x20 /**< @brief Read Command Byte */
#define WRITE_CMD_BT  0x60 /**< @brief Write Command Byte */
#define KBC_CHECK     0xAA /**< @brief Check KBC (Self-test) */
#define KBD_IF_CHECK  0xAB /**< @brief Check Keyboard Interface */
#define KBD_IF_EN     0xAE /**< @brief Enable Keyboard Interface */
#define KBD_IF_DIS    0xAD /**< @brief Disable Keyboard Interface */
#define MS_IF_CHECK   0xA9 /**< @brief Check Mouse Interface */
#define MS_EN         0xA7 /**< @brief Enable Mouse */
#define MS_DIS        0xA8 /**< @brief Disable Mouse */
#define MS_WRITE_BYTE 0xD4 /**< @brief Write byte to mouse */


/* Mouse commands */

#define MS_RESET      0xFF /**< @brief Mouse reset */
#define MS_RESEND     0xFE /**< @brief Mouse resend, for serial communcications errors */
#define SET_DEFAULT   0xF6 /**< @brief Set default values */
#define DIS_DATA_REP  0xF5 /**< @brief Disable data reporting */
#define EN_DATA_REP   0xF4 /**< @brief Enable data reporting */
#define SET_SPL_RATE  0xF3 /**< @brief Set state sampling rate */
#define SET_REMOTE_MD 0xF0 /**< @brief Send data on request only */
#define READ_DATA     0xEB /**< @brief Send data packet request */
#define SET_STREAM_MD 0xEA /**< @brief Send data on events */
#define STATUS_RQST   0xE9 /**< @brief Get mouse config */
#define SET_RES       0xE8 /**< @brief Set resolution */
#define SET_ACC_MODE  0xE7 /**< @brief Set scalling 2:1 */
#define SET_LIN_MODE  0xE6 /**< @brief Set scalling 1:1 */

/* Acknowledgements */

#define MS_BT_ACK   0xFA /**< @brief Everything OK after writting byte to mouse */
#define MS_BT_NACK  0xFE /**< @brief Invalid byte written to mouse */
#define MS_BT_ERR   0xFC /**< @brief Second consecutive invalid byte written to mouse */

/* KBC command byte */

#define DIS_MS        BIT(5) /**< @brief Disable Mouse */
#define DIS_KBD       BIT(4) /**< @brief Disable Keyboard Interface */
#define MS_INT_EN     BIT(1) /**< @brief Enable interrupt on OBF, from mouse */
#define KBD_INT_EN    BIT(0) /**< @brief Enable interrupt on OBF, from keyboard */

/* Events */

#define MOUSE_EVT     BIT(0) /**< @brief Bit set to one if there is a mouse event */


/** @} */

#endif /* _LCOM_I8042_H */
