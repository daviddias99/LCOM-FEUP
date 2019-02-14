#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC.
 * 
 */

#define BIT(n) (0x01<<(n))

#define KBC_IRQ 1 /**< @brief KBC IRQ line */
 
#define ESC_BREAKCODE 0x81 /**< @brief Esc key breakcode */

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

/* KBC Commands */
#define READ_CMD_BT   0x20  /**< @brief Read Command Byte */
#define WRITE_CMD_BT  0x60  /**< @brief Write Command Byte */
#define KBC_CHECK     0xAA  /**< @brief Check KBC (Self-test) */
#define KBD_IF_CHECK  0xAB  /**< @brief Check Keyboard Interface */
#define KBD_IF_EN     0xAE  /**< @brief Enable Keyboard Interface */
#define KBD_IF_DIS    0xAD  /**< @brief Disable Keyboard Interface */

/* KBC Commands */
#define DIS_MS        BIT(5) /**< @brief Disable Mouse */
#define DIS_KBD       BIT(4) /**< @brief Disable Keyboard Interface */
#define MS_INT_EN     BIT(1) /**< @brief Enable interrupt on OBF, from mouse */
#define KBD_INT_EN    BIT(0) /**< @brief Enable interrupt on OBF, from keyboard */


#endif /* _LCOM_I8042_H */
