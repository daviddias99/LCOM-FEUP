#ifndef UART_H
#define UART_H

/** @defgroup UART UART
 * @{
 *
 * Symbolic constants used to work with the UART serial port 
 */

#ifndef BIT
#define BIT(n) (0x01<<(n))
#endif

#define MAX_TRIES_POLLING 10 /**< @brief Max number of tries to send a char in polled mode */

#define UART_COM1_IRQ  4  /**< @brief UART IRQ line */

#define UART_BIT_RATE_DIVISOR   115200 /**< @brief Divisor used to calculate the value for the Divisor Latch registers */

#define COM1_BASE_ADDRESS   0x3F8 /**< @brief Base address of the first UART */
#define COM2_BASE_ADDRESS   0x2F8 /**< @brief Base address of the second UART */

/* REGISTER OFFSET */

#define RBR_ADDRESS   0 /**< @brief Offset of the Reiceiver Buffer Register */
#define THR_ADDRESS   0 /**< @brief Offset of the Transmitter Holding Register */
#define IER_ADDRESS   1 /**< @brief Offset of the Interrupt Enable Register */
#define IIR_ADDRESS   2 /**< @brief Offset of the Interrupt Identification Register */
#define FCR_ADDRESS   2 /**< @brief Offset of the FIFO Control Register */
#define LCR_ADDRESS   3 /**< @brief Offset of the Line Control Register */
#define LSR_ADDRESS   5 /**< @brief Offset of the Line Status Register */

#define DLL_ADDRESS   0 /**< @brief Offset of the Divisor Latch LSB register */
#define DLM_ADDRESS   1 /**< @brief Offset of the Divisor Latch MDB register */

/* LCR */

#define LCR_DLAB      BIT(7) /**< @brief Bit of the LCR that allows DL access when set to one */

#define UART_PARITY_NONE    0 /**< @brief Mask used to set the parity to none */
#define UART_PARITY_ODD     BIT(3) /**< @brief Mask used to set the parity to odd */
#define UART_PARITY_EVEN    (BIT(3) | BIT(4)) /**< @brief Mask used to set the parity to even */
#define UART_PARITY_ZERO    (BIT(3) | BIT(4) | BIT(5)) /**< @brief Mask used to set the parity to zero */
#define UART_PARITY_ONE     (BIT(3) | BIT(5)) /**< @brief Mask used to set the parity to one */

/* LSR */
#define LSR_RECEIVER_DATA   BIT(0) /**< @brief Set to one when there's data for receiving */
#define LSR_THR_EMPTY       BIT(5) /**< @brief Bit ot the LSR that means that the UART is ready to transmitt if set to one */

/* IER */

#define UART_ENABLE_RECEIVED_DATA_INT     BIT(0) /**< @brief Enables the Received Data Available interrupt when set */

/* IIR */

#define UART_INT_PEND                     BIT(0) /**< @brief If set, no interrupt is pending */
#define UART_INT_ID                       (BIT(1) | BIT(2) | BIT(3)) /**< @brief Bits containing the interrupt identification */
#define UART_INT_MODEM_ST                 0x00 /**< @brief Modem Status interrupt */
#define UART_INT_THR_EMPTY                0x02 /**< @brief THR empty interrupt */
#define UART_INT_CHAR_TO_FIFO             0x0C /**< @brief Character Timeout (FIFO) interrupt */
#define UART_INT_DATA_AVAILABLE           0x04 /**< @brief Received Data Available interrupt */ 
#define UART_INT_RECEIVER_LINE_STATUS     0x06 /**< @brief Receiver Line Status interrupt */

/** @} */

#endif
