/**
  ******************************************************************************
  * @file    sd.c 
  * @author  MCD Application Team
  * @brief   This example code shows how to use the SD Driver
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
#include "stm32f7xx_hal.h"
#include "main.h"
#include "main_sd_FatFS.h"

#define SENSOR_LED_PIN                        GPIO_PIN_7
#define SENSOR_INPUT_1_PIN                    GPIO_PIN_3
#define SENSOR_INPUT_2_PIN                    GPIO_PIN_4

#define SENSOR_LED_GPIO_PORT                  GPIOF
#define SENSOR_INPUTS_GPIO_PORT               GPIOJ

#define SENSOR_LED_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()
#define SENSOR_LED_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOF_CLK_DISABLE()
#define SENSOR_INPUTS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOJ_CLK_ENABLE()
#define SENSOR_INPUTS_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOJ_CLK_DISABLE()

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS SD_fatfs;
char SDDISKPath[4];            /* USB Host logical drive path */

/* Private function prototypes -----------------------------------------------*/
static void SD_SetHint(void);
static void SensorInputs_Init(void);
static void SensorLed_Init(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SD Demo
  * @param  None
  * @retval None
  */
void SD_FatFS_demo (void)
{
  uint8_t SD_state = MSD_OK;

  SensorLed_Init();
  HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_RESET);
  SensorInputs_Init();

  SdmmcTest = 1;
  SD_SetHint();
  SD_state = BSP_SD_Init();

  if (SD_state != MSD_OK)
  {
    if(SD_state == MSD_ERROR_SD_NOT_PRESENT)
    {
      BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD shall be inserted before running test", LEFT_MODE);
    }
    else
    {
    BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD Initialization : FAIL.", LEFT_MODE);
    }
    BSP_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD Test Aborted.", LEFT_MODE);
  }
  else
  {
    BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD Initialization : OK.", LEFT_MODE);
    HAL_Delay(3000);
    if (FATFS_LinkDriver(&SD_Driver, SDDISKPath) == 0)
    {
      BSP_LCD_DisplayStringAt(20, 115, (uint8_t *)"Step 1", LEFT_MODE);
      HAL_Delay(3000);
      LCD_LOG_Init();
      if (f_mount(&SD_fatfs, SDDISKPath, 0) == FR_OK)
      {
          BSP_LCD_DisplayStringAt(20, 120, (uint8_t *)"Step 2", LEFT_MODE);
          HAL_Delay(3000);
          MSC_File_Operations_SD_FatFS(SDDISKPath[0]);
          HAL_Delay(2000);
          Explore_Disk_SD_FatFS(SDDISKPath);
          HAL_Delay(2000);

          LCD_UsrLog("> Select an operation to Continue.\n");
          while (BSP_PB_GetState(BUTTON_USER) != SET)
          {
            if(HAL_GPIO_ReadPin(SENSOR_INPUTS_GPIO_PORT, SENSOR_INPUT_2_PIN) == GPIO_PIN_SET)  {
              HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_SET);
            } else {
              HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_RESET);
            }
          }
          /* Prevent debounce effect for user key */
          HAL_Delay(400);

      } else {
        LCD_ErrLog("ERROR : Cannot Mount FatFs! \n");
      }
    }

  }


  if (CheckForUserInput() > 0)
  {
    SdmmcTest = 0;
    if (f_mount(NULL, "", 0) != FR_OK)
    {
      LCD_ErrLog("ERROR : Cannot DeInitialize FatFs! \n");
    }
    if (FATFS_UnLinkDriver(SDDISKPath) != 0)
    {
      LCD_ErrLog("ERROR : Cannot UnLink FatFS Driver! \n");
    }
    BSP_SD_DeInit();
    return;
  }
  
}

/**
  * @brief  Display SD Demo Hint
  * @param  None
  * @retval None
  */
static void SD_SetHint(void)
{
  /* Clear the LCD */
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set LCD Demo description */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 80);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"SD", CENTER_MODE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"This example shows how to write", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 45, (uint8_t *)"and read data on the microSD and also", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"how to detect the presence of the card", CENTER_MODE);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_DrawRect(10, 90, BSP_LCD_GetXSize() - 20, BSP_LCD_GetYSize() - 100);
  BSP_LCD_DrawRect(11, 91, BSP_LCD_GetXSize() - 22, BSP_LCD_GetYSize() - 102);

  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
}

static void SensorLed_Init(void) {

  GPIO_InitTypeDef gpio_init_structure;

  /* Enable the SENSOR INPUTS clock */
  SENSOR_LED_GPIO_CLK_ENABLE();

    /* Configure sensor inputs pin as input */
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    gpio_init_structure.Pin = SENSOR_LED_PIN;
    HAL_GPIO_Init(SENSOR_LED_GPIO_PORT, &gpio_init_structure);

}

static void SensorInputs_Init(void) {

  GPIO_InitTypeDef gpio_init_structure;

  /* Enable the SENSOR INPUTS clock */
  SENSOR_INPUTS_GPIO_CLK_ENABLE();

    /* Configure sensor inputs pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;

    gpio_init_structure.Pin = SENSOR_INPUT_1_PIN;
    HAL_GPIO_Init(SENSOR_INPUTS_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.Pin = SENSOR_INPUT_2_PIN;
    HAL_GPIO_Init(SENSOR_INPUTS_GPIO_PORT, &gpio_init_structure);

}

/**
  * @}
  */ 

/**
  * @}
  */ 

