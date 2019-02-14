#include <machine/asm.h>
#include "i8042.h"

.intel_syntax

.global _C_LABEL(kbc_asm_ih)
/**
 * @brief Last scanbyte read in the asm interrupt handler
 */ 
.global _C_LABEL(scanbyte)
/**
 * @brief Flag set to a value other than 0 if there is an error in the interrupt handler
 */ 
.global _C_LABEL(ih_error)

.data
  _C_LABEL(scanbyte):
    .byte 0
  _C_LABEL(ih_error):
    .byte 0 

.text
  _C_LABEL(kbc_asm_ih):

    in al, STAT_REG      /* read status byte to al register */
    mov cl, al           /* cl contains status byte */
    test cl, OBF_FULL    /* check if the output buffer is full */
    jz err_obf           /* jump to err_obf if obf is not full */
    in al, OUT_BUFFER    /* read the scanbyte from the output buffer */
    test cl, (PAR_ERR | TO_ERR)     /* check for parity or timeout errors */ 
    jnz par_to_err                  /* jump to par_to_err if an error occurred */
    mov _C_LABEL(scanbyte), al      /* move the scanbyte to the global variable */
    mov byte ptr [_C_LABEL(ih_error)], 0    /* set ih_error to 0 because no error occurred */
    jmp end

  err_obf:
    mov byte ptr [_C_LABEL(ih_error)], 1    /* if obf is full, set the flag to 1 */
    jmp end

  par_to_err:
    mov byte ptr [_C_LABEL(ih_error)], 2    /* if a parity or timeout error occurred, set the flag to 2 */

  end:
    ret
