/**
  ******************************************************************************
  * @file    radar_sensor_timers.h
  * @author  Marco Dau
  * @brief   radar sensor timers routine implementation
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADAR_SENSOR_TIMERS_H
#define __RADAR_SENSOR_TIMERS_H

#ifdef _DEF_C
#define _EXTERN_DEF
#else
#define _EXTERN_DEF extern
#endif


_EXTERN_DEF uint16_t timer_lcd;
_EXTERN_DEF uint16_t g_timer_lcd_SysTick;
_EXTERN_DEF volatile uint8_t  timer_lcd_update;

void lcd_timer_SysTick (void);

#endif /* __RADAR_SENSOR_TIMERS_H */

