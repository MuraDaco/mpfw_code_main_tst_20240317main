/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/file_operations.c
  * @author  MCD Application Team
  * @brief   Write/read file on the disk.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ----------------------------------------------------------------- */
#include "main_msc_standalone.h"

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
//FATFS USBH_fatfs;
FIL MyFile;
FRESULT res;
uint32_t bytesWritten;
uint8_t rtext[200];
uint8_t wtext[] = "USB Host Library : Mass Storage Example";
//uint8_t m_WorkingBuffer	[_MAX_SS];
char file_path[] = "-:/USB_SD.txt";

/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
/* Private functions -------------------------------------------------------- */

/**
  * @brief  Files operations: Read/Write and compare
  * @param  None
  * @retval None
  */
void MSC_File_Operations(char p_unit)
{
  char res_str[2];
  res_str[1] = 0;
  uint16_t bytesread;

  file_path[0] = p_unit;

  LCD_UsrLog("INFO : FatFs Initialized \n");

  if (f_open(&MyFile, file_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    LCD_ErrLog("Cannot Open 'USB_SD.txt' file \n");
  }
  else
  {
    LCD_UsrLog("INFO : waiting open/create file ...\n");
    HAL_Delay(5000);
    f_sync(&MyFile);
    LCD_UsrLog("INFO : waiting sync ...\n");
    HAL_Delay(5000);
    LCD_UsrLog("INFO : 'USB_SD.txt' opened for write  \n");
    res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&bytesWritten);
    f_close(&MyFile);

    if ((bytesWritten == 0) || (res != FR_OK))  /* EOF or Error */
    {
      LCD_ErrLog("Cannot Write on the  'USB_SD.txt' file - Error code: -");
      if(res<10) {
        res_str[0] = res + '0';
      } else {
        res_str[0] = res + 'A';
      }
      LCD_ErrLog(res_str);
      LCD_ErrLog("-\n");
    }
    else
    {
      if (f_open(&MyFile, file_path, FA_READ) != FR_OK)
      {
        LCD_ErrLog("Cannot Open 'USB_SD.txt' file for read.\n");
      }
      else
      {
        LCD_UsrLog("INFO : Text written on the 'USB_SD.txt' file \n");

        res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

        if ((bytesread == 0) || (res != FR_OK)) /* EOF or Error */
        {
          LCD_ErrLog("Cannot Read from the  'USB_SD.txt' file \n");
        }
        else
        {
          LCD_UsrLog("Read Text : \n");
          LCD_DbgLog((char *)rtext);
          LCD_DbgLog("\n");
        }
        f_close(&MyFile);
      }
      /* Compare read data with the expected data */
      if ((bytesread == bytesWritten))
      {
        LCD_UsrLog("INFO : FatFs data compare SUCCES");
        LCD_UsrLog("\n");
      }
      else
      {
        LCD_ErrLog("FatFs data compare ERROR");
        LCD_ErrLog("\n");
      }
    }
  }
}

