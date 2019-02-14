#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <lcom/lcf.h>
#include "uart.h"
#include "debug.h"

/** @defgroup Serialport Serialport
 * @{
 * Functions used to manipulate de UART.
 */

/**
 * @brief Modes of parity, used for error detection, in which the serial port can operate
 */
enum uart_parity { PRT_NONE = 0, PRT_ODD, PRT_EVEN, PRT_ONE, PRT_ZERO };
/**
 * @brief Subscribes uart interrupts
 * 
 * @param bit_no Number of the bit that is set in the mask used to identify interrupts
 * @return int Zero if successful, non-zero otherwise
 */
int uart_subscribe_int(uint8_t* bit_no);
/**
 * @brief Unsubscribes uart interrupts
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_unsubscribe_int();
/**
 * @brief Reads the RBR register 
 * 
 * @param data Pointer to an 8 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_RBR(uint8_t *data);
/**
 * @brief Reads the IER register 
 * 
 * @param data Pointer to an 8 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_IER(uint8_t *data);
/**
 * @brief Reads the IIR register 
 * 
 * @param data Pointer to an 8 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_IIR(uint8_t *data);
/**
 * @brief Reads the LCR register 
 * 
 * @param data Pointer to an 8 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_LCR(uint8_t *data);
/**
 * @brief Reads the LSR register 
 * 
 * @param data Pointer to an 8 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_LSR(uint8_t *data);
/**
 * @brief Reads the contents of both DL registers
 * 
 * @param data Pointer to a 16 bit int where the read data is store
 * @return int Zero if successful, non-zero otherwise
 */
int uart_read_DL(uint16_t *data);
/**
 * @brief Writes data to the THR register
 * 
 * @param data Data to be written in the register
 * @return int Zero if successful, non-zero otherwise
 */
int uart_write_THR(uint8_t data);
/**
 * @brief Writes data to the LCR register
 * 
 * @param data Data to be written in the register
 * @return int Zero if successful, non-zero otherwise
 */
int uart_write_LCR(uint8_t data);
/**
 * @brief Writes data to the DLM register
 * 
 * @param dlm Data to be written in the register
 * @return int Zero if successful, non-zero otherwise
 */
int uart_write_DLM(uint8_t dlm);
/**
 * @brief Writes data to the DLL register
 * 
 * @param dll Data to be written in the register
 * @return int Zero if successful, non-zero otherwise
 */
int uart_write_DLL(uint8_t dll);
/**
 * @brief Configures the number of bits per character used by the UART
 * 
 * @param bpc Number of bits per char
 * @return int Zero if successful, non-zero otherwise
 */
int uart_set_bits_per_char(uint8_t bpc);
/**
 * @brief Configures the number of stop bits used by the UART
 * 
 * @param stop_bits Number of stop bits
 * @return int Zero if successful, non-zero otherwise
 */
int uart_set_no_stop_bits(uint8_t stop_bits);
/**
 * @brief Configures the bitrate used by the UART
 * 
 * @param bitrate Bit-rate we want the UART to use
 * @return int Zero if successful, non-zero otherwise
 */
int uart_set_bit_rate(int bitrate);
/**
 * @brief Configures the parity used by the UART for error detection
 * 
 * @param prt Type of parity to use
 * @return int Zero if successful, non-zero otherwise
 */
int uart_set_parity(enum uart_parity prt);
/**
 * @brief Disables Received Data Available interrupts
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int disable_received_data_int();
/**
 * @brief Enables Received Data Available interrupts
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int enable_received_data_int();
/**
 * @brief UART interrupt handler to use only with received data available interrupts
 */
void uart_ih();
/**
 * @brief Sends a char using polling
 * 
 * @param data Char to be sent
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_char(uint8_t data);
/**
 * @brief Get the last read char by the UART interrupt handler
 * 
 * @return uint8_t Last read char by the UART interrupt handler
 */
uint8_t get_last_received_char();


/* COMMUNICATION PROTOCOL */

/**
 * @brief Types of messages used in the communication protocol
 */
enum uart_msg_type { ACK_JOIN = 0,  /**< Acknowledge that the secondary player joined the game */
                     JOIN, /**< Second player tries to join the game */
                     LEAVE, /**< Second player leaves the game */
                     P1_LEAVE, /**< First player leaves the game */
                     KBD_CHAR, /**< Input char from the secondary player to the first player */
                     ADD_CHAR, /**< New char to add to the secondary game */
                     REM_CHAR, /**< Char to remove from the secondary game */
                     TRAILER_ID /**< Last char of the message */
                     };

/** @name uart_msg */
/** 
 * @{
 * 
 * @brief Struct that contains the info related to a serial port message
 */
struct uart_msg {

  enum uart_msg_type type;
  char bytes[3];
};
/** @} */


/* MESSAGE HEADERS */

#define ACK_JOIN_MSG 0xFA /**< Header of the ACK_JOIN message */
#define JOIN_MSG 0xFB     /**< Header of the JOIN message */
#define LEAVE_MSG 0xFC    /**< Header of the LEAVE message */
#define KBD_CHAR_MSG 0xFD /**< Header of the KBD_CHAR message */
#define ADD_CHAR_MSG 0xEF /**< Header of the ADD_CHAR message */
#define REM_CHAR_MSG 0xFE /**< Header of the REM_CHAR message */
#define P1_LEAVE_MSG 0xEE /**< Header of the P1_LEAVE message */
#define TRAILER_ID 0xFF   /**< Trailer char of each message */


/**
 * @brief Sends an ACK_JOIN message 
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_ack_join_msg();
/**
 * @brief Sends a JOIN message 
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_join_msg();
/**
 * @brief Sends a LEAVE message 
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_leave_msg();
/**
 * @brief Sends a P1_LEAVE message 
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_p1_leave();
/**
 * @brief Sends a KBD_CHAR message 
 * 
 * @param kbd_char Char that's part of the message body
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_kbd_char(uint8_t kbd_char);
/**
 * @brief Sends a ADD_CHAR message 
 * 
 * @param kbd_char Char that's part of the message body
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_add_char(uint8_t kbd_char);
/**
 * @brief Sends a REM_CHAR message 
 * 
 * @param kbd_char Char that's part of the message body
 * @return int Zero if successful, non-zero otherwise
 */
int uart_send_rem_char(uint8_t kbd_char);

/**
 * @brief Checks the last char read by the ih and fills the msg_struct as needed by identifying the type of message, puts the byte in the msg_struct if the message has a body and checking if the msg is complete
 */
void uart_parse_msg();
/**
 * @brief Returns the last message read by the serial port
 * 
 * @return struct uart_msg* Last message read by the serial port
 */
struct uart_msg* uart_get_last_msg();
/**
 * @brief Checks if the last char read by the interrupt handler completed a message
 * 
 * @return true if the message is complete, false otherwise 
 */
bool is_msg_complete();
/**
 * @brief Configures the UART as desired for use in the project
 * 
 * @return int Zero if successful, non-zero otherwise
 */
int uart_init();

#endif
