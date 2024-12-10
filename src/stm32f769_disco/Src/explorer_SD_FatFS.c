/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/explorer.c
  * @author  MCD Application Team
  * @brief   Explore the USB flash disk content
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
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
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
/* Private functions -------------------------------------------------------- */

/**
  * @brief  Displays disk content.
  * @param  path: Pointer to root path
  * @retval Operation result
  */
FRESULT Explore_Disk_SD_FatFS(char *path)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char tmp[14];
  uint8_t line_idx = 0;
  char res_str[2];
  res_str[1] = 0;

  LCD_UsrLog("Opening root directory.\n");
  res = f_opendir(&dir, path);
  if (res != FR_OK) {
    LCD_UsrLog("Error during opening the root directory, see the following error code.\n");
    if(res<10) {
      res_str[0] = res + '0';
    } else {
      res_str[0] = res + 'A';
    }
    LCD_UsrLog(res_str);
    LCD_UsrLog("\n");
  } else {
    LCD_UsrLog("Start display list.\n");
    while (1)
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      // if (fno.fname[0] == '.')
      // {
      //   continue;
      // }

      strcpy(tmp, fno.fname);

      line_idx++;
      if (line_idx > YWINDOW_SIZE)
      {
        line_idx = 0;
        LCD_UsrLog("> Press [Key] To Continue.\n");

        /* KEY Button in polling */
        while ((BSP_PB_GetState(BUTTON_USER) != SET) &&
               (Appli_state != APPLICATION_DISCONNECT))
        {
          /* Wait for User Input */
        }
      }

      {
        LCD_UsrLog("   |__");
      }

      {
        strcat(tmp, "\n");
        LCD_UsrLog((void *)tmp);
      }

    }
    f_closedir(&dir);
  }


  return res;
}

