#include <lcom/lcf.h>

#include "mouse.h"
#include "keyboard.h"
#include "i8042.h"


/**
 * @brief Bit we wish to set to one in the mask returned by an interrupt
 */
static int hookID = 12;
/**
 * @brief Last byte read in the mouse interrupt handler
 */
static uint32_t scanbyte = 0;
/**
 * @brief Flag set to something other than 0 if there is an error in the interrupt handler
 */
static int ih_err = 0;


int ms_subscribe_int(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(MS_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID);
}

int ms_unsubscribe_int(void) {
  
  return sys_irqrmpolicy(&hookID);
}

int ms_disable_int_not(void){

  return sys_irqdisable(&hookID);
}

int ms_enable_int_not(void){

  return sys_irqenable(&hookID);
}

void(mouse_ih)(void) {

   uint8_t statusByte = 0;

  if (kbd_read_status(&statusByte) != OK)
    ih_err = -1;
  else {

    // if the output buffer has available data
    if (statusByte & OBF_FULL) {

      if (sys_inb_cnt(OUT_BUFFER, &scanbyte) != OK)
        ih_err = -3;

      if ((statusByte & (PAR_ERR | TO_ERR)) != OK)
        ih_err = -2;
      else
        ih_err = 0;
    }
  }
}

uint8_t ms_get_ih_byte(void) {
  return (uint8_t) scanbyte;
}

int ms_is_ih_err(void) {
  return ih_err;
}

int ms_command(uint8_t command) {

  int errByte;              // store the validity state of kbc command
  uint8_t msAck = 0;        // variable used to store acknowledgements
  unsigned int numTrys = 2; // number of times a command is tried before giving up

  for(unsigned int i  = 0; i < numTrys; i++){

      // write the command to the mouse
      msAck = kbc_command(MS_WRITE_BYTE, command, &errByte);
      
      // check acknowledgement
      if(msAck == MS_BT_ACK)
        return 0;
      else if(errByte != OK)
        return -1;
  } 
  
  return -2;
}
