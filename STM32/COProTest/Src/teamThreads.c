//
// Created by August R. Mason on 9/8/17.
//
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "teamThreads.h"
#include "teamDepthSensor.h"

//Heartbeat thread function


void vHeartbeat(void *pvParameters ){
    struct depthSensor sensor;     //define the sensor
    MX_I2C1_Init();
    initializeSensor(&sensor, &hi2c1, 997.0);

    for( ;; ){
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
      HAL_Delay(500);
      }
      vTaskDelete(NULL);
}
/*
void vDepthsensor(void *pvParameters ){
    //define sensor
    struct depthSensor sensor;
    //initialize sensor
    sensor.sclPin = depthSCL_Pin;
    sensor.sdaPin = depthSDA_Pin;

    for (;; ){
        //read the sensor

    }
}
*/

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}
