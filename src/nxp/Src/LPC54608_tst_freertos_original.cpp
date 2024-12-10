/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    LPC54608_tst_freertos.cpp
 * @brief   Application entry point.
 */


// lib include
#include <krInitStaticManager.h>
#include <krThreadManager.h>
#include <krTimerManager.h>
#include <svStateMachineManager.h>
#include <rsSerial.h>

// tst include
#include <mn/mg/tt_MainManager.h>
#include <sv/svSync/v_01/mn/tt_mn_svSyncMain.h>

// tst/tb include
#include <tb/kr/krInit/v_02/ap_krInitStaticTbl.h>
#include <tb/kr/krThread/v_02/ap_krThreadTbl.h>
#include <tb/kr/krTimer/v_02/ap_krTimerTbl.h>
#include <tb/sv/svStateMachine/v_02/ap_svStateMachineTbl.h>

// ***************************************************

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include <drivers/freertos/fsl_usart_freertos.h>
#include "fsl_usart.h"

#include <stdbool.h>

// ****

#include "fsl_usart.h"
#include "fsl_gpio.h"


// ****
#include "ds3231.h"


// ****

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54608.h"
#include "fsl_debug_console.h"

#include <wrapper/freertos/emphi/th/thThread.h>

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */


#include <atomic>

std::atomic<std::uint32_t> 	global_atomic_var_work;
uint32_t 					global_var_work;
uint32_t 					global_var;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_IRQHandler FLEXCOMM0_IRQHandler
#define DEMO_USART_IRQn FLEXCOMM0_IRQn
/* Task priorities. */
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)
#define USART_NVIC_PRIO 5

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_DEBUG_UART_MARCO_TEST_CLK_ATTACH   kFRO12M_to_FLEXCOMM4

#define DEMO_USART_MARCO_TEST                    USART4
#define DEMO_USART_CLK_FREQ                      CLOCK_GetFlexCommClkFreq(0)
#define DEMO_USART_MARCO_TEST_CLK_FREQ           CLOCK_GetFlexCommClkFreq(0)

// *********************************************
#define APP_BOARD_MARCO_TEST_SW2_PORT 0U
#define APP_BOARD_MARCO_TEST_SW2_PIN  6U

#define APP_BOARD_MARCO_TEST_LED2_PORT 3U
#define APP_BOARD_MARCO_TEST_LED2_PIN  3U

// *********************************************
#define APP_BOARD_MARCO_TEST_SW3_PORT 0U
#define APP_BOARD_MARCO_TEST_SW3_PIN  5U

#define APP_BOARD_MARCO_TEST_LED3_PORT 2U
#define APP_BOARD_MARCO_TEST_LED3_PIN  2U


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
//                                            1         2         3         4         5         6
//                                  012345678901234567890123456789012345678901234567890123456789012345678
uint8_t txbuff[]                  = "Usart polling example\r\nPress any key to continue ...\r\n";
uint8_t txbuff_marco[]            = "OK! rs485 Tx works! \r\n Now, press any key to test rs485 Rx ...\r\n";
uint8_t rxbuff[20]                = {0};
// ..................................0123456789012345678901234567890
uint8_t txbuff_show_rx[]          = "the key just pressed is: -\r\n";
uint8_t txbuff_first_msg[]        = "Welcome to USART MARCO test\r\n";

uint8_t txbuff_sw2_status[]       = "sw2 (two) pressed\r\n";
uint8_t txbuff_led2_status_on[]   = "Pin connected to led2 (two) is HIGH\r\n";
uint8_t txbuff_led2_status_off[]  = "Pin connected to led2 (two) is LOW\r\n";

uint8_t txbuff_sw3_status[]       = "sw3 (three) pressed\r\n";
uint8_t txbuff_led3_status_on[]   = "Pin connected to led3 (three) is HIGH\r\n";
uint8_t txbuff_led3_status_off[]  = "Pin connected to led3 (three) is LOW\r\n";


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
const char *to_send             = "cmkLib T FreeRTOS (static task) USART driver example!\r\n";
const char *send_buffer_overrun = "\r\nRing buffer overrun!\r\n";
uint8_t background_buffer[32];
uint8_t recv_buffer[4];

usart_rtos_handle_t handle;
struct _usart_handle t_handle;

struct rtos_usart_config usart_config = {
    .baudrate    = 115200,
    .parity      = kUSART_ParityDisabled,
    .stopbits    = kUSART_OneStopBit,
    .buffer      = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

#define STACK_SIZE 200



// Structure that will hold the TCB of the task being created.
StaticTask_t xTaskBuffer;

// Buffer that the task being created will use as its stack.  Note this is
// an array of StackType_t variables.  The size of StackType_t is dependent on
// the RTOS port.
StackType_t xStack[ STACK_SIZE ];

void vTaskCodeTest01( void * pvParameters )
{
    // The parameter value is expected to be 1 as 1 is passed in the
    // pvParameters value in the call to xTaskCreateStatic().
    configASSERT( ( uint32_t ) pvParameters == 1UL );
    uint32_t i=0;
    uint32_t local_var_work=0;
    global_var = 1;

    while(1) {
    	local_var_work=global_var_work;
    	local_var_work++;
    	global_var_work++;

    	if(global_var != 1) {
    		if(local_var_work != global_var_work) {
        		PRINTF("Task switch occurred at 0x%x cycle of task 01\r\n",i);
    		}
    		global_var = 1;
    	}
    	i++;
    }
}

void vTaskCodeTest02( void * pvParameters )
{
    // The parameter value is expected to be 1 as 1 is passed in the
    // pvParameters value in the call to xTaskCreateStatic().
    configASSERT( ( uint32_t ) pvParameters == 1UL );
    uint32_t i=0;
    uint32_t local_var_work=0;
    global_var = 2;

    while(1) {
    	local_var_work=global_var_work;
    	local_var_work++;
    	global_var_work++;

    	if(global_var != 2) {
    		if(local_var_work != global_var_work) {
    			PRINTF("Task switch occurred at 0x%x cycle of task 02\r\n",i);
    		}
			global_var = 2;
    	}
    	i++;
    }
}

void vTaskCodeTestAtomic01( void * pvParameters )
{
    // The parameter value is expected to be 1 as 1 is passed in the
    // pvParameters value in the call to xTaskCreateStatic().
    configASSERT( ( uint32_t ) pvParameters == 1UL );
    uint32_t i=0;
    uint32_t local_var_work=0;
    global_var = 1;

    while(1) {
    	local_var_work=global_atomic_var_work;
    	local_var_work++;
    	global_atomic_var_work++;

    	if(global_var != 1) {
    		if(local_var_work != global_atomic_var_work) {
        		PRINTF("Task switch occurred at 0x%x cycle of task 01\r\n",i);
    		}
    		global_var = 1;
    	}
    	i++;
    }
}

void vTaskCodeTestAtomic02( void * pvParameters )
{
    // The parameter value is expected to be 1 as 1 is passed in the
    // pvParameters value in the call to xTaskCreateStatic().
    configASSERT( ( uint32_t ) pvParameters == 1UL );
    uint32_t i=0;
    uint32_t local_var_work=0;
    global_var = 2;

    while(1) {
    	local_var_work=global_atomic_var_work;
    	local_var_work++;
    	global_atomic_var_work++;

    	if(global_var != 2) {
    		if(local_var_work != global_atomic_var_work) {
    			PRINTF("Task switch occurred at 0x%x cycle of task 02\r\n",i);
    		}
			global_var = 2;
    	}
    	i++;
    }
}

// Function that implements the task being created.
void vTaskCode( void * pvParameters )
{
    // The parameter value is expected to be 1 as 1 is passed in the
    // pvParameters value in the call to xTaskCreateStatic().
    configASSERT( ( uint32_t ) pvParameters == 1UL );

    uint8_t array_ch[5];
    uint8_t rtc_data;
    uint8_t retVal;
    uint8_t rtc_data_array[7];

    while (1)
    {
//        // ******************************************************************************************
//        // from serial DEFAULT
//        USART_WriteBlocking(DEMO_USART, txbuff, sizeof(txbuff) - 1);
//        // waiting start test: press any key on st_link (minicom - lpcxpresso54608) terminal
//        USART_ReadBlocking (DEMO_USART, &ch, 1);
//        txbuff_show_rx[25] = ch;
//        USART_WriteBlocking(DEMO_USART, txbuff_show_rx, sizeof(txbuff_show_rx) - 1);

        // ******************************************************************************************
        // from serial/rs485 MARCO_TEST
        // N.B.:
        //       - marrone sul led2 (port3 - pin3) accoppiato a sw2 -> DE
        //       - bianco  sul led3 (port2 - pin2) accoppiato a sw3 -> RE
        // ******************************************************************************************
        // first message
        //USART_WriteBlocking(DEMO_USART_MARCO_TEST, txbuff_marco, sizeof(txbuff_marco) - 1);
        rsSerial::Tx(txbuff_marco);

        // ******************************************************************************************
        // tx disable
        GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED2_PORT, APP_BOARD_MARCO_TEST_LED2_PIN, 0);
        // rx enable
        GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED3_PORT, APP_BOARD_MARCO_TEST_LED3_PIN, 0);

        // waiting start RS485 RX test: press any key on minicom (by minicom.rs485_dsdtech) terminal
        // USART_ReadBlocking(DEMO_USART_MARCO_TEST, array_ch, size_of(array_ch));
        rsSerial::Rx(array_ch, sizeof(array_ch) - 1);

        txbuff_marco[0] = array_ch[0];
        txbuff_marco[1] = array_ch[1];
        txbuff_marco[2] = array_ch[2];
        txbuff_marco[3] = array_ch[3];

        txbuff_marco[4] = ' ';
        txbuff_marco[5] = '-';
        txbuff_marco[6] = ' ';

        PRINTF("Rx data from rs485 : %02d - %02d - %02d - %02d\r\n",	array_ch[0],
        		array_ch[1],
				array_ch[2],
				array_ch[3]
    			);

        if (array_ch[0] == 'w') {
        	if ( (array_ch[2] >= 0x30) && (array_ch[2] <= 0x39) &&
        		 (array_ch[3] >= 0x30) && (array_ch[3] <= 0x39) ) {
        		rtc_data = (array_ch[2] - 0x30) * 10 + (array_ch[3] - 0x30);

                if (array_ch[1] == 'h') {
                	ds3231_set	(HOUR,   &rtc_data);
                } else
                if (array_ch[1] == 'm') {
                	ds3231_set	(MINUTE, &rtc_data);
                } else
                if (array_ch[1] == 's') {
                	ds3231_set	(SECOND, &rtc_data);
                }

                rtc_data_array[0] = 0;	// seconds
                rtc_data_array[1] = 0;	// minutes
                rtc_data_array[2] = 0;	// hours
                rtc_data_array[3] = 0;	// week day
                rtc_data_array[4] = 0;	// day
                rtc_data_array[5] = 0;	// month
                rtc_data_array[6] = 0;  // year

                retVal = ds3231_read	(TIME, rtc_data_array);
                PRINTF("ds3231_read status retVal: %d\r\n",retVal);

                PRINTF("Rx data from rs485 : %d/%d/%d/ - %d - %d:%d:%d\r\n",	rtc_data_array[4],
                					    										rtc_data_array[5],
            																	rtc_data_array[6],
            																	rtc_data_array[3],
            																	rtc_data_array[2],
            																	rtc_data_array[1],
            																	rtc_data_array[0]
            			);
        	}
        }

        if (array_ch[0] == 'x') {
        	vTaskSuspend(NULL);
        } else
        if (array_ch[0] == 'r') {
            retVal = ds3231_read	(TIME, rtc_data_array);
            PRINTF("ds3231_read status retVal: %d\r\n",retVal);

            PRINTF("Rx data from rs485 : %d/%d/%d/ - %d - %d:%d:%d\r\n",	rtc_data_array[4],
            					    										rtc_data_array[5],
        																	rtc_data_array[6],
        																	rtc_data_array[3],
        																	rtc_data_array[2],
        																	rtc_data_array[1],
        																	rtc_data_array[0]
																						   );
        }


        // tx enable
        GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED2_PORT, APP_BOARD_MARCO_TEST_LED2_PIN, 1);
        // rx disable
        GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED3_PORT, APP_BOARD_MARCO_TEST_LED3_PIN, 1);
    }


    for( ;; )
    {
        // Task code goes here.
    }
}

void uart_task(void *pvParameters)
{
    int error;
    size_t n            = 0;
    usart_config.srcclk = BOARD_DEBUG_UART_CLK_FREQ;
    usart_config.base   = DEMO_USART;

    NVIC_SetPriority(DEMO_USART_IRQn, USART_NVIC_PRIO);

    if (0 > USART_RTOS_Init(&handle, &t_handle, &usart_config))
    {
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */
    if (0 > USART_RTOS_Send(&handle, (uint8_t *)to_send, strlen(to_send)))
    {
        vTaskSuspend(NULL);
    }

    /* Receive user input and send it back to terminal. */
    do
    {
        error = USART_RTOS_Receive(&handle, recv_buffer, sizeof(recv_buffer), &n);
        if (error == kStatus_USART_RxRingBufferOverrun)
        {
            /* Notify about hardware buffer overrun */
            if (kStatus_Success !=
                USART_RTOS_Send(&handle, (uint8_t *)send_buffer_overrun, strlen(send_buffer_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (n > 0)
        {
            /* send back the received data */
            USART_RTOS_Send(&handle, (uint8_t *)recv_buffer, n);
        }
    } while (kStatus_Success == error);

    USART_RTOS_Deinit(&handle);
    vTaskSuspend(NULL);
}


// Function that creates a task.
void vOtherFunction( void )
{
    TaskHandle_t xHandle = NULL;

    // Create the task without using any dynamic memory allocation.
    xHandle = xTaskCreateStatic(
    		      uart_task,             // Function that implements the task.
                  "NAME",                // Text name for the task.
                  STACK_SIZE,            // Stack size in words, not bytes.
                  ( void * ) 1,          // Parameter passed into the task.
				  uart_task_PRIORITY,    // Priority at which the task is created.
                  xStack,                // Array to use as the task's stack.
                  &xTaskBuffer );        // Variable to hold the task's data structure.

    // puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
    // been created, and xHandle will be the task's handle.  Use the handle
    // to suspend the task.
    vTaskSuspend( xHandle );
}

/*!
 * @brief Task responsible for loopback.
 */
/*
 * @brief   Application entry point.
 */
int main(void) {
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);


//    // ***************************************************************************
//    rsSerial::Tx("START  - Init static functions"); rsSerial::Tx(rsSerial::endl);
//    rsSerial::Tx(rsSerial::endl);
    krInitStaticManager::LoopOnUnits(ap_krInitStaticTbl::krInitStaticPtrArray);
    rsSerial::Tx(rsSerial::endl);

    /* attach 12 MHz clock to FLEXCOMM8 (I2C master) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM8);
    /* reset FLEXCOMM for I2C */
    RESET_PeripheralReset(kFC8_RST_SHIFT_RSTn);
    BOARD_InitBootPins();


    /* Init board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    // CLOCK_AttachClk(BOARD_DEBUG_UART_MARCO_TEST_CLK_ATTACH);
    // BOARD_BootClockPLL180M();
    BOARD_InitDebugConsole();


    // ***************************************************************************
    // ...........1234567890123456789012345678901234567890
    rsSerial::Tx("START  - Main"); rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    // ...........1234567890123456789012345678901234567890
    rsSerial::Tx("START  - Setup Timer Service"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    krTimerManager::SetUnitPtrArrayPtr(ap_krTimerTbl::TimerPtrArray);
    rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    // ...........1234567890123456789012345678901234567890
    rsSerial::Tx("START  - Setup State Machine Service"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    svStateMachineManager::SetUnitsArrayPtr(ap_svStateMachineTbl::UnitsArray);
    rsSerial::Tx(rsSerial::endl);

/*
    // ***************************************************************************
    rsSerial::Tx("START  - Threads definition and then run them"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    krThreadManager::InitThenRun(ap_krThreadTbl::LoopFunctionArray);
    rsSerial::Tx(rsSerial::endl);

    do {
        wp_sleep_for(10);
    } while (tt_mn_svSyncMain::CheckRaceConditionCounter(10));
    
    // ***************************************************************************
    krThreadManager::Exit();
    rsSerial::Tx("EXIT  - from Main Loop"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    rsSerial::Tx("SHOW  - Tests results"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    tt_MainManager::LoopOnUnits();
    rsSerial::Tx(rsSerial::endl);
// *****************************************************
*/
    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;


// ****
    // uint8_t ch;
//    usart_config_t config;

/*
    // BOARD_InitPins();
    // BOARD_BootClockPLL180M();
    // BOARD_InitDebugConsole();


    GPIO_PortInit(GPIO, APP_BOARD_MARCO_TEST_SW2_PORT );
    GPIO_PortInit(GPIO, APP_BOARD_MARCO_TEST_LED2_PORT);

    GPIO_PortInit(GPIO, APP_BOARD_MARCO_TEST_SW3_PORT );
    GPIO_PortInit(GPIO, APP_BOARD_MARCO_TEST_LED3_PORT);


    // Define the init structure for the output LED pin
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    GPIO_PinInit (GPIO, APP_BOARD_MARCO_TEST_LED2_PORT, APP_BOARD_MARCO_TEST_LED2_PIN, &led_config);
    // rs485 tx enable
    GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED2_PORT, APP_BOARD_MARCO_TEST_LED2_PIN, 1);

    GPIO_PinInit (GPIO, APP_BOARD_MARCO_TEST_LED3_PORT, APP_BOARD_MARCO_TEST_LED3_PIN, &led_config);
    // rs485 rx disable
    GPIO_PinWrite(GPIO, APP_BOARD_MARCO_TEST_LED3_PORT, APP_BOARD_MARCO_TEST_LED3_PIN, 1);
*/
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUSART_ParityDisabled;
     * config.stopBitCount = kUSART_OneStopBit;
     * config.loopback = false;
     * config.enableTx = false;
     * config.enableRx = false;
     */
/*
    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    // USART_Init(DEMO_USART,            &config, DEMO_USART_CLK_FREQ             );
    USART_Init(DEMO_USART_MARCO_TEST, &config, DEMO_USART_MARCO_TEST_CLK_FREQ  );
*/
    // USART_WriteBlocking(DEMO_USART,            txbuff_first_msg, sizeof(txbuff_first_msg) - 1);
    USART_WriteBlocking(DEMO_USART_MARCO_TEST, txbuff_first_msg, sizeof(txbuff_first_msg) - 1);


// ****
    uint8_t rtc_data_array[7];
    uint8_t rtc_data_array_rx[7];

    PRINTF("Receive sent data from slave :\r\n");

    status_t retVal        = kStatus_Fail;
    ds3231_I2C_init();
    retVal = ds3231_init_status_report();
    PRINTF("ds3231_init_status_report retVal: %d\r\n",retVal);

    // rtc_data_array[0] = 43;	// seconds
    // rtc_data_array[1] = 46;	// minutes
    // rtc_data_array[2] =  9;	// hours
    // rtc_data_array[3] =  1;	// week day
    // rtc_data_array[4] = 29;	// day
    // rtc_data_array[5] =  8;	// month
    // rtc_data_array[6] = 22;  // year

    PRINTF("Tx data to slave : %d/%d/%d/ - %d - %d:%d:%d\r\n",	rtc_data_array[4],
    															rtc_data_array[5],
																rtc_data_array[6],
																rtc_data_array[3],
																rtc_data_array[2],
																rtc_data_array[1],
																rtc_data_array[0]
			);

    retVal = ds3231_init(rtc_data_array, CLOCK_RUN, NO_FORCE_RESET);
    PRINTF("ds3231_init status retVal: %d\r\n",retVal);

    // retVal = ds3231_set	(TIME, rtc_data_array);
    // PRINTF("ds3231_set status retVal: %d\r\n",retVal);

    retVal = ds3231_run_status	();
    PRINTF("ds3231_run_status status retVal: %d\r\n",retVal);

    if (kStatus_DS3231_Clock_Halt == retVal) {
        retVal = ds3231_run_command	(CLOCK_RUN);
        PRINTF("ds3231_run_command status retVal: %d\r\n",retVal);

        retVal = ds3231_run_status	();
        PRINTF("ds3231_run_status status retVal: %d\r\n",retVal);
    }

    rtc_data_array_rx[0] = 0;	// seconds
    rtc_data_array_rx[1] = 0;	// minutes
    rtc_data_array_rx[2] = 0;	// hours
    rtc_data_array_rx[3] = 0;	// week day
    rtc_data_array_rx[4] = 0;	// day
    rtc_data_array_rx[5] = 0;	// month
    rtc_data_array_rx[6] = 0;  // year

    retVal = ds3231_read	(TIME, rtc_data_array_rx);
    PRINTF("ds3231_read status retVal: %d\r\n",retVal);

    PRINTF("Rx data from slave : %d/%d/%d/ - %d - %d:%d:%d\r\n",	rtc_data_array_rx[4],
    					    										rtc_data_array_rx[5],
																	rtc_data_array_rx[6],
																	rtc_data_array_rx[3],
																	rtc_data_array_rx[2],
																	rtc_data_array_rx[1],
																	rtc_data_array_rx[0]
			);



// ****


//	TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,
//									const char * const pcName,		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
//									const uint32_t ulStackDepth,
//									void * const pvParameters,
//									UBaseType_t uxPriority,
//									StackType_t * const puxStackBuffer,
//									StaticTask_t * const pxTaskBuffer )

//	BaseType_t xTaskCreate(	TaskFunction_t pxTaskCode,
//							const char * const pcName,		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
//							const configSTACK_DEPTH_TYPE usStackDepth,
//							void * const pvParameters,
//							UBaseType_t uxPriority,
//							TaskHandle_t * const pxCreatedTask )

    PRINTF("Start example BARE01 xTaskCreateStatic.\r\n");
    if (xTaskCreate(uart_task, "Uart_task", configMINIMAL_STACK_SIZE + 100, NULL, uart_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreateStatic(vTaskCode ,"Uart_static_task",STACK_SIZE,( void * ) 1,uart_task_PRIORITY,xStack,&xTaskBuffer ) == NULL)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

//    vOtherFunction();
    vTaskStartScheduler();
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }

    return 0 ;
}


