/**
  ******************************************************************************
  * @file    radar_sensor.c 
  * @author  Marco Dau
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "radar_sensor.h"
#include "radar_sensor_log.h"
#include "radar_sensor_timers.h"

#define SENSOR_LED_PIN                        GPIO_PIN_7
#define SENSOR_INPUT_1_PIN                    GPIO_PIN_3
#define SENSOR_INPUT_2_PIN                    GPIO_PIN_4

#define SENSOR_LED_GPIO_PORT                  GPIOF
#define SENSOR_INPUTS_GPIO_PORT               GPIOJ

#define SENSOR_LED_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()
#define SENSOR_LED_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOF_CLK_DISABLE()
#define SENSOR_INPUTS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOJ_CLK_ENABLE()
#define SENSOR_INPUTS_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOJ_CLK_DISABLE()


typedef struct
{
  DIn_Status    status_cur;
  DIn_Status    status_old;
  GPIO_TypeDef* port;
  uint16_t      pin;
} DIn_TypeDef;

DIn_TypeDef radar_sensor;
DIn_TypeDef input[InputType_num];

uint8_t log_event_message[LOG_MESSAGE_LENGTH];
uint8_t g_log_event_counter = 0;
uint8_t g_step = 5;
uint8_t lcd_brightness_level;
uint8_t lcd_brightness_timer;

void radar_sensor_input_check   (InputType p_input);
void lcd_on (void);
void lcd_off (void);
void lcd_timer_update (void);

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

/* Private function prototypes -----------------------------------------------*/
static void RadarSensor_SetHint(void);
static void SensorInputs_Init(void);
static void SensorLed_Init(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  SD Demo
  * @param  None
  * @retval None
  */
void RadarSensor_demo (void)
{
    SdmmcTest = 1;

    SensorLed_Init();
    SensorInputs_Init();
    RadarSensor_SetHint();

    g_step = 5;
    g_log_event_counter = 0;

    if(0 == radar_sensor_log_init(log_event_message))
    {
        lcd_brightness_level = 4;
        timer_lcd = 30;

        BSP_LCD_DisplayStringAt(20, 100 + 15*g_step, (uint8_t *)"Step 5 - Logger loop running ...", LEFT_MODE);
        g_step++;

        while (BSP_PB_GetState(BUTTON_USER) != SET) {
            uyRTC_Update();
            radar_sensor_input_check(InputType_button);
            radar_sensor_input_check(InputType_sensor_out);
            lcd_timer_update();
            lcd_off();
        }
        BSP_LCD_DisplayStringAt(20, 100 + 15*(g_step + MAX_LOG_EVENT_LINE), (uint8_t *)"Step 6 - Logger loop ended. Bye, bye.", LEFT_MODE);
        g_step++;
        
        /* Prevent debounce effect for user key */
        HAL_Delay(400);
    }
        
    if (CheckForUserInput() > 0) {
        radar_sensor_log_deinit(g_step);
    }
  
    SdmmcTest = 0;
}

void lcd_on (void)
{
    BSP_LCD_DisplayOn();
    BSP_LCD_SetBrightness(100);
    lcd_brightness_level = 4;
    timer_lcd = 30;

}

void lcd_off (void)
{
    if(timer_lcd == 0) 
    {
        if(lcd_brightness_level)
        {
            if(lcd_brightness_level > 4) lcd_brightness_level = 4;
            lcd_brightness_level--;
            timer_lcd = 30;
            BSP_LCD_SetBrightness(lcd_brightness_level*25);
        }
        if(0 == lcd_brightness_level)
        {
            BSP_LCD_SetBrightness(0);
            BSP_LCD_DisplayOff();
        }   
    }
}

void lcd_timer_update (void)
{
    if(timer_lcd_update)
    {
        timer_lcd_update = 0;
        if(timer_lcd) timer_lcd--;
    }
}

void lcd_timer_SysTick (void)
{
    if(g_timer_lcd_SysTick)
    {
        if(g_timer_lcd_SysTick > 1000) g_timer_lcd_SysTick = 1000;
        g_timer_lcd_SysTick--;
    }
    else
    {
        g_timer_lcd_SysTick = 999;
        timer_lcd_update = 1;
    }
}

void radar_sensor_input_check   (InputType p_input)
{
    input[p_input].status_cur = HAL_GPIO_ReadPin(input[p_input].port, input[p_input].pin);
    if(InputType_button == p_input) HAL_Delay(100); /* Prevent debounce effect */
    if(input[p_input].status_old == DIn_Status_ON) {
        // old radar sensor status is ON
        if(input[p_input].status_cur == DIn_Status_OFF) {
            // current radar sensor status is OFF
            // 1. reset old status
            input[p_input].status_old = DIn_Status_OFF;
            // 2. log the event on "logs.txt" file
            // 2.1. read date&time from RTC
            uyRTC_DateTimeGet(log_event_message);
            // 2.2 display sensor event
            // ..................................................................................... 0         1         2         3         4         5
            // ..................................................................................... 0123456789012345678901234567890123456789012345678901
            BSP_LCD_DisplayStringAt(20,      100 + 15*(g_step+g_log_event_counter), (uint8_t *)"log event - Radar sensor OFF. ", LEFT_MODE);
            log_event_message[24] = 'A'+p_input; log_event_message[26] = 'O'; log_event_message[27] = 'F'; log_event_message[28] = 'F';
            log_event_message[29] = 0;
            BSP_LCD_DisplayStringAt(20+30*7, 100 + 15*(g_step+g_log_event_counter), log_event_message, LEFT_MODE);
            if(g_log_event_counter < MAX_LOG_EVENT_LINE) g_log_event_counter++; else g_log_event_counter=0;
            // 2.3. write log message
            //radar_sensor_log(log_event_message, InputType_sensor_out, DIn_Status_OFF);
            radar_sensor_log(log_event_message);

            // 2.4. turn off the led
            if(InputType_sensor_out == p_input) HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_RESET);
            if(InputType_button == p_input) lcd_on();
        }
    } else {
        // old radar sensor status is OFF
        if(input[p_input].status_cur == DIn_Status_ON) {
            // current radar sensor status is ON
            // 1. set old status
            input[p_input].status_old = DIn_Status_ON;
            // 2. log the event on "logs.txt" file
            // 2.1. read date&time from RTC
            uyRTC_DateTimeGet(log_event_message);
            // 2.2 display sensor event
            // ..................................................................................... 0         1         2         3         4         5
            // ..................................................................................... 0123456789012345678901234567890123456789012345678901
            BSP_LCD_DisplayStringAt(20,      100 + 15*(g_step+g_log_event_counter), (uint8_t *)"log event - Radar sensor ON.  ", LEFT_MODE);
            log_event_message[24] = 'A'+p_input; log_event_message[26] = 'O'; log_event_message[27] = 'N'; log_event_message[28] = ' ';
            log_event_message[29] = 0;
            BSP_LCD_DisplayStringAt(20+30*7, 100 + 15*(g_step+g_log_event_counter), log_event_message, LEFT_MODE);
            if(g_log_event_counter < MAX_LOG_EVENT_LINE) g_log_event_counter++; else g_log_event_counter=0;
            // 2.3. write log message
            //radar_sensor_log(log_event_message, InputType_sensor_out, DIn_Status_OFF);
            radar_sensor_log(log_event_message);

            // 2.4. turn on the led
            if(InputType_sensor_out == p_input)
            {
                HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_SET); 
                lcd_on();
            }

        }
    }
}

/**
  * @brief  Display SD Demo Hint
  * @param  None
  * @retval None
  */
static void RadarSensor_SetHint(void)
{
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    /* Set LCD Demo description */
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 80);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"RADAR SENSOR", CENTER_MODE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"This example shows how to manage radar sensor input", CENTER_MODE);

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

    HAL_GPIO_WritePin(SENSOR_LED_GPIO_PORT, SENSOR_LED_PIN, GPIO_PIN_RESET);
}

static void SensorInputs_Init(void) {

    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the SENSOR INPUTS clock */
    SENSOR_INPUTS_GPIO_CLK_ENABLE();

    /* Configure sensor inputs pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;

    // push button
    input[InputType_button].status_old  = DIn_Status_OFF;
    input[InputType_button].port        = SENSOR_INPUTS_GPIO_PORT;
    input[InputType_button].pin         = SENSOR_INPUT_1_PIN;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Pin  = input[InputType_button].pin;
    HAL_GPIO_Init(input[InputType_button].port, &gpio_init_structure);

    // radar sensor
    input[InputType_sensor_out].status_old  = DIn_Status_OFF;
    input[InputType_sensor_out].port        = SENSOR_INPUTS_GPIO_PORT;
    input[InputType_sensor_out].pin         = SENSOR_INPUT_2_PIN;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Pin  = input[InputType_sensor_out].pin;
    HAL_GPIO_Init(input[InputType_sensor_out].port, &gpio_init_structure);

}

/**
  * @}
  */ 

/**
  * @}
  */ 

