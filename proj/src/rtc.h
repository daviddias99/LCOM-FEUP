#ifndef RTC_H

#define RTC_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include "rtc_macro.h"
#include "debug.h"

/** @defgroup RTC RTC
 * @{
 * Functions to manage the real time clock
 */

/** @name RTCFields */
/**@{
 *
 * @brief Struct that holds the current state of the RTC
 * 
 */
struct RTCFields {

  uint8_t seconds;  /**< @brief seconds on the RTC*/
  uint8_t minutes;  /**< @brief minutes on the RTC*/
  uint8_t hours;    /**< @brief hours on the RTC(0 to 23)*/
  uint8_t weekDay;  /**< @brief week day on the RTC (1 to 7, 1=Sunday)*/
  uint8_t day;      /**< @brief day on the RTC*/
  uint8_t month;    /**< @brief month on the RTC*/
  uint8_t year;     /**< @brief year on the RTC(0 to 99)*/
};




/**
 * @brief Init the RTC operations. Inits the RTCFields struct, sets the display to binary, unblocks que rtc, stores the initial config and disables previously enabled interrutps.
 * 
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_init();

/**
 * @brief Reverts the state of the RTC to the initial one
 * 
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_revert();

/**
 * @brief Subscribe the RTC interrutps
 * 
 * @param bit_no    returns the bit of the interrupt mask that corresponds to the RTC
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribe the RTC interrutps
 * 
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_unsubscribe_int (void);

/**
 * @brief Writes the data byte into the given register
 * 
 * @param reg       register where the data will be written
 * @param data      data to be written in the register
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_write_to_reg(uint8_t reg, uint8_t data);

/**
 * @brief Reads the data byte of a given register
 * 
 * @param reg       register where the data will be written
 * @param data      data read from the register
 * @return int      zero if sucessful, non-zero otherwise
 */
int rtc_read_reg(uint8_t reg, uint8_t* data);

/**
 * @brief Enables periodic interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_enable_periodic_int();

/**
 * @brief Enables alarm interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_enable_alarm_int();

/**
 * @brief Enables update interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_enable_update_int();

/**
 * @brief Disables periodic interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_disable_periodic_int();

/**
 * @brief Disables alarm interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_disable_alarm_int();

/**
 * @brief Disables update interrutps from the rtc
 * 
 *  @return int      zero if sucessful, non-zero otherwise
 */
int rtc_disable_update_int();
  
/**
 * @brief Set the RTC display mode on the registers as BCD (binary coded decimal)
 * 
 * @return int       zero if sucessful, non-zero otherwise
 */
int rtc_set_BCD();

/**
 * @brief Set the RTC display mode on the registers as binary
 * 
 * @return int       zero if sucessful, non-zero otherwise
 */
int rtc_set_BIN();

/**
 * @brief Enables alarm interrutps for the time passed as argument. This function enables alarm interrutps. Don't care values are passed as -1.
 * 
 * @param hour        hours of the alarm
 * @param minute      minutes of the alarm
 * @param second      seconds of the alarm
 * @return int        zero if sucessful, non-zero otherwise
 */
int rtc_set_alarm(int8_t hour, int8_t minute, int8_t second);

/**
 * @brief Disable the updating of RTC's time and date registers
 * 
 * @return int        zero if sucessful, non-zero otherwise
 */
int rtc_disable_updates();

/**
 * @brief Enable the updating of RTC's time and date registers
 * 
 * @return int        zero if sucessful, non-zero otherwise
 */
int rtc_enable_updates();

/**
 * @brief Initializes the fields of the RTCFields struct with the values of RTC's date and time registers. This must done by automatically by the rtc_init() function.
 * 
 * @return int        zero if sucessful, non-zero otherwise
 */
int rtc_init_fields();

/**
 * @brief Updates the RTCFields struct with the current value of the RTC's time and date registers. To otimize the process the date is only updated if when a day passes to the following(at 0:00), but if the argument flag is set to true the date is updated anyway.
 * 
 * @param forceDateUpdate     true if the date should be updated forcefully
 * @return int                zero if sucessful, non-zero otherwise
 */
int rtc_update_fields(bool forceDateUpdate);

/**
 * @brief Updates the RTCFields struct and sets the alarmOccured flag to true.
 * 
 * @return int                zero if sucessful, non-zero otherwise
 */
int rtc_handle_alarm_int();

/**
 * @brief Updates the RTCFields struct and sets the updateOccured flag to true.
 *  
 * @return int                zero if sucessful, non-zero otherwise
 */
int rtc_handle_update_int();

/**
 * @brief Updates the RTCFields struct. This function currently does not have any use.
 *  
 * @return int                zero if sucessful, non-zero otherwise
 */
int rtc_handle_periodic_int();

/**
 * @brief Returns the value a RTCFields struct with the current values of time and date
 * 
 * @return struct RTCFields   struct containing the time and date info
 */
 int rtc_get_currentTime(struct RTCFields* rtcfields);

/**
 * @brief Returns the state of the alarmOccured flag and toggles it to false. This function is used to check if an alarm occurred. If an alarm is set this function must be used in order to clear the flag for future alarm interrupts.
 * 
 * @return true         a alarm occurred
 * @return false        no alarm occurred
 */
bool rtc_ack_alarm();

/**
 * @brief Returns the state of the updateOccured flag and toggles it to false. This function is used to check if an update occurred. If an update occurs this function must be used in order to clear the flag for future update interrupts.
 * 
 * @return true         a update occurred
 * @return false        no update occurred
 */
bool rtc_ack_update();

/**
 * @brief Checks if the argument is a valid RTC time and date register.
 * 
 * @param reg           register to check
 * @return true         if the register is valid
 * @return false        if the register isn't valid
 */
bool rtc_valid_register(uint8_t reg);

/**
 * @brief Checks if the RTC is being updated. During this period the time and date registers should not ne altered nor read to avoid errors and invalid readings.
 * 
 * @return true        if an update is in progress
 * @return false       if no update is occuring
 */
bool rtc_update_in_progress();

/**
 * @brief Handles RTC interrupts. When an int happens the handler checks which type os types of interrupts happened and executes the correct sub-handlers for each time of interrupt. Currently the periodic interrupts don't have an usefull handler.
 * 
 */
void rtc_ih();

/**
 * @brief Handles RTC interrupts. When an int happens the handler checks which type os types of interrupts happened and executes the correct sub-handlers for each time of interrupt. Currently the periodic interrupts don't have an usefull handler. This version of the interrupt handler is written in assembly. 
 * 
 */

void rtc_asm_ih();

/**
 * @brief Calls the assembly interrupt handler. (rtc_asm_ih)
 * 
 */
void rtc_asm_ih_caller();

/**
 * @brief Returns the state of the alarmOccuredASM flag and toggles it to 0. This function is used to check if an alarm occurred when using the assembly interrupt handler. If an alarm is set this function must be used in order to clear the flag for future alarm interrupts.
 * 
 * @return true         a alarm occurred
 * @return false        no alarm occurred
 */
bool rtc_ack_alarm_asm();

/**
 * @brief Returns the state of the updateOccuredASM flag and toggles it to 0. This function is used to check if an update occurred when using the assembly interrupt handler. If an update occurs this function must be used in order to clear the flag for future update interrupts.
 * 
 * @return true         a update occurred
 * @return false        no update occurred
 */
bool rtc_ack_update_asm();



#endif
