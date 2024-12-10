/**
  ******************************************************************************
  * @file    BSP/Src/lcd.c 
  * @author  MCD Application Team
  * @brief   This example code shows how to use LCD drawing features.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "main_msc_standalone.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */


/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
USBH_HandleTypeDef hUSBHost;
FATFS USBH_fatfs;
MSC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;
char USBDISKPath[4];            /* USB Host logical drive path */

/* Private function prototypes ---------------------------------------------- */
//static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id);
static void MSC_InitApplication(void);
//static void CPU_CACHE_Enable(void);
//static void MPU_Config(void);
//static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LCD demo
  * @param  None
  * @retval None
  */
void USBH_demo (void)
{ 
  
  /* Init MSC Application */
  MSC_InitApplication();

  /* Init Host Library */
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  /* Add Supported Class */
  USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);

  /* Start Host Process */
  USBH_Start(&hUSBHost);

  while (1)
  {
    /* USB Host Background task */
    USBH_Process(&hUSBHost);

    /* MSC Menu Process */
    MSC_MenuProcess(USBDISKPath[0]);

    if(CheckForUserInput() > 0)
    {

      if (f_mount(NULL, "", 0) != FR_OK)
      {
        LCD_ErrLog("ERROR : Cannot DeInitialize FatFs! \n");
      }
      if (FATFS_UnLinkDriver(USBDISKPath) != 0)
      {
        LCD_ErrLog("ERROR : Cannot UnLink FatFS Driver! \n");
      }

      /* Stop Host Process */
      USBH_Stop(&hUSBHost);
      /* USB Host Background task */
      USBH_Process(&hUSBHost);

      // 
      USBH_LL_Disconnect(&hUSBHost);
      /* USB Host Background task */
      USBH_Process(&hUSBHost);

      /* Init Host Library */
      USBH_DeInit(&hUSBHost);
      /* USB Host Background task */
      USBH_Process(&hUSBHost);

      return;
    }
  }
}

/**
  * @brief  MSC application Init.
  * @param  None
  * @retval None
  */
static void MSC_InitApplication(void)
{
//  /* LCD Layer Initialization */
//  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);
//
//  /* Select the LCD Layer */
//  BSP_LCD_SelectLayer(1);
//
//  /* Enable the display */
//  BSP_LCD_DisplayOn();

  /* Initialize the LCD Log module */
  LCD_LOG_Init();

#ifdef USE_USB_HS
  LCD_LOG_SetHeader((uint8_t *) " USB OTG HS MSC Host");
#else
  LCD_LOG_SetHeader((uint8_t *) " USB OTG FS MSC Host");
#endif

  LCD_UsrLog("USB Host library started.\n");

  /* Initialize menu and MSC process */
  USBH_UsrLog("Starting MSC Demo");
  Menu_Init();
}


/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
  switch (id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
    break;

  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_DISCONNECT;
    if (f_mount(NULL, "", 0) != FR_OK)
    {
      LCD_ErrLog("ERROR : Cannot DeInitialize FatFs! \n");
    }
    if (FATFS_UnLinkDriver(USBDISKPath) != 0)
    {
      LCD_ErrLog("ERROR : Cannot UnLink FatFS Driver! \n");
    }
    break;

  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    break;

  case HOST_USER_CONNECTION:
    if (FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == 0)
    {
      if (f_mount(&USBH_fatfs, USBDISKPath, 0) != FR_OK)
      {
        LCD_ErrLog("ERROR : Cannot Initialize FatFs! \n");
      }
      USBH_UsrLog("Waiting mount ...");
      HAL_Delay(2000);
    }
    break;

  default:
    break;
  }
}


/**
  * @}
  */ 

/**
  * @}
  */ 
