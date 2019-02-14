#include <machine/asm.h>
#include "rtc_macro.h"


.intel_syntax


.global _C_LABEL(rtc_asm_ih)
.global _C_LABEL(alarmOccuredASM)
.global _C_LABEL(updateOccuredASM)
.global _C_LABEL(rtc_update_fields)

.data
  _C_LABEL(alarmOccuredASM):
    .byte 0
  _C_LABEL(updateOccuredASM):
    .byte 0 

.text

  _C_LABEL(rtc_asm_ih):

    /* get the status reg */
    mov al, RTC_REG_C                 
    out RTC_ADDR_REG, al                          /* Select the RTC's C-Register for reading */
    in al,RTC_DATA_REG                            /* Put the C-Register's value into AL */
    mov cl, al                                    /* Move the C-Register's value into CL */

    /* find what kind if int happened */

    /* ALARM INTERRUPT */
  ai:
    test cl, RTC_AI_PENDING                     /* Check if an alarm interrupt occured*/
    jz ui                                       /* If none occured, check the next interrupt-type */
    call rtc_update_fields                      /* Update the RTCFields struct */
    mov byte ptr [_C_LABEL(alarmOccuredASM)], 1 /* Toggle the alarm occured flag */

     /* UPDATE INTERRUPT */
  ui:
    test cl, RTC_UI_PENDING                     /* Check if an update interrupt occured*/
    jz pi                                       /* If none occured, check the next interrupt-type */
    call rtc_update_fields                      /* Update the RTCFields struct */
    mov byte ptr [_C_LABEL(updateOccuredASM)], 1/* Toggle the update occured flag */

    /* PERIODIC INTERRUPT */
  pi:
    test dl, RTC_PI_PENDING                     /* Check if an update interrupt occured*/
    jz end                                      /* If none occured, jump to the end*/
  
    /* No functionality as been implemented in for the periodic interrupts */



  end:

    ret







