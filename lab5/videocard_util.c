
#include "videocard_util.h"


int util_get_LSW(uint32_t val, uint32_t *lsw) {

  *lsw = 0x0000FFFF & val; // stores in lsb the least significant byte of val

  return 0;
}

int util_get_MSW(uint32_t val, uint32_t *msw) {

  *msw = (val >> 16);// stores in msb the most significant byte of val

  return 0;
}

int waitForEscape(){

 // ipc_status and msg store information from the received interrupt
  int ipc_status;
  message msg;

  // storage of number of the bit that is set in the vector if the desired interrupt occurs
  __uint8_t hookMaskBit = 0;

  // validSubscribe stores the validity of interrupt subscriptions to deal with error cases
  int validSubscribe = kbd_subscribe_int(&hookMaskBit);

  if (validSubscribe != OK)
    return validSubscribe;

  __uint64_t irq_set = BIT(hookMaskBit); // mask used to check if the desired interrupt occurred

  uint8_t scancode = 0; // variable used to store the scancode
  uint8_t ih_error;

  // while scancode is not ESC breakcode
  while (scancode != ESC_BREAKCODE) {

    // get a request message
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0)
      continue;
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) { // check the source of the interrupt
        case HARDWARE:

          if (msg.m_notify.interrupts & irq_set) { // check if interrupt comes from the KBC

            kbc_ih(); // get the current scancode using the 'C' keyboard interrupt handler

            ih_error = is_ih_err();    // store the error status of the keyboard interrupt handler
            scancode = get_scancode(); // store the scancode
          }
          break;
        default:
          break;
      }
    }
  }

  return kbd_unsubscribe_int(); // unsubscribe from the keyboard interrupts

}

uint32_t makeMask(uint8_t numBits, bool mask_of_ones){

  uint32_t resultMask = 1;

  for(int i = 0; i < numBits -1;i++){

    resultMask = resultMask << 1;
    resultMask |= 1;
  }
  if(!mask_of_ones)
    resultMask = ~resultMask;

  return resultMask;

}

uint32_t FarPtr_to_Ptr(void *baseAdd, uint32_t farPtr) {

  uint32_t result = (uint32_t) baseAdd;
  uint32_t offsetValue;
  uint32_t segmentValue;

  util_get_LSW(farPtr, &offsetValue);
  util_get_MSW(farPtr, &segmentValue);

  result += ((segmentValue << 4) + offsetValue);

  return result;
}

