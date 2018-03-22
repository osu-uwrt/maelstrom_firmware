//
// Created by August R. Mason on 2/11/18.
//

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptidethreads.h"
//#include "../../Inc/main.h"
//#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h"

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
    uint8_t switchMessage[14] = "$$$$nnnnnn@@@@";
    //uint8_t switchMessage = "";
    for(;;){
        //You can read directly in to the the message without forcing a 1 or 0 but that returns
        //a weird block character when switch is closed which may be hard to parse later
        switchMessage [4] = HAL_GPIO_ReadPin(KillSwitch_GPIO_Port, KillSwitch_Pin)  ? '1' : '0';
        switchMessage [5] = HAL_GPIO_ReadPin(Switch1_GPIO_Port, Switch1_Pin)        ? '1' : '0';
        switchMessage [6] = HAL_GPIO_ReadPin(Switch2_GPIO_Port, Switch2_Pin)        ? '1' : '0';
        switchMessage [7] = HAL_GPIO_ReadPin(Switch2_GPIO_Port, Switch2_Pin)        ? '1' : '0';
        switchMessage [8] = HAL_GPIO_ReadPin(Switch2_GPIO_Port, Switch2_Pin)        ? '1' : '0';
        CDC_Transmit_HS(switchMessage, sizeof(switchMessage));
        vTaskDelay(900);
    }
}