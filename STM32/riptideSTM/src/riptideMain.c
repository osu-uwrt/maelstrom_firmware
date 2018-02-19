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

void riptideMain(){
    //create task handlers here
    xTaskHandle xHeartbeat;
    xTaskHandle xSwitchMonitor;
    //create tasks here
    xTaskCreate( vHeartbeat, "Heartbeat", configMINIMAL_STACK_SIZE, NULL, 1, &xHeartbeat);
    xTaskCreate( vSwitchMonitor, "SwitchMonitor", configMINIMAL_STACK_SIZE, NULL, 1, &xSwitchMonitor);
    //create queues here
    //create mutexes here
    //create semaphores here
    return;
}