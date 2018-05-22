//setup all your stuff here, so we barely touch main.h
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptideThreads.h"

void riptideMain(){
    //create task handlers here
    xTaskHandle xHeartbeat;
    xTaskHandle xSwitchMonitor;
    xTaskHandle xKillSwitch;
    xTaskHandle xDepthSensor;
    xTaskHandle xBackplaneSensors;
    //create tasks here
    xTaskCreate( vHeartbeat, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, 1, &xHeartbeat);
    xTaskCreate( vSwitchMonitor, "SwitchMonitor", configMINIMAL_STACK_SIZE, NULL, 2, &xSwitchMonitor);
    xTaskCreate( vKillSwitch, "KillSwitch", configMINIMAL_STACK_SIZE, NULL, 4, &xKillSwitch);
    xTaskCreate( vBackplaneI2C, "Backplane", configMINIMAL_STACK_SIZE, NULL, 3, &xBackplaneSensors);
    xTaskCreate( vDepthSensor, "DepthSensor", configMINIMAL_STACK_SIZE, NULL, 3, &xDepthSensor);
    //create queues here
    //create mutexes here
    //create semaphores here

    if( HAL_GPIO_ReadPin( KillSwitch_GPIO_Port,KillSwitch_Pin) == GPIO_PIN_RESET){
    	HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_SET);
    }

    HAL_GPIO_WritePin(LED_PC6_GPIO_Port, LED_PC6_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_PC7_GPIO_Port, LED_PC7_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_PA5_GPIO_Port, LED_PA5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_PC5_GPIO_Port, LED_PC5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_PC4_GPIO_Port, LED_PC4_Pin, GPIO_PIN_SET);

    HAL_Delay(8000);

    HAL_GPIO_WritePin(LED_PC6_GPIO_Port, LED_PC6_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PC7_GPIO_Port, LED_PC7_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PA5_GPIO_Port, LED_PA5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PC5_GPIO_Port, LED_PC5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PC4_GPIO_Port, LED_PC4_Pin, GPIO_PIN_RESET);

    return;
}
