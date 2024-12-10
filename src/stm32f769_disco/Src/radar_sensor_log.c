/**
  ******************************************************************************
  * @file    radar_sensor_file_op.c 
  * @author  Marco Dau
  * @brief   
  ******************************************************************************
  */

/* Includes ----------------------------------------------------------------- */
#include "radar_sensor.h"
#include "radar_sensor_log.h"
#include "lcd_log.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include <stdbool.h>

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
static FIL MyFile_SD_FatFS;
static FRESULT res_SD_FatFS;
static char file_time_set[] =   "-:/time_set.txt";
static char file_logs[] =       "-:/logs.txt";
static bool uyRTC_ds3231_DateTime_Set = true;

static FATFS SD_fatfs;
static char SDDISKPath[4];            /* USB Host logical drive path */

/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
/* Private functions -------------------------------------------------------- */
// #pragma GCC push_options
// #pragma GCC optimize ("O0")

// optimize("O0"):  110088
// optimize("O1"):  110004
// optimize("O2"):  109992
// optimize("O3"):  109992
// optimize("O4"):  109992
// optimize("MAX"): 109912 -> there is a BAD OPTIMIZATION
__attribute__((optimize("O1"))) uint8_t radar_sensor_log_init(uint8_t *p_str)  
//uint8_t radar_sensor_log_init(uint8_t *p_str)  
{
    uint8_t l_result = 1; // '0': OK - '1': ERROR
    uint8_t SD_state = MSD_OK;

    SD_state = BSP_SD_Init();
    if (SD_state != MSD_OK) {

        if(SD_state == MSD_ERROR_SD_NOT_PRESENT) {
            // ......................................... 0         1         2         3         4         5
            // ......................................... 0123456789012345678901234567890123456789012345678901
            BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD shall be inserted before running logger function", LEFT_MODE);
        } else    {
            BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD Initialization : FAIL.", LEFT_MODE);
        }
        BSP_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD Test Aborted.", LEFT_MODE);

    } else  {

        BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD Initialization : OK.", LEFT_MODE);
        
        if (FATFS_LinkDriver(&SD_Driver, SDDISKPath) == 0) {

            BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_1, (uint8_t *)"Step 1 - Link Driver -> OK", LEFT_MODE);
            
            if (f_mount(&SD_fatfs, SDDISKPath, 0) == FR_OK) {

                BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_2, (uint8_t *)"Step 2 - File System Mount -> OK", LEFT_MODE);
                l_result = radar_sensor_start_time_get(p_str);

                p_str[21] = ' '; p_str[22] = '-'; p_str[23] = ' '; 
                p_str[24] = 'x'; p_str[25] = ' ';
                p_str[26] = 'x'; p_str[27] = 'x'; p_str[28] = 'x'; 
                p_str[29] = 0;

                BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_3, (uint8_t*) "Test - " , LEFT_MODE);
                BSP_LCD_DisplayStringAt(100, 100 + 15*LINE_STEP_3, p_str, LEFT_MODE);

            } else {
                BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_2, (uint8_t *)"Step 2 - File System Mount -> ERROR!!!", LEFT_MODE);
            }
        }
    }

    return l_result;
}
// #pragma GCC pop_options

uint8_t radar_sensor_log_deinit(uint8_t p_step)  
{
    uint8_t l_result = 0;
        if (f_mount(NULL, "", 0) != FR_OK) {
            BSP_LCD_DisplayStringAt(20, 100 + 15*(p_step + MAX_LOG_EVENT_LINE), (uint8_t *)"Step END 1 - DeInitialize FatFs -> ERROR!!!  ", LEFT_MODE);
            l_result = 1;
            p_step++;
        }
        HAL_Delay(3000);

        if (FATFS_UnLinkDriver(SDDISKPath) != 0) {
            BSP_LCD_DisplayStringAt(20, 100 + 15*(p_step + MAX_LOG_EVENT_LINE), (uint8_t *)"Step END 2 - UnLink FatFS Driver -> ERROR!!!  ", LEFT_MODE);
            l_result = 1;
            p_step++;
        }
        HAL_Delay(3000);

        BSP_SD_DeInit();

    return l_result;
}

/**
  * @brief  Files operations: Read/Write and compare
  * @param  None
  * @retval None
  */
uint8_t radar_sensor_start_time_get(uint8_t *p_str)  
{
    uint8_t l_result = 0;
    uint16_t bytesread;

    file_time_set[0] = SDDISKPath[0];

    if (f_open(&MyFile_SD_FatFS, file_time_set, FA_READ) != FR_OK)
    {
        BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_3, (uint8_t *)"Cannot Open 'file_time_set.txt' file for read. -> ERROR!!!  ", LEFT_MODE);

        // read data from i2c-RTC
        uyRTC_DateTimeGet(p_str); 
    }
    else
    {
        // date & time string format: <yyyy>_<mm>_<dd>_/_<hh>:<mm>:<ss>
        BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_3, (uint8_t *)"Step 3 - Reading date & time from <time_set.txt> file ...", LEFT_MODE);
        res_SD_FatFS = f_read(&MyFile_SD_FatFS, p_str, RTC_STRING_DATE_TIME_LENGTH-1, (void *)&bytesread);

        if ((bytesread == 0) || (res_SD_FatFS != FR_OK)) /* EOF or Error */
        {
            BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_4, (uint8_t *)"Cannot read 'file_time_set.txt' file for read. -> ERROR!!!  ", LEFT_MODE);
            l_result = 1;
        } else {
            if(uyRTC_ds3231_DateTime_Set)   {
                uyRTC_ds3231_DateTime_Set = false;
                // set data to i2c-RTC
                uyRTC_DateTimeSet(p_str); 
            } else {
                // read data from i2c-RTC
                uyRTC_DateTimeGet(p_str); 
            }
        }
        f_close(&MyFile_SD_FatFS);
    }

    return l_result;
}

uint8_t radar_sensor_log(uint8_t *p_str)
{
    uint8_t l_result = 0;
    uint32_t bytesWritten_SD_FatFS;
    file_logs[0] = SDDISKPath[0];

    if (f_open(&MyFile_SD_FatFS, file_logs, FA_WRITE|FA_OPEN_APPEND) != FR_OK)
    {
        BSP_LCD_DisplayStringAt(20, 100 + 15*LINE_STEP_3, (uint8_t *)"Cannot Open 'logs.txt' file for write. -> ERROR!!!  ", LEFT_MODE);
        l_result = 1;
    }
    else
    {
        p_str[29] = '\n';
        res_SD_FatFS = f_write(&MyFile_SD_FatFS, p_str, LOG_MESSAGE_LENGTH-1, (void *)&bytesWritten_SD_FatFS);
        f_close(&MyFile_SD_FatFS);
    }

    return l_result;
}

