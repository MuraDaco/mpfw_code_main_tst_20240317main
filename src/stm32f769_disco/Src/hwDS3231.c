/*DS3231 high level driver - Reza Ebrahimi v1.0*/
/*This is MCU independent; there's no need to change the contents of this file. Use low level api to adapt the driver to your MCU of choice*/
#include "hwDS3231.h"

static void BCD_to_HEX(uint8_t *data_array, uint8_t array_length);        /*turns the bcd time values from ds3231 into hex*/
static void HEX_to_BCD(uint8_t *data_array, uint8_t array_length);        /*turns the hex numbers into bcd, to be written back into ds3231*/
static void ds3231_data_clone(uint8_t option, uint8_t *input_array);        /*clones an array into one of 3 ..._registers_clone[], based on chosen option*/

static uint8_t register_current_value;        /*used to read current values of ds3231 registers*/
static uint8_t register_new_value;        /*used to write new values to ds3231 registers*/
static uint8_t time_registers_clone[7];       /*used for the purpose of not curropting the time settings by reconverting an already converted HEX to BCD array*/
static uint8_t alarm1_registers_clone[4];       /*used for the purpose of not curropting the time settings by reconverting an already converted HEX to BCD array*/
static uint8_t alarm2_registers_clone[3];       /*used for the purpose of not curropting the time settings by reconverting an already converted HEX to BCD array*/
static uint8_t register_default_value[] = {       /*used in reset function, contains default values*/
  DS3231_REGISTER_SECONDS_DEFAULT,
  DS3231_REGISTER_MINUTES_DEFAULT,
  DS3231_REGISTER_HOURS_DEFAULT,
  DS3231_REGISTER_DAY_OF_WEEK_DEFAULT,
  DS3231_REGISTER_DATE_DEFAULT,
  DS3231_REGISTER_MONTH_DEFAULT,
  DS3231_REGISTER_YEAR_DEFAULT,
  DS3231_REGISTER_ALARM1_SECONDS_DEFAULT,
  DS3231_REGISTER_ALARM1_MINUTES_DEFAULT,
  DS3231_REGISTER_ALARM1_HOURS_DEFAULT,
  DS3231_REGISTER_ALARM1_DAY_OF_WEEK_OR_DATE_DEFAULT,
  DS3231_REGISTER_ALARM2_MINUTES_DEFAULT,
  DS3231_REGISTER_ALARM2_HOURS_DEFAULT,
  DS3231_REGISTER_ALARM2_DAY_OF_WEEK_OR_DATE_DEFAULT,
  DS3231_REGISTER_CONTROL_DEFAULT,
  DS3231_REGISTER_CONTROL_STATUS_DEFAULT,
  DS3231_REGISTER_AGING_OFFSET_DEFAULT,
};

/*ds3231_init function accepts 3 inputs, data_array[7] is the new time settings,
  run_state commands ds3231 to run or halt (CLOCK_RUN and CLOCK_HALT), and reset_state
  could force-reset ds3231 (FORCE_RESET) or checks if ds3231 is reset beforehand
  (NO_FORCE_RESET)*/
uint32_t ds3231_init(uint8_t run_command, uint8_t reset_state)
{
  uint32_t retVal = kStatus_I2C_ERROR;
  uint8_t  l_rtc_data_array[7];

  l_rtc_data_array[kIdDateTime_Second]   =  0;
  l_rtc_data_array[kIdDateTime_Minute]   =  0;
  l_rtc_data_array[kIdDateTime_Hour]     = 12;
  l_rtc_data_array[kIdDateTime_DayWeek]  =  1;
  l_rtc_data_array[kIdDateTime_Day]      =  1;
  l_rtc_data_array[kIdDateTime_Month]    =  1;
  l_rtc_data_array[kIdDateTime_Year]     = 24;

  hwI2C_Init();
  retVal = ds3231_init_status_report();
  if (((retVal == kStatus_DS3231_Not_Initialized) && (reset_state == NO_FORCE_RESET)) || (reset_state == FORCE_RESET))
  {
        uint8_t l_value =  0;     
	  retVal = ds3231_reset(ALL);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &l_value);
	  retVal = ds3231_set(TIME, l_rtc_data_array);
  }
  if (retVal == kStatus_DS3231_Initialized) {
	  retVal = ds3231_init_status_update();        /*now the device is initialized (DS3231_INITIALIZED)*/
	  retVal = ds3231_run_command(run_command);
  }

  return retVal;
}

/*function to command ds3231 to stop or start updating its time registers, WORKS ONLY WITH BATTERY BACKED DS3231*/
uint32_t ds3231_run_command(uint8_t command)
{
  uint32_t retVal = kStatus_I2C_ERROR;

  switch (command)
  {
    case CLOCK_RUN:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);
      register_new_value = register_current_value & (~(1 << DS3231_BIT_EOSC));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_new_value);
      break;
    case CLOCK_HALT:
  	  retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);
      register_new_value = register_current_value | (1 << DS3231_BIT_EOSC);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_new_value);
      break;
    default:
    	retVal = kStatus_DS3231_Unknown_Command;
    	break;
  }

  return retVal;
}

/*function to check the status of ds3231, whether its running or not. WORKS ONLY WITH BATTERY BACKED DS3231*/
uint32_t ds3231_run_status()
{
  uint32_t retVal;

  register_current_value = 0xFF;
  retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);
  if((retVal & 0x000000FF) == kStatus_I2C_OK) {
	  if ((register_current_value & (1 << DS3231_BIT_EOSC)) == 0)
		  retVal = kStatus_DS3231_Clock_Run;
	  else
		  retVal = kStatus_DS3231_Clock_Halt;
  }
  return retVal;
}

/*function to read the oscillator flag OSF and to decide whether it has been reset beforehand or not*/
uint32_t ds3231_init_status_report()
{
  uint32_t retVal;

  register_current_value = 0xFF;
  retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);
  if((retVal & 0x000000FF) == kStatus_I2C_OK) {
	  if (register_current_value & (1 << DS3231_BIT_OSF))
		  retVal = kStatus_DS3231_Not_Initialized;
	  else
		  retVal = kStatus_DS3231_Initialized;
  }
  return retVal;
}

/*function to reset the OSF bit (OSF = 0)*/
uint32_t ds3231_init_status_update()
{
  uint32_t retVal        = kStatus_I2C_ERROR;

  retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);
  register_new_value = register_current_value & (~(1 << DS3231_BIT_OSF));
  retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_new_value);

  return retVal;
}

/*resets the desired register(s), without affecting run_state (RUN_STATE ONLY MAKES SENSE WITH BATTERY-BACKED DS3231*/
uint32_t ds3231_reset(uint8_t option)
{
  uint32_t retVal        = kStatus_I2C_ERROR;

  /*data_clone function together with registers_clone static variables, prevent data curroption caused by reconvertig HEX to BCD*/
  ds3231_data_clone(TIME, &register_default_value[0]);
  ds3231_data_clone(ALARM1, &register_default_value[7]);
  ds3231_data_clone(ALARM2, &register_default_value[0X0B]);
  HEX_to_BCD(&time_registers_clone[0], 7);
  HEX_to_BCD(&alarm1_registers_clone[0], 4);
  HEX_to_BCD(&alarm2_registers_clone[0], 3);
  switch (option)
  {
    case SECOND:
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &time_registers_clone[0]);
      break;
    case MINUTE:
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MINUTES, &time_registers_clone[1]);
      break;
    case HOUR:
      time_registers_clone[2] &= (~(1 << DS3231_BIT_12_24));        /*to turn on 24 hours format by default*/
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_HOURS, &time_registers_clone[2]);
      break;
    case DAY_OF_WEEK:
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DAY_OF_WEEK, &time_registers_clone[3]);
      break;
    case DATE:
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DATE, &time_registers_clone[4]);
      break;
    case MONTH:
      time_registers_clone[5] &= (~(1 << DS3231_BIT_CENTURY));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MONTH, &time_registers_clone[5]);
      break;
    case YEAR:
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_YEAR, &time_registers_clone[6]);
      break;
    case CONTROL:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);       /*in order to preserve running state (RUN or HALT)*/
      register_new_value = (register_current_value & (1 << DS3231_BIT_EOSC)) | (register_default_value[0X0E] & (~(1 << DS3231_BIT_EOSC)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_new_value);
      break;
    case CONTROL_STATUS:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);       /*in order to preserve OSF flag*/
      register_new_value = (register_current_value & (1 << DS3231_BIT_OSF)) | (register_default_value[0X0F] & (~(1 << DS3231_BIT_EOSC)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_new_value);
      break;
    case ALARM1:
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_ALARM1_SECONDS, &alarm1_registers_clone[0], 4);
      break;
    case ALARM2:
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_ALARM2_MINUTES, &alarm2_registers_clone[0], 3);
      break;
    case ALARMS:
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_ALARM1_SECONDS, &alarm1_registers_clone[0], 4);
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_ALARM2_MINUTES, &alarm2_registers_clone[0], 3);
      break;
    case AGING_OFFSET:
      register_new_value = DS3231_REGISTER_AGING_OFFSET_DEFAULT;
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_AGING_OFFSET, &register_new_value);
    case TIME:
      time_registers_clone[2] &= (~(1 << DS3231_BIT_12_24));
      time_registers_clone[5] &= (~(1 << DS3231_BIT_CENTURY));
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &time_registers_clone[0], 7);
      break;
    case ALL:
      /*TIME registers reset*/
      time_registers_clone[2] &= (~(1 << DS3231_BIT_12_24));        /*to preserve 24 hours mode*/
      time_registers_clone[5] &= (~(1 << DS3231_BIT_CENTURY));        /*resetting century bit*/
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &time_registers_clone[0], 7);       /*to reset all the TIME registers*/
      /*CONTROL and CONTROL_STATUS registers reset*/
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);       /*in order to preserve OSF flag*/
      register_new_value = (register_current_value & (1 << DS3231_BIT_OSF)) | (register_default_value[0X0F] & (~(1 << DS3231_BIT_EOSC)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_new_value);
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);       /*to preserve run_status, either RUN or HALT*/
      register_new_value = (register_current_value & (1 << DS3231_BIT_EOSC)) | (register_default_value[0X0E] & (~(1 << DS3231_BIT_EOSC)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_new_value);
      /*AGING_OFFSET registers reset*/
      register_new_value = DS3231_REGISTER_AGING_OFFSET_DEFAULT;
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_AGING_OFFSET, &register_new_value);
      break;
    default:
      retVal = kStatus_DS3231_Unknown_Option;
      break;
  }

  return retVal;

}

/*function to read internal registers of ds3231, one register at a time or an array of registers*/
uint32_t ds3231_read(uint8_t option, uint8_t *data_array)
{
	uint32_t retVal        = kStatus_I2C_ERROR;

  switch (option)
  {
    case SECOND:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case MINUTE:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MINUTES, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case HOUR:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_HOURS, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case DAY_OF_WEEK:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DAY_OF_WEEK, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case DATE:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DATE, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case MONTH:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MONTH, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case YEAR:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_YEAR, &register_current_value);
      *data_array = register_current_value;
      BCD_to_HEX(data_array, 1);
      break;
    case CONTROL:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);
      *data_array = register_current_value;
      break;
    case CONTROL_STATUS:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);
      *data_array = register_current_value;
      break;
    case AGING_OFFSET:
      retVal = time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_AGING_OFFSET, &register_current_value);
      *data_array = register_current_value;
    case TIME:
      retVal = time_i2c_read_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, data_array, 7);
      BCD_to_HEX(data_array, 7);
      break;
    default:
      retVal = kStatus_DS3231_Unknown_Option;
  }

  return retVal;
}

/*function to set internal registers of ds3231, one register at a time or an array of registers*/
uint32_t ds3231_set(uint8_t option, uint8_t *data_array)
{
  uint32_t retVal        = kStatus_I2C_ERROR;

  switch (option)
  {
    case SECOND:
      time_registers_clone[0] = *data_array;
      HEX_to_BCD(&time_registers_clone[0], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &time_registers_clone[0]);
      break;
    case MINUTE:
      time_registers_clone[1] = *data_array;
      HEX_to_BCD(&time_registers_clone[1], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MINUTES, &time_registers_clone[1]);
      break;
    case HOUR:
      time_registers_clone[2] = *data_array;
      HEX_to_BCD(&time_registers_clone[2], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_HOURS, &time_registers_clone[2]);
      break;
    case DAY_OF_WEEK:
      time_registers_clone[3] = *data_array;
      HEX_to_BCD(&time_registers_clone[3], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DAY_OF_WEEK, &time_registers_clone[3]);
      break;
    case DATE:
      time_registers_clone[4] = *data_array;
      HEX_to_BCD(&time_registers_clone[4], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_DATE, &time_registers_clone[4]);
      break;
    case MONTH:
      time_registers_clone[5] = *data_array;
      HEX_to_BCD(&time_registers_clone[5], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_MONTH, &time_registers_clone[5]);
      break;
    case YEAR:
      time_registers_clone[6] = *data_array;
      HEX_to_BCD(&time_registers_clone[6], 1);
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_YEAR, &time_registers_clone[6]);
      break;
    case CONTROL:
      time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_current_value);
      register_new_value = (register_current_value & (1 << DS3231_BIT_EOSC)) | (*data_array & (~(1 << DS3231_BIT_EOSC)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL, &register_new_value);
      break;
    case CONTROL_STATUS:
      time_i2c_read_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_current_value);
      register_new_value = (register_current_value & (1 << DS3231_BIT_OSF)) | (*data_array & (~(1 << DS3231_BIT_OSF)));
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_CONTROL_STATUS, &register_new_value);
      break;                                                                                         
    case TIME:
      ds3231_data_clone(TIME, data_array);
      HEX_to_BCD(&time_registers_clone[0], 7);
      retVal = time_i2c_write_multi(DS3231_I2C_ADDRESS, DS3231_REGISTER_SECONDS, &time_registers_clone[0], 7);
      break;
    case AGING_OFFSET:
      register_new_value = *data_array;
      retVal = time_i2c_write_single(DS3231_I2C_ADDRESS, DS3231_REGISTER_AGING_OFFSET, &register_new_value);
      break;
    default:
      retVal = kStatus_DS3231_Unknown_Option;
  }

  return retVal;
}

/*to clone the desired data and prevent reconversion of BCD to HEX*/
static void ds3231_data_clone(uint8_t option, uint8_t *input_array)
{
  switch (option)
  {
    case TIME:
      for (uint8_t counter = 0; counter < 7; counter++)
      {
        time_registers_clone[counter] = input_array[counter];
      }
      break;
    case ALARM1:
      for (uint8_t counter = 0; counter < 4; counter++)
      {
        alarm1_registers_clone[counter] = input_array[counter];
      }
      break;
    case ALARM2:
      for (uint8_t counter = 0; counter < 3; counter++)
      {
        alarm2_registers_clone[counter] = input_array[counter];
      }
      break;
    default:
      break;
  }
}

/*internal function related to this file and not accessible from outside*/
static void BCD_to_HEX(uint8_t *data_array, uint8_t array_length)
{
  for (int8_t index = (array_length - 1); index >= 0; index--)
  {
    data_array[index] = ((data_array[index] >> 4) << 1) + ((data_array[index] >> 4) << 3) + (data_array[index] & 0X0F);
  }
}

/*internal function related to this file and not accessible from outside*/
static void HEX_to_BCD(uint8_t *data_array, uint8_t array_length)
{
  uint8_t temporary_value;
  for (int8_t index = (array_length - 1); index >= 0; index--)
  {
    temporary_value = 0;
    while (((int8_t)data_array[index] - 0X0A) >= 0)
    {
      temporary_value += 0X10;
      data_array[index] -= 0X0A;
    }
    temporary_value += data_array[index];
    data_array[index] = temporary_value;
  }
}
