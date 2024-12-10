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
#ifndef __MAIN_MSC_STANDALONE_H
#define __MAIN_MSC_STANDALONE_H


/* Includes ----------------------------------------------------------------- */
#include "main_FatFS.h"

/* Exported types ----------------------------------------------------------- */
typedef enum
{
  MSC_DEMO_START = 0,
  MSC_DEMO_FILE_OPERATIONS,
  MSC_DEMO_EXPLORER,
} MSC_Demo_State;

typedef struct _DemoStateMachine
{
  __IO MSC_Demo_State state;
  __IO uint8_t select;
} MSC_DEMO_StateMachine;

typedef enum
{
  APPLICATION_IDLE = 0,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
} MSC_ApplicationTypeDef;

extern USBH_HandleTypeDef hUSBHost;
extern MSC_ApplicationTypeDef Appli_state;

/* Exported constants ------------------------------------------------------- */
/* Exported macro ----------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void Menu_Init(void);
void MSC_MenuProcess(char p_unit);
void Toggle_Leds(void);

#endif /* __MAIN_MSC_STANDALONE_H */

