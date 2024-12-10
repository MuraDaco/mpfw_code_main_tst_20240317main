/**
  ******************************************************************************
  * @file    hwI2C.h
  * @author  Marco Dau
  * @brief   RTC routine implementation
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_I2C_H
#define __HW_I2C_H

#include "main_hwI2C.h"


#ifdef _DEF_C
#define _EXTERN_DEF
#else
#define _EXTERN_DEF extern
#endif
/* Private define ------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
//#define MASTER_BOARD
//#define I2C_ADDRESS        0x40

/* I2C TIMING Register define when I2C clock source is APB1 (SYSCLK/4) */
/* I2C TIMING is calculated in case of the I2C Clock source is the APB1CLK = 50 MHz */
/* This example use TIMING to 0x40912732 to reach 100 kHz speed (Rise time = 700 ns, Fall time = 100 ns) */
#define I2C_TIMING      0x40912732

HAL_StatusTypeDef       hwI2C_Init          (void);
HAL_StatusTypeDef       hwI2C_Test          (void);
HAL_StatusTypeDef       hwI2C_Test2          (void);
uint32_t                hwI2C_Error_Get     (void);
HAL_I2C_StateTypeDef    hwI2C_State_Get     (void);

uint32_t time_i2c_write_single (uint8_t device_address, uint8_t register_address, uint8_t *data_byte);
uint32_t time_i2c_write_multi  (uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length);
uint32_t time_i2c_read_single  (uint8_t device_address, uint8_t register_address, uint8_t *data_byte);
uint32_t time_i2c_read_multi   (uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length);


#endif /* __HW_I2C_H */

