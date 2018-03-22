//
// Created by August R. Mason on 2/11/18.
//

//setup all your stuff here, so we barely touch main.h
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptidethreads.h"

xTaskHandle xHeartbeat;
xTaskHandle xSwitchMonitor;
xTaskHandle xKillSwitch;
xTaskHandle xDepthSensor;

void riptideMain(){
    //create tasks here
    xTaskCreate( vHeartbeat, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, 1, &xHeartbeat);
    xTaskCreate( vSwitchMonitor, "SwitchMonitor", configMINIMAL_STACK_SIZE, NULL, 1, &xSwitchMonitor);
    xTaskCreate( vKillSwitchMonitor, "KillSwitch", configMINIMAL_STACK_SIZE, NULL, 1, &xKillSwitch);
    xTaskCreate( vDepthSensor, "DepthSensor", configMINIMAL_STACK_SIZE, NULL, 1, &xDepthSensor);
    //create mutexes here
    //create semaphores here
    return;
}

void restartI2C() {
	vTaskDelete(xDepthSensor);
	vTaskDelay(50);
	xTaskCreate( vDepthSensor, "DepthSensor", configMINIMAL_STACK_SIZE, NULL, 1, &xDepthSensor);
}

