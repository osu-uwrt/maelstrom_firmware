//
// Created by August R. Mason on 9/8/17.
//
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "usb_device.h"
#include "FreeRTOS.h"
#include "teamThreads.h"

//Heartbeat thread function


void vHeartbeat(void *pvParameters ){
    //it's an arduino, setup then loop
    //initialize the GPIO Bus clock should be initialized but re-
    __HAL_RCC_GPIOC_CLK_ENABLE(); //we are using bus C for this
    //this struct is what is needed by the HAL_GPIO_Init()
    GPIO_InitTypeDef GPIO_InitDef;
    GPIO_InitDef.Pin = GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitDef.Pull = GPIO_NOPULL;
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_HIGH;

    //Initialize your GPIO!
    HAL_GPIO_Init(GPIOC, &GPIO_InitDef);
    //**********END SETUP***********************//
    //this is really void (loop) trust me

    for( ;; ){
        //HAL_GPIO should be initialized and this should just work
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12); //This toggles the spcific pin number from high to low, an LED
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
        HAL_Delay(1000);
    }

}


void vKillStatus(void *pvParameters )
{
    //take your GPIO pins here
    int i;
    for ( ;; ){
        i = i+1;
    }
    vTaskDelete(NULL);
}

void vMoveRobot(void *pvParameters )
{
    //group the PWM values into sets of two
    /*
     * surge_stbd_low, surge_port_low
     * surge_stbd_high, surge_stbd_high
     *
     */

    for ( ;; ){

    }

    vTaskDelete(NULL);
}

//This function is to set the pwm value from the ros callback
void user_pwm_setvalue(uint16_t value)
{
    TIM_OC_InitTypeDef sConfigOC;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}