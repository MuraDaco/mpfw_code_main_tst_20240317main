/*ds3231 driver header file - Reza Ebrahimi v1.0*/
/* modified by Marco Dau */ 
#ifndef HW_DS3231_H
#define HW_DS3231_H

#include <stdbool.h>
#include <stdint.h>

#include "hwI2C.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x68U
#define I2C_BAUDRATE 100000U
#define I2C_DATA_LENGTH 33U

/*******************************************************************************
 * Variables
 ******************************************************************************/

enum
{
	kStatusGroup_DS3231_I2C = 1000
};
/*! @brief I2C status return codes. */
typedef enum
{
    kStatus_I2C_OK                  , // 00
    kStatus_I2C_ERROR               , // 01
    kStatus_I2C_BUSY                , // 02
    kStatus_I2C_TIMEOUT             , // 03
    kStatus_DS3231_Clock_Halt		, // 04
    kStatus_DS3231_Clock_Run		, // 05
    kStatus_DS3231_No_Reset			, // 06
    kStatus_DS3231_Is_Running		, // 07
    kStatus_DS3231_Is_Stpped		, // 08
    kStatus_DS3231_Operation_Done	, // 09
    kStatus_DS3231_Operation_Failed , // 10
    kStatus_DS3231_Not_Initialized	, // 11
    kStatus_DS3231_Initialized		, // 12
    kStatus_DS3231_Unknown_Option	, // 13
    kStatus_DS3231_Unknown_Command	, // 14

} kStatus_DS3231;

typedef enum
{
    kIdDateTime_Second      ,
    kIdDateTime_Minute      ,
    kIdDateTime_Hour        ,
    kIdDateTime_DayWeek     ,
    kIdDateTime_Day         ,
    kIdDateTime_Month       ,
    kIdDateTime_Year        ,
    kIdDateTime_Num         ,
} kIdDateTime;

enum options {SECOND, MINUTE, HOUR, DAY_OF_WEEK, DATE, MONTH, YEAR, CONTROL, CONTROL_STATUS, AGING_OFFSET, ALARM1, ALARM2, ALARMS, TEMPERATURE, TIME, ALL};
enum square_wave {WAVE_OFF, WAVE_1, WAVE_2, WAVE_3, WAVE_4};
enum run_state {CLOCK_HALT, CLOCK_RUN};
enum ds3231_registers {
  DS3231_REGISTER_SECONDS, 
  DS3231_REGISTER_MINUTES, 
  DS3231_REGISTER_HOURS, 
  DS3231_REGISTER_DAY_OF_WEEK, 
  DS3231_REGISTER_DATE, 
  DS3231_REGISTER_MONTH, 
  DS3231_REGISTER_YEAR, 
  DS3231_REGISTER_ALARM1_SECONDS,
  DS3231_REGISTER_ALARM1_MINUTES,
  DS3231_REGISTER_ALARM1_HOURS,
  DS3231_REGISTER_ALARM1_DAY_OF_WEEK_OR_DATE,
  DS3231_REGISTER_ALARM2_MINUTES,
  DS3231_REGISTER_ALARM2_HOURS,
  DS3231_REGISTER_ALARM2_DAY_OF_WEEK_OR_DATE,
  DS3231_REGISTER_CONTROL,
  DS3231_REGISTER_CONTROL_STATUS,
  DS3231_REGISTER_AGING_OFFSET,
  DS3231_REGISTER_ALARM2_TEMP_MSB,
  DS3231_REGISTER_ALARM2_TEMP_LSB
  };
  
#define DS3231_I2C_ADDRESS                    0XD0 // 0x1101000_ : the LSB bit is for read/write command

#define FORCE_RESET                           0X00
#define NO_FORCE_RESET                        0X01
#define NO_RESET                              0X02

#define DS3231_BIT_12_24                      0X06
#define DS3231_BIT_CENTURY                    0X07
#define DS3231_BIT_A1M1                       0X07
#define DS3231_BIT_A1M2                       0X07
#define DS3231_BIT_A1M3                       0X07
#define DS3231_BIT_A1M4                       0X07
#define DS3231_BIT_A2M2                       0X07
#define DS3231_BIT_A3M3                       0X07
#define DS3231_BIT_A4M4                       0X07
#define DS3231_BIT_12_24_ALARM1               0X06
#define DS3231_BIT_12_24_ALARM2               0X06
#define DS3231_BIT_DY_DT_ALARM1               0X06
#define DS3231_BIT_DY_DT_ALARM2               0X06
#define DS3231_BIT_A1IE                       0X00
#define DS3231_BIT_A2IE                       0X01
#define DS3231_BIT_INTCN                      0X02
#define DS3231_BIT_RS1                        0X03
#define DS3231_BIT_RS2                        0X04
#define DS3231_BIT_CONV                       0X05
#define DS3231_BIT_BBSQW                      0X06
#define DS3231_BIT_EOSC                       0X07
#define DS3231_BIT_A1F                        0X00
#define DS3231_BIT_A2F                        0X01
#define DS3231_BIT_BSY                        0X02
#define DS3231_BIT_EN32KHZ                    0X03
#define DS3231_BIT_OSF                        0X07

#define DS3231_REGISTER_SECONDS_DEFAULT                       0X00
#define DS3231_REGISTER_MINUTES_DEFAULT                       0X00
#define DS3231_REGISTER_HOURS_DEFAULT                         0X00
#define DS3231_REGISTER_DAY_OF_WEEK_DEFAULT                   0X01
#define DS3231_REGISTER_DATE_DEFAULT                          0X01
#define DS3231_REGISTER_MONTH_DEFAULT                         0X01
#define DS3231_REGISTER_YEAR_DEFAULT                          0X00
#define DS3231_REGISTER_ALARM1_SECONDS_DEFAULT                0X00
#define DS3231_REGISTER_ALARM1_MINUTES_DEFAULT                0X00            
#define DS3231_REGISTER_ALARM1_HOURS_DEFAULT                  0X00
#define DS3231_REGISTER_ALARM1_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define DS3231_REGISTER_ALARM2_MINUTES_DEFAULT                0X00
#define DS3231_REGISTER_ALARM2_HOURS_DEFAULT                  0X00    
#define DS3231_REGISTER_ALARM2_DAY_OF_WEEK_OR_DATE_DEFAULT    0X00
#define DS3231_REGISTER_CONTROL_DEFAULT                       0X1C
#define DS3231_REGISTER_CONTROL_STATUS_DEFAULT                0X00
#define DS3231_REGISTER_AGING_OFFSET_DEFAULT                  0X00

#if defined(__cplusplus)
extern "C" {
#endif

uint8_t  ds3231_register_get (uint8_t p_register);

uint32_t ds3231_reset(uint8_t input);
uint32_t ds3231_init(uint8_t run_command, uint8_t reset_state);
uint32_t ds3231_init_status_update();
uint32_t ds3231_read(uint8_t registers, uint8_t *data_array);
uint32_t ds3231_set(uint8_t registers, uint8_t *data_array);
uint32_t ds3231_init_status_report();
uint32_t ds3231_run_command(uint8_t command);
uint32_t ds3231_run_status();


#if defined(__cplusplus)
}
#endif


#endif /* __HW_IHW_DS3231_H2C_H */
