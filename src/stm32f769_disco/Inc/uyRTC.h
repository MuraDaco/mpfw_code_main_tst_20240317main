/**
  ******************************************************************************
  * @file    uyRTC.h
  * @author  Marco Dau
  * @brief   RTC routine implementation
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UY_RTC_H
#define __UY_RTC_H

#ifdef _DEF_C
#define _EXTERN_DEF
#else
#define _EXTERN_DEF extern
#endif


#include <stdio.h>
#include <string.h>

typedef struct
{
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
} uyRTC_TypeDef;

// 0123456789012345678901
// 2024.04.10 - 17:23:51
// year     ->  0
// month    ->  5
// day      ->  8
// hour     -> 13 
// minute   -> 16
// second   -> 19
// null     -> 21

#define YEAR_1000                       0
#define YEAR_100                        1
#define YEAR_10                         2
#define YEAR_1                          3
#define SPACE_1                         4
#define MONTH_10                        5
#define MONTH_1                         6
#define SPACE_2                         7
#define DAY_10                          8
#define DAY_1                           9
#define SPACE_3                         10
#define SPACE_4                         11
#define SPACE_5                         12
#define HOUR_10                         13
#define HOUR_1                          14
#define SPACE_6                         15
#define MINUTE_10                       16
#define MINUTE_1                        17
#define SPACE_7                         18
#define SECOND_10                       19
#define SECOND_1                        20
#define NULL_TERMINATOR                 21
#define RTC_STRING_DATE_TIME_LENGTH     22


#define MONTH_JANUARY     1
#define MONTH_FEBRUARY    2
#define MONTH_MARCH       3
#define MONTH_APRIL       4
#define MONTH_MAY         5
#define MONTH_JUNE        6
#define MONTH_JULY        7
#define MONTH_AUGUST      8
#define MONTH_SEPTEMBER   9
#define MONTH_OCTOBER     10
#define MONTH_NOVEMBER    11
#define MONTH_DICEMBER    12

_EXTERN_DEF uyRTC_TypeDef g_rtc;
_EXTERN_DEF uint16_t            g_rtc_timer_sys_tick;
_EXTERN_DEF volatile uint8_t    g_rtc_update;

void uyRTC_SysTick        (void);
void uyRTC_Init           (void);
void uyRTC_Update         (void);
void uyRTC_DateTimeSet    (uint8_t* p_str);
void uyRTC_DateTimeGet    (uint8_t* p_str);


#endif /* __UY_RTC_H */

