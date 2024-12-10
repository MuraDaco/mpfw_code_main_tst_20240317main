/**
  ******************************************************************************
  * @file    radar_sensor.h
  * @brief   Header for radar_sensor.c module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADAR_SENSOR_H
#define __RADAR_SENSOR_H


/* Includes ----------------------------------------------------------------- */
#include "stdio.h"
#include "main.h"
#include "uyRTC.h"
#include "stm32f769i_discovery.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"


typedef enum {
     InputType_button
    ,InputType_sensor_out
    ,InputType_num
} InputType;

typedef enum {
     DIn_Status_OFF = GPIO_PIN_RESET
    ,DIn_Status_ON  = GPIO_PIN_SET
} DIn_Status;

// 0         1         2    
// 0123456789012345678901234567890
// 2024.04.10 - 17:23:51 - x xxx[cr][null]
#define LOG_MESSAGE_LENGTH RTC_STRING_DATE_TIME_LENGTH+9

#define LINE_STEP_1     1
#define LINE_STEP_2     2
#define LINE_STEP_3     3
#define LINE_STEP_4     4
#define LINE_STEP_5     5
#define LINE_STEP_6     6
#define LINE_STEP_7     7
#define LINE_STEP_8     8

#define MAX_LOG_EVENT_LINE 8

/* Function definitions */

void RadarSensor_demo (void);


#endif /* __RADAR_SENSOR_H */

