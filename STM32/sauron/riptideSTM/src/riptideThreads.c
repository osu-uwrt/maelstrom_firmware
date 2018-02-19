//
// Created by August R. Mason on 2/11/18.
//

#include <stdint.h>
#include <memory.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptidethreads.h"



void vHeartbeat(void *pvParameters ){

    uint8_t HiMsg[] = "heartbeat\r\n";
    /* Infinite loop */
    for(;;) {
        HAL_GPIO_TogglePin(HeartBeat1_GPIO_Port, HeartBeat1_Pin);
        //to note here, we do not nead the sizeof(msg)/sizeof(uint8_t)
        CDC_Transmit_HS(HiMsg, sizeof(HiMsg));
        vTaskDelay(1000);

    }

}

void vSwitchMonitor(void *pvParameters){
    uint8_t switchMessage[14] = "$$$$";
    //uint8_t switchMessage = "";
    for(;;){
       //switchOne = HAL_GPIO_ReadPin(Switch5_GPIO_Port, Switch5_Pin);

        //strcat(switchst,switchend);
        CDC_Transmit_HS(switchMessage, sizeof(switchMessage));
        vTaskDelay(100);
    }
}