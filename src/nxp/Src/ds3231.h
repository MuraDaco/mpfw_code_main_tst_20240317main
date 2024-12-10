/*ds3231 driver header file - Reza Ebrahimi v1.0*/
#ifndef DS3231_H
#define DS3231_H

#include <stdbool.h>
#include <stdint.h>

#include "fsl_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_MASTER_BASE (I2C8_BASE)
#define I2C_MASTER_CLOCK_FREQUENCY (12000000)
#define WAIT_TIME 10U
#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)

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
enum kStatus_DS3231
{
    kStatus_DS3231_Clock_Halt		= MAKE_STATUS(kStatusGroup_DS3231_I2C, 0),
    kStatus_DS3231_Clock_Run		= MAKE_STATUS(kStatusGroup_DS3231_I2C, 1),
    kStatus_DS3231_No_Reset			= MAKE_STATUS(kStatusGroup_DS3231_I2C, 2),
    kStatus_DS3231_Is_Running		= MAKE_STATUS(kStatusGroup_DS3231_I2C, 3),
    kStatus_DS3231_Is_Stpped		= MAKE_STATUS(kStatusGroup_DS3231_I2C, 4),
    kStatus_DS3231_Operation_Done	= MAKE_STATUS(kStatusGroup_DS3231_I2C, 5),
    kStatus_DS3231_Operation_Failed = MAKE_STATUS(kStatusGroup_DS3231_I2C, 6),
    kStatus_DS3231_Not_Initialized	= MAKE_STATUS(kStatusGroup_DS3231_I2C, 7),
    kStatus_DS3231_Initialized		= MAKE_STATUS(kStatusGroup_DS3231_I2C, 8),
    kStatus_DS3231_Unknown_Option	= MAKE_STATUS(kStatusGroup_DS3231_I2C, 9),
    kStatus_DS3231_Unknown_Command	= MAKE_STATUS(kStatusGroup_DS3231_I2C,10),

};


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
  
#define DS3231_I2C_ADDRESS                    0X68

#define FORCE_RESET                           0X00
#define NO_FORCE_RESET                        0X01
#define NO_RESET                              0X02
// #define DS3231_IS_RUNNING                     0X01
// #define DS3231_IS_STOPPED                     0X00
// #define OPERATION_DONE                        0X01
// #define OPERATION_FAILED                      0X00
// #define DS3231_NOT_INITIALIZED                0X01        /*bit OSF == 1 indicates that the oscillator was stopped*/
// #define DS3231_INITIALIZED                    0X00        /*bit OSF == 0 indicates that the oscillator was running before mcu was powered on*/

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

status_t ds3231_reset(uint8_t input);
status_t ds3231_init(uint8_t *data_array, uint8_t run_command, uint8_t reset_state);
status_t ds3231_init_status_update();
status_t ds3231_read(uint8_t registers, uint8_t *data_array);
status_t ds3231_set(uint8_t registers, uint8_t *data_array);
status_t ds3231_init_status_report();
status_t ds3231_run_command(uint8_t command);
status_t ds3231_run_status();

void     ds3231_I2C_init		(void);
status_t time_i2c_write_single	(uint8_t device_address, uint8_t register_address, 			uint8_t *data_byte							);
status_t time_i2c_write_multi	(uint8_t device_address, uint8_t start_register_address,	uint8_t *data_array, uint8_t data_length	);
status_t time_i2c_read_single	(uint8_t device_address, uint8_t register_address,			uint8_t *data_byte							);
status_t time_i2c_read_multi	(uint8_t device_address, uint8_t start_register_address,	uint8_t *data_array, uint8_t data_length	);

#if defined(__cplusplus)
}
#endif


#endif
