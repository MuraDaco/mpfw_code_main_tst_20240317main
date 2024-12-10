/**
  ******************************************************************************
  * @file    radar_sensor_log.h
  * @brief   Header for radar_sensor_file_op.c module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADAR_SENSOR_LOG_H
#define __RADAR_SENSOR_LOG_H

#include "radar_sensor.h"

uint8_t radar_sensor_log_init(uint8_t *p_str);
uint8_t radar_sensor_log_deinit(uint8_t p_step);

uint8_t radar_sensor_start_time_get(uint8_t *p_str);
uint8_t radar_sensor_log(uint8_t *p_str);

/* Includes ----------------------------------------------------------------- */


#endif /* __RADAR_SENSOR_LOG_H */

