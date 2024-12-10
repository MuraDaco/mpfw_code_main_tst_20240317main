/**
  ******************************************************************************
  * @file    uyRTC.c 
  * @author  Marco Dau
  * @brief   RTC routine implementation
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "uyRTC.h"
#include "hwDS3231.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void uyRTC_Init           (void)  {

    // set date & time
    g_rtc.year      = 2024;
    g_rtc.month     = 4;
    g_rtc.day       = 11;

    g_rtc.hour      = 15;
    g_rtc.minute    = 20;
    g_rtc.second    = 54;    
    // start update rtc

}


void uyRTC_SysTick        (void)
{
    if(g_rtc_timer_sys_tick)
    {
        if(g_rtc_timer_sys_tick > 1000) g_rtc_timer_sys_tick = 1000;
        g_rtc_timer_sys_tick--;
    }
    else
    {
        g_rtc_timer_sys_tick = 999;
        g_rtc_update = 1;
    }
}


void uyRTC_Update         (void)  {

    if(g_rtc_update)
    {
        g_rtc_update = 0;

        if(g_rtc.second < 59)   {
            g_rtc.second++;
        } else {
            g_rtc.second = 0;
            if(g_rtc.minute < 59)   {
                g_rtc.minute++;
            } else {
                g_rtc.minute = 0;
                if(g_rtc.hour < 23 )   {
                    g_rtc.hour++;
                } else {
                    g_rtc.hour = 0;
                    switch (g_rtc.month)    {
                        case MONTH_JANUARY:
                        case MONTH_MARCH:
                        case MONTH_MAY:
                        case MONTH_JULY:
                        case MONTH_AUGUST:
                        case MONTH_OCTOBER:
                        case MONTH_DICEMBER:
                            if(g_rtc.day < 31 ) g_rtc.day++; else g_rtc.day = 1;
                            break;
                        case MONTH_FEBRUARY:
                            if(g_rtc.year%4)    {
                                // it is NOT a leap year
                                if(g_rtc.day < 28 ) g_rtc.day++; else g_rtc.day = 1;
                            } else {
                                // it is a leap year
                                if(g_rtc.day < 29 ) g_rtc.day++; else g_rtc.day = 1;
                            }
                            break;
                        case MONTH_APRIL:
                        case MONTH_JUNE:
                        case MONTH_SEPTEMBER:
                        case MONTH_NOVEMBER:
                            if(g_rtc.day < 30 ) g_rtc.day++; else g_rtc.day = 1;
                            break;
                    }
                    if(g_rtc.day == 1)  {
                        if(g_rtc.month < 12)    {
                            g_rtc.month++;
                        } else {
                            g_rtc.month = 1;
                            g_rtc.year++;
                        }
                    }
                }
            }
        }
    }

}


void uyRTC_DateTimeSet    (uint8_t* p_str)  {
    // 0123456789012345678901
    // 2024.04.10 - 17:23:51
    // year     ->  0 YEAR_1000
    // month    ->  5 MONTH_10
    // day      ->  8 DAY_10
    // hour     -> 13 HOUR_10
    // minute   -> 16 MINUTE_10
    // second   -> 19 SECOND_10
    // null     -> 21
    uint8_t l_date_time[7];

    // year
    g_rtc.year      = (p_str[YEAR_1000]-'0')*1000   + (p_str[YEAR_100]-'0')*100     + (p_str[YEAR_10]-'0')*10 + (p_str[YEAR_1]-'0');
    g_rtc.month     = (p_str[MONTH_10]-'0')*10      + (p_str[MONTH_1]-'0');
    g_rtc.day       = (p_str[DAY_10]-'0')*10        + (p_str[DAY_1]-'0');

    g_rtc.hour      = (p_str[HOUR_10]-'0')*10       + (p_str[HOUR_1]-'0');
    g_rtc.minute    = (p_str[MINUTE_10]-'0')*10     + (p_str[MINUTE_1]-'0');
    g_rtc.second    = (p_str[SECOND_10]-'0')*10     + (p_str[SECOND_1]-'0');

    l_date_time[kIdDateTime_Year]       = g_rtc.year   - 2000;
    l_date_time[kIdDateTime_Month]      = g_rtc.month  ;
    l_date_time[kIdDateTime_Day]        = g_rtc.day    ;
    l_date_time[kIdDateTime_Hour]       = g_rtc.hour   ;
    l_date_time[kIdDateTime_Minute]     = g_rtc.minute ;
    l_date_time[kIdDateTime_Second]     = g_rtc.second ;
  
    ds3231_set(TIME, l_date_time);

}


void uyRTC_DateTimeGet    (uint8_t* p_str)  {
    uint16_t l_tmp;
    uint8_t l_date_time[7];

    p_str[SPACE_1] = p_str[SPACE_2] = p_str[SPACE_3] = p_str[SPACE_5] = p_str[SPACE_6] = p_str[SPACE_7] = ' ';
    p_str[SPACE_4] = '/';

    // read data from i2c-RTC
    ds3231_read(TIME, l_date_time);
    g_rtc.year   = l_date_time[kIdDateTime_Year] + 2000;
    g_rtc.month  = l_date_time[kIdDateTime_Month];
    g_rtc.day    = l_date_time[kIdDateTime_Day];
    g_rtc.hour   = l_date_time[kIdDateTime_Hour];
    g_rtc.minute = l_date_time[kIdDateTime_Minute];
    g_rtc.second = l_date_time[kIdDateTime_Second];

    // year
    p_str[YEAR_1000]    = g_rtc.year/1000  + '0';   l_tmp = g_rtc.year%1000;
    p_str[YEAR_100]     = l_tmp/100        + '0';   l_tmp = l_tmp%100;
    p_str[YEAR_10]      = l_tmp/10         + '0';   l_tmp = l_tmp%10;
    p_str[YEAR_1]       = l_tmp            + '0';

    // month
    p_str[MONTH_10]     = g_rtc.month/10   + '0';   l_tmp = g_rtc.month%10;
    p_str[MONTH_1]      = l_tmp            + '0';

    // day
    p_str[DAY_10]       = g_rtc.day/10     + '0';   l_tmp = g_rtc.day%10;
    p_str[DAY_1]        = l_tmp            + '0';

    // hour
    p_str[HOUR_10]      = g_rtc.hour/10   + '0';   l_tmp = g_rtc.hour%10;
    p_str[HOUR_1]       = l_tmp           + '0';

    // minute
    p_str[MINUTE_10]    = g_rtc.minute/10 + '0';   l_tmp = g_rtc.minute%10;
    p_str[MINUTE_1]     = l_tmp           + '0';

    // second
    p_str[SECOND_10]    = g_rtc.second/10 + '0';   l_tmp = g_rtc.second%10;
    p_str[SECOND_1]     = l_tmp           + '0';



}
