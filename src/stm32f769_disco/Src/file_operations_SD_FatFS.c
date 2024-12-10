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
FIL MyFile_SD_FatFS;
FRESULT res_SD_FatFS;
uint32_t bytesWritten_SD_FatFS;
uint8_t rtext_SD_FatFS[200];
uint8_t wtext_SD_FatFS[] = "USB Host Library : Mass Storage Example";
//uint8_t m_WorkingBuffer	[_MAX_SS];
char file_path_SD_FatFS[] = "-:/SD_FatFS.txt";

/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
/* Private functions -------------------------------------------------------- */

/**
  * @brief  Files operations: Read/Write and compare
  * @param  None
  * @retval None
  */
void MSC_File_Operations_SD_FatFS(char p_unit)
{
  uint16_t bytesread;

  file_path_SD_FatFS[0] = p_unit;

  LCD_UsrLog("INFO SD_FatFS: FatFs Initialized \n");

  if (f_open(&MyFile_SD_FatFS, file_path_SD_FatFS, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    LCD_ErrLog("Cannot Open 'SD_FatFS.txt' file \n");
  }
  else
  {
    LCD_UsrLog("INFO SD_FatFS: 'SD_FatFS.txt' opened for write  \n");
    res_SD_FatFS = f_write(&MyFile_SD_FatFS, wtext_SD_FatFS, sizeof(wtext_SD_FatFS), (void *)&bytesWritten_SD_FatFS);
    f_close(&MyFile_SD_FatFS);

    if ((bytesWritten_SD_FatFS == 0) || (res_SD_FatFS != FR_OK))  /* EOF or Error */
    {
      LCD_ErrLog("Cannot Write on the  'SD_FatFS.txt' file \n");
    }
    else
    {
      if (f_open(&MyFile_SD_FatFS, file_path_SD_FatFS, FA_READ) != FR_OK)
      {
        LCD_ErrLog("Cannot Open 'SD_FatFS.txt' file for read.\n");
      }
      else
      {
        LCD_UsrLog("INFO SD_FatFS: Text written on the 'SD_FatFS.txt' file \n");

        res_SD_FatFS = f_read(&MyFile_SD_FatFS, rtext_SD_FatFS, sizeof(rtext_SD_FatFS), (void *)&bytesread);

        if ((bytesread == 0) || (res_SD_FatFS != FR_OK)) /* EOF or Error */
        {
          LCD_ErrLog("Cannot Read from the  'SD_FatFS.txt' file \n");
        }
        else
        {
          LCD_UsrLog("Read Text : \n");
          LCD_UsrLog((char *)rtext_SD_FatFS);
          LCD_UsrLog("\n");
        }
        f_close(&MyFile_SD_FatFS);
      }
      /* Compare read data with the expected data */
      if ((bytesread == bytesWritten_SD_FatFS))
      {
        LCD_UsrLog("INFO SD_FatFS: FatFs data compare SUCCES");
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

