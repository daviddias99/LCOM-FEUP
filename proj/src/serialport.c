#include "serialport.h"

static int hookID = 16; /**< @brief Bit that is set on the interrupt message when there's a UART interrupt */

static uint8_t lastChar = 0; /**< @brief Last char read by the UART interrupt handler */

/**
 * @brief Reads data from a UART register
 * 
 * @param port Register to read
 * @param data Data read from the register
 * @return int Zero if successful, non-zero otherwise
 */
static int read_reg(uint32_t port, uint8_t *data) {

  uint32_t aux = 0;

  if (sys_inb(port, &aux))
    return 1;

  *data = (uint8_t) aux;

  return 0;
}
/**
 * @brief Writes data to a UART register
 * 
 * @param port Register to write
 * @param data Data to write in the register
 * @return int Zero if successful, non-zero otherwise
 */
static int write_reg(uint32_t port, uint8_t data) {

  return sys_outb(port, data);
}

int uart_subscribe_int(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(UART_COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID);
}
int uart_unsubscribe_int() {

  return sys_irqrmpolicy(&hookID);
}

int uart_read_RBR(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + RBR_ADDRESS, data);
}
int uart_read_IER(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + IER_ADDRESS, data);
}
int uart_read_IIR(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + IIR_ADDRESS, data);
}
int uart_read_LCR(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + LCR_ADDRESS, data);
}
int uart_read_DL(uint16_t *data) {

  uint8_t lcr_status;

  uart_read_LCR(&lcr_status);

  uart_write_LCR(lcr_status | LCR_DLAB); // set to one Divisor Latch Access Bit

  uint8_t dll = 0;
  read_reg(COM1_BASE_ADDRESS + DLL_ADDRESS, &dll);
  uint8_t dlm = 0;
  read_reg(COM1_BASE_ADDRESS + DLM_ADDRESS, &dlm);
  uint16_t dl = (dlm << 8) | dll;

  *data = dl;

  uart_write_LCR(lcr_status); // revert the LCR register to its original value

  return 0;
}
int uart_read_LSR(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + LSR_ADDRESS, data);
}

int uart_write_FCR(uint8_t *data) {
  return read_reg(COM1_BASE_ADDRESS + FCR_ADDRESS, data);
}
int uart_write_THR(uint8_t data) {
  return write_reg(COM1_BASE_ADDRESS + THR_ADDRESS, data);
}
int uart_write_LCR(uint8_t data) {
  return write_reg(COM1_BASE_ADDRESS + LCR_ADDRESS, data);
}
int uart_write_DLM(uint8_t dlm) {
  return write_reg(COM1_BASE_ADDRESS + DLM_ADDRESS, dlm);
}
int uart_write_DLL(uint8_t dll) {
  return write_reg(COM1_BASE_ADDRESS + DLL_ADDRESS, dll);
}
int uart_write_IER(uint8_t data) {
  return write_reg(COM1_BASE_ADDRESS + IER_ADDRESS, data);
}

int uart_set_bits_per_char(uint8_t bpc) {

  if ((bpc < 5) || (bpc > 8))
    return 1;

  uint8_t lcr_data;

  uart_read_LCR(&lcr_data);

  bpc -= 5;

  uart_write_LCR((lcr_data & ~(BIT(0) | BIT(1))) | bpc);

  return 0;
}
int uart_set_no_stop_bits(uint8_t stop_bits) {

  uint8_t lcr_data;

  uart_read_LCR(&lcr_data);

  switch (stop_bits) {
    case 1:
      lcr_data = lcr_data & (~BIT(2));
      break;
    case 2:
      lcr_data = lcr_data | BIT(2);
      break;
    default:
      return 1;
  }

  uart_write_LCR(lcr_data);

  return 0;
}
int uart_set_parity(enum uart_parity prt) {

  uint8_t lcr_data;

  uart_read_LCR(&lcr_data);

  // toggle to 0 the bits regarding the parity
  lcr_data = lcr_data & (~(BIT(3) | BIT(4) | BIT(5)));

  switch (prt) {
    case PRT_NONE:
      lcr_data |= UART_PARITY_NONE;
      break;
    case PRT_ODD:
      lcr_data |= UART_PARITY_ODD;
      break;
    case PRT_EVEN:
      lcr_data |= UART_PARITY_EVEN;
      break;
    case PRT_ONE:
      lcr_data |= UART_PARITY_ONE;
      break;
    case PRT_ZERO:
      lcr_data |= UART_PARITY_ZERO;
      break;
  }

  uart_write_LCR(lcr_data);

  return 0;
}
int uart_set_bit_rate(int bitrate) {

  uint8_t lcr_status;

  uart_read_LCR(&lcr_status);

  uart_write_LCR(lcr_status | LCR_DLAB); // set to one Divisor Latch Access Bit

  uint16_t dl = UART_BIT_RATE_DIVISOR / bitrate;
  uint8_t dlm = 0;
  uint8_t dll = 0;

  dlm = dl << 8;
  dll = (uint8_t)(0x00ff & dl);

  uart_write_DLM(dlm);
  uart_write_DLL(dll);
  uart_write_LCR(lcr_status); // revert the LCR register to its original value

  return 0;
}

int enable_received_data_int() {

  uint8_t ier = 0;

  uart_read_IER(&ier);

  ier |= BIT(0);
  ier = ier & (~(BIT(1) | BIT(2) | BIT(3)));

  uart_write_IER(ier);

  return 0;
}

int disable_received_data_int() {

  uint8_t ier = 0;

  uart_read_IER(&ier);

  ier &= ~BIT(0);

  uart_write_IER(ier);

  return 0;
}

void uart_ih() {

  uint8_t iir;

  uart_read_IIR(&iir);

  if (!(iir & UART_INT_PEND)) {

    switch (iir & UART_INT_ID) {

      case UART_INT_RECEIVER_LINE_STATUS:
        break;
      case UART_INT_DATA_AVAILABLE:

        uart_read_RBR(&lastChar);

        break;
      default:
        break;
    }
  }
}

static int current_byte = 0;
static bool uart_msg_complete;
static struct uart_msg current_msg;

int uart_send_kbd_char(uint8_t kbd_char) {

  uart_send_char(KBD_CHAR_MSG);
  uart_send_char(kbd_char);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_join_msg() {

  uart_send_char(JOIN_MSG);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_ack_join_msg() {

  uart_send_char(ACK_JOIN_MSG);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_leave_msg() {

  uart_send_char(LEAVE_MSG);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_add_char(uint8_t kbd_char) {

  uart_send_char(ADD_CHAR_MSG);
  uart_send_char(kbd_char);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_rem_char(uint8_t kbd_char) {

  uart_send_char(REM_CHAR_MSG);
  uart_send_char(kbd_char);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_p1_leave() {

  uart_send_char(P1_LEAVE_MSG);
  uart_send_char(TRAILER_ID);

  return 0;
}

int uart_send_char(uint8_t data) {

  uint8_t lsr;
  uint8_t i = 0;
  

  do {
    tickdelay(micros_to_ticks(2000));
    uart_read_LSR(&lsr);
    
    if (i == MAX_TRIES_POLLING) 
      return 1;

    i++;
  } while (!(lsr & LSR_THR_EMPTY));

  uart_write_THR(data);

  return 0;
}

void uart_parse_msg() {

  switch (lastChar) {
    case ACK_JOIN_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = ACK_JOIN;
      break;
    case JOIN_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = JOIN;
      break;
    case LEAVE_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = LEAVE;
      break;
    case P1_LEAVE_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = P1_LEAVE;
      break;
    case KBD_CHAR_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = KBD_CHAR;
      break;
    case ADD_CHAR_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = ADD_CHAR;
      break;
    case REM_CHAR_MSG:
      current_byte = 0;
      uart_msg_complete = false;
      current_msg.type = REM_CHAR;
      break;
    case TRAILER_ID:

      if (current_msg.type == JOIN || current_msg.type == LEAVE || current_msg.type == P1_LEAVE || current_msg.type == ACK_JOIN)
        uart_msg_complete = true;

      else if ((current_msg.type == ADD_CHAR && current_byte == 1) || (current_msg.type == KBD_CHAR && current_byte == 1) || (current_msg.type == REM_CHAR && current_byte == 1))
        uart_msg_complete = true;

      break;

    default:

      if (current_msg.type == KBD_CHAR) {
        current_msg.bytes[current_byte] = lastChar;
        current_byte++;
      }
      else if (current_msg.type == REM_CHAR) {
        current_msg.bytes[current_byte] = lastChar;
        current_byte++;
      }
      else if (current_msg.type == ADD_CHAR) {
        current_msg.bytes[current_byte] = lastChar;
        current_byte++;
      }
      break;
  }
}

bool is_msg_complete() {
  return uart_msg_complete;
}

uint8_t get_last_received_char() {

  uint8_t newChar = lastChar;
 
  return newChar;
}

struct uart_msg *uart_get_last_msg() {
  return &current_msg;
}

int uart_init() {

  uint8_t default_lcr = 0;
  uart_read_LCR(&default_lcr);
  uart_set_bit_rate(9600);
  uart_set_no_stop_bits(2);
  uart_set_parity(PRT_ODD);
  uart_set_bits_per_char(8);
  enable_received_data_int();

  uint8_t lsr = 0;
  uart_read_LSR(&lsr);
  while (lsr & LSR_RECEIVER_DATA) {

    uint8_t lixo = 0;
    uart_read_RBR(&lixo);

    uart_read_LSR(&lsr);
  };
  return 0;
}
