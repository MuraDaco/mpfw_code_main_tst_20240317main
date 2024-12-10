/**
  ******************************************************************************
  * @file    main.h
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_FATFS_H
#define __MAIN_FATFS_H

#include "lcd_log.h"

/* Includes ----------------------------------------------------------------- */
#include "stdio.h"
#include "stm32f769i_discovery.h"
#include "lcd_log.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "usbh_diskio_dma.h"
#include "sd_diskio.h"

/* Exported types ----------------------------------------------------------- */

/* Exported constants ------------------------------------------------------- */
/* Exported macro ----------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
FRESULT Explore_Disk(char *path, uint8_t recu_level);
FRESULT Explore_Disk_SD_FatFS(char *path);
void MSC_File_Operations(char p_unit);
void MSC_File_Operations_SD_FatFS(char p_unit);

#endif /* __MAIN_FATFS_H */

