#include "rtc.h"

/**
 * @brief Bit we wish to set to one in the mask returned by an interrupt
 */
static int hookID = 8;
/**
 * @brief Struct that holds the information about the current time and date registers of the RTC
 */
static struct RTCFields fields;

/**
 * @brief Flag that signals the occurence of an alarm
 */
static bool alarmOccured = false;

/**
 * @brief Flag that signals the occurence of an update
 */
static bool updateOccured = false;

/**
 * @brief Variable that holds the initial configuration of the RTC
 */
static uint8_t initialConfig;

extern uint8_t alarmOccuredASM;
extern uint8_t updateOccuredASM;

int rtc_init(){

  // read the initial config of the RTC
  uint8_t regB;
  rtc_read_reg(RTC_REG_B, &regB);
  initialConfig = regB;

  // disable previously enabled interrupts
  rtc_disable_update_int();
  rtc_disable_periodic_int();
  rtc_disable_alarm_int();

  // read the register C to unblock the rtc
  uint8_t regC;
  rtc_read_reg(RTC_REG_C, &regC);

  // set the display to binary
  rtc_set_BIN();

  // init the fields struct
  rtc_init_fields();

  return 0;
}

int rtc_revert(){

  rtc_write_to_reg(RTC_REG_B,initialConfig);
  return 0;
}


int rtc_subscribe_int(uint8_t *bit_no) {

  *bit_no = hookID;

  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID);
}

int rtc_unsubscribe_int (void) {
  return sys_irqrmpolicy(&hookID);
}

int rtc_write_to_reg(uint8_t reg, uint8_t data){

  if(!rtc_valid_register(reg))
    return 1;

  sys_outb(RTC_ADDR_REG,reg);
  sys_outb(RTC_DATA_REG,data);

  return 0;
}

bool rtc_valid_register(uint8_t reg){

  if( (reg >= 0x0) && (reg <= 0x0D))
    return true;
  else
    return false;

}

int rtc_read_reg(uint8_t reg, uint8_t* data){

  uint32_t data32;

  if(!rtc_valid_register(reg))
    return 1;

  sys_outb(RTC_ADDR_REG,reg);
  sys_inb(RTC_DATA_REG,&data32);
  *data = (uint8_t)data32;
  return 0;
}

int rtc_enable_periodic_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB | RTC_PIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_enable_alarm_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB | RTC_AIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_enable_update_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB | RTC_UIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_disable_periodic_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB & ~RTC_PIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_disable_alarm_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB & ~RTC_AIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_disable_update_int(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB & ~RTC_UIE; 
  rtc_write_to_reg(RTC_REG_B,new_regB);


  return 0;
}

int rtc_set_BIN(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB | RTC_DM; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_set_BCD(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB & ~RTC_DM; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_disable_updates(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB | RTC_SET; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;

}

int rtc_enable_updates(){

  uint8_t regB;
  rtc_read_reg(RTC_REG_B,&regB);
  uint8_t new_regB = regB & ~RTC_SET; 
  rtc_write_to_reg(RTC_REG_B,new_regB);

  return 0;
}

int rtc_init_fields(){

  rtc_read_reg(RTC_HOUR,&fields.hours);
  rtc_read_reg(RTC_MIN,&fields.minutes);
  rtc_read_reg(RTC_SEC,&fields.seconds);


  rtc_read_reg(RTC_DOM,&fields.day);
  rtc_read_reg(RTC_DOW,&fields.weekDay);
  rtc_read_reg(RTC_MON,&fields.month);
  rtc_read_reg(RTC_YEAR,&fields.year);
  

  return 0;
}

int rtc_update_fields(bool forceDateUpdate){

  if(rtc_update_in_progress())
    return 0;

  rtc_read_reg(RTC_HOUR,&fields.hours);
  rtc_read_reg(RTC_MIN,&fields.minutes);
  rtc_read_reg(RTC_SEC,&fields.seconds);



  if((fields.hours == 0 && fields.minutes == 0 && fields.seconds == 0)  || 
      forceDateUpdate){
    rtc_read_reg(RTC_DOM,&fields.day);
    rtc_read_reg(RTC_DOW,&fields.weekDay);
    rtc_read_reg(RTC_MON,&fields.month);
    rtc_read_reg(RTC_YEAR,&fields.year);
  }


  return 0;
}

int rtc_set_alarm(int8_t hour, int8_t minute, int8_t second){

  if( (hour > 59) || (minute > 59) || (second >59))
    return 1;

  rtc_disable_updates();

  if(hour < 0){
    rtc_write_to_reg(RTC_HOUR_ALARM,RTC_DC_ALARM);

  }
  else{

    rtc_write_to_reg(RTC_HOUR_ALARM,hour);
  }

  if(minute < 0){
    rtc_write_to_reg(RTC_MIN_ALARM,RTC_DC_ALARM);

  }
  else{

    rtc_write_to_reg(RTC_MIN_ALARM,minute);
  }

  if(second < 0){

    rtc_write_to_reg(RTC_SEC_ALARM,RTC_DC_ALARM);

  }
  else{
    rtc_write_to_reg(RTC_SEC_ALARM,second);
  }

  rtc_enable_updates();
  rtc_enable_alarm_int();
  
  return 0;
}

int rtc_handle_alarm_int(){

  rtc_update_fields(true);
  alarmOccured = true;

  return 0;
}

int rtc_handle_update_int(){

  rtc_update_fields(true);
  updateOccured = true;
  return 0;
}

int rtc_handle_periodic_int(){

  rtc_update_fields(true);

  return 0;
}

int rtc_get_currentTime(struct RTCFields* rtcfields){

   while (rtc_update_in_progress()) {
     tickdelay(micros_to_ticks(50));
   }

   rtc_update_fields(true);
   *rtcfields = fields;

   return 0;
}

bool rtc_ack_alarm(){

  if(alarmOccured){
    alarmOccured = false;
    return true;
  }
  else
    return false;

}

bool rtc_ack_update(){

  if(updateOccured){
    updateOccured = false;
    return true;
  }
  else 
    return false;

}

bool rtc_ack_alarm_asm(){

 

  if(alarmOccuredASM){
    alarmOccuredASM = 0;
    return true;
  }
  else
    return false;
}

bool rtc_ack_update_asm(){

  

  if(updateOccuredASM){
    updateOccuredASM = 0;
    return true;
  }
  else 
    return false;

}

bool rtc_update_in_progress(){

  uint8_t regA;
  rtc_read_reg(RTC_REG_A,&regA);

  if(regA & RTC_UIP)
    return true;
  else
    return false;
}

void rtc_ih(){

  uint8_t int_source;

  rtc_read_reg(RTC_REG_C,&int_source);

  if(int_source & RTC_UI_PENDING){
    rtc_handle_update_int();
  }

  if(int_source & RTC_PI_PENDING){

  }

  if(int_source & RTC_AI_PENDING){
   
    rtc_handle_alarm_int();
  }
}

void rtc_asm_ih_caller(){


    rtc_asm_ih();

}
