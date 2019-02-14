#include "util_lab3.h"
#include "i8042.h"

void(kbd_process_scancode)(uint8_t byte[], bool *escapedSeq, uint8_t scanbyte) {

  bool isMake = true; // isMake is true if the scancode is a makecode, false if it's a breakcode
  uint8_t size = 1;   // size stores the number of bytes of a scancode, 1 or 2

  // if escapedSeq is true than the current value of 'scancode' is the second byte of a two byte scancode
  if (*escapedSeq) {

    size = 2;
    byte[1] = scanbyte;

    if ((scanbyte & BIT(7)) != 0) // check if the scancode is a breakcode
      isMake = false;

    *escapedSeq = false;
    kbd_print_scancode(isMake, size, byte); // print the scancode
  }
  else if (scanbyte == TWOBYTE_SC_ID) { // check if the current scancode is a two-byte scancode
    byte[0] = scanbyte;
    *escapedSeq = true;
  }
  else {
    byte[0] = scanbyte;

    if ((scanbyte & BIT(7)) != 0) // check if the scancode is a breakcode
      isMake = false;

    kbd_print_scancode(isMake, size, byte); // print the scancode
  }
}
