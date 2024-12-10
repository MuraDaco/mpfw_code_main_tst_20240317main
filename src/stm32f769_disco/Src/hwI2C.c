/**
  ******************************************************************************
  * @file    hwI2C.c 
  * @author  Marco Dau
  * @brief   I2C routine implementation
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main_hwI2C.h"
#include "hwI2C.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HWI2C_TIMEOUT   1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;
HAL_I2C_StateTypeDef hwi2c_state;
uint32_t             hwi2c_error;

uint32_t hwI2C_Error_Get     (void)
{
    return hwi2c_error;
}

HAL_I2C_StateTypeDef hwI2C_State_Get     (void)
{
    return hwi2c_state;
}

HAL_StatusTypeDef hwI2C_Test     (void)
{
    HAL_StatusTypeDef l_result;
    l_result = HAL_I2C_IsDeviceReady(&I2cHandle,0x68,2,1000);
    hwi2c_state = I2cHandle.State;
    hwi2c_error = I2cHandle.ErrorCode;
    return l_result;
}

HAL_StatusTypeDef hwI2C_Test2     (void)
{
    HAL_StatusTypeDef l_result;
    l_result = HAL_I2C_IsDeviceReady(&I2cHandle,0x11,2,1000);
    hwi2c_state = I2cHandle.State;
    hwi2c_error = I2cHandle.ErrorCode;
    return l_result;
}

HAL_StatusTypeDef hwI2C_Init           (void)
{
    HAL_StatusTypeDef l_result;

    /*##-1- Configure the I2C peripheral ######################################*/
    I2cHandle.Instance             = I2Cx;
    I2cHandle.Init.Timing          = I2C_TIMING;
    I2cHandle.Init.OwnAddress1     = 0;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2     = 0;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;  
    I2cHandle.State                = HAL_I2C_STATE_RESET;  
  
    l_result = HAL_I2C_Init(&I2cHandle);

    if(HAL_OK == l_result)
    {
        /* Enable the Analog I2C Filter */
        HAL_I2CEx_ConfigAnalogFilter(&I2cHandle,I2C_ANALOGFILTER_ENABLE);
    }

    return l_result;
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
  
    /*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2Cx;
    RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2CxCLKSOURCE_SYSCLK;
    if(HAL_OK != HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct)) {
        Error_Handler();
    }

    /*##-2- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    I2Cx_SCL_GPIO_CLK_ENABLE();
    I2Cx_SDA_GPIO_CLK_ENABLE();
    /* Enable I2Cx clock */
    I2Cx_CLK_ENABLE(); 

    /*##-3- Configure peripheral GPIO ##########################################*/  
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;

    /* I2C TX GPIO pin configuration  */
    GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
    GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
    HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
    
    /* I2C RX GPIO pin configuration  */
    GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
    GPIO_InitStruct.Pin       = I2Cx_SDA_PIN;
    HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

    // /*##-4- Configure the NVIC for I2C ########################################*/   
    // /* NVIC for I2Cx */
    // HAL_NVIC_SetPriority(I2Cx_ER_IRQn, 0, 1);
    // HAL_NVIC_EnableIRQ(I2Cx_ER_IRQn);
    // HAL_NVIC_SetPriority(I2Cx_EV_IRQn, 0, 2);
    // HAL_NVIC_EnableIRQ(I2Cx_EV_IRQn);

}

/**
  * @brief I2C MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  
    /*##-1- Reset peripherals ##################################################*/
    I2Cx_FORCE_RESET();
    I2Cx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure I2C Tx as alternate function  */
    HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
    /* Configure I2C Rx as alternate function  */
    HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);
}

/*function to transmit one byte of data to register_address on ds3231 (device_address: 0X68)*/
uint32_t time_i2c_write_single(uint8_t device_address, uint8_t register_address, uint8_t *data_byte)
{
	return time_i2c_write_multi(device_address, register_address, data_byte, 1);
}

/*function to transmit an array of data to device_address, starting from start_register_address*/
uint32_t time_i2c_write_multi(uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length)
{
    uint32_t l_result;
    l_result = HAL_I2C_Mem_Write(&I2cHandle, device_address, start_register_address,
                                    I2C_MEMADD_SIZE_8BIT, data_array, data_length, HWI2C_TIMEOUT);

    l_result += (0x10000)*I2cHandle.State;
    l_result += (0x100)*I2cHandle.ErrorCode;

    return l_result;
}

/*function to read one byte of data from register_address on ds3231*/
uint32_t time_i2c_read_single(uint8_t device_address, uint8_t register_address, uint8_t *data_byte)
{
	return time_i2c_read_multi(device_address, register_address, data_byte, 1);
}

/*function to read an array of data from device_address*/
uint32_t time_i2c_read_multi(uint8_t device_address, uint8_t start_register_address, uint8_t *data_array, uint8_t data_length)
{
    uint32_t l_result;
    l_result =  HAL_I2C_Mem_Read (&I2cHandle, device_address, start_register_address,
                                   I2C_MEMADD_SIZE_8BIT, data_array, data_length, HWI2C_TIMEOUT);

    l_result += (0x10000)*I2cHandle.State;
    l_result += (0x100)*I2cHandle.ErrorCode;

    return l_result;
}
