//
// Created by August R. Mason on 2/11/18.
//

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stdio.h"


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
        HAL_GPIO_TogglePin(PC4_LED_GPIO_Port, PC4_LED_Pin);
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
        switchMessage [4] = HAL_GPIO_ReadPin(KillSwitch_GPIO_Port, KillSwitch_Pin)      ? '1' : '0';
        switchMessage [5] = HAL_GPIO_ReadPin(MissionStart_GPIO_Port, MissionStart_Pin)  ? '1' : '0';
        switchMessage [6] = HAL_GPIO_ReadPin(Switch3_GPIO_Port, Switch3_Pin)            ? '1' : '0';
        switchMessage [7] = HAL_GPIO_ReadPin(Switch4_GPIO_Port, Switch4_Pin)            ? '1' : '0';
        switchMessage [8] = HAL_GPIO_ReadPin(Switch5_GPIO_Port, Switch5_Pin)            ? '1' : '0';
        switchMessage [9] = HAL_GPIO_ReadPin(Switch6_GPIO_Port, Switch6_Pin)            ? '1' : '0';
        CDC_Transmit_HS(switchMessage, sizeof(switchMessage));
        vTaskDelay(900);
    }
}

void vKillSwitch ( void *pvParameters ){
  //variables ?
  for(;;){
    if( HAL_GPIO_ReadPin(KillSwitch_GPIO_Port,KillSwitch_Pin) == GPIO_PIN_RESET){
      resetPWM();
      HAL_GPIO_WritePin(PC4_LED_GPIO_Port, PC4_LED_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(PC4_LED_GPIO_Port, PC4_LED_Pin, GPIO_PIN_SET);
    }
    vTaskDelay(100);
  }
}

void vDepthSensor(void *pvParameters) {
	// this is the reset point, in case of failure to initialize;
	resetI2C: vTaskDelay(100);

	uint16_t address_write = 0xEC;
	uint16_t address_read = 0xED;
	uint8_t reset = 0x1E;
	uint8_t prom_read = 0xA0;
	uint8_t prom_val = prom_read;
	uint8_t read_d1 = 0x4A;
	uint8_t read_d2 = 0x5A;
	uint8_t read_adc = 0x00;

	uint16_t calibration[8];
	uint8_t buffer[3];

	uint32_t d1;
	uint32_t d2;

	uint32_t temp;
	uint32_t press;

	float pressure;
	float temperature;
	float depth;
	float altitude;
	uint16_t fluidDensity = 1029;

	I2C_HandleTypeDef *i2c = getI2CRef();

	HAL_I2C_Master_Transmit(i2c, address_write, &reset, 1, 20);

	vTaskDelay(50);

	for (uint8_t i = 0; i < 7; i++) {

		HAL_I2C_Master_Transmit(i2c, address_write, &prom_val, 1, 20);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, address_read, buffer, 2, 20);
		calibration[i] = (buffer[0] << 8) | buffer[1];

		prom_val += 2;

	}

	uint8_t crcRead = calibration[0] >> 12;
	uint8_t crcCalc = crc4(calibration);

	uint8_t success[] = "initialization succeeded\r\n";
	uint8_t failure[] = "initialization failed\r\n";

	if (crcRead == crcCalc) {
		CDC_Transmit_HS(success, strlen(success));
	} else {
		CDC_Transmit_HS(failure, strlen(failure));
		goto resetI2C;
	}


	uint8_t values[20];
	for(;;) {

		HAL_I2C_Master_Transmit(i2c, address_write, &read_d1, 1, 20);
		vTaskDelay(20);
		HAL_I2C_Master_Transmit(i2c, address_write, &read_adc, 1, 20);
		vTaskDelay(20);

		HAL_I2C_Master_Receive(i2c, address_read, buffer, 3, 20);
		d1 = buffer[0];
		d1 = d1 << 8 | buffer[1];
		d1 = d1 << 8 | buffer[2];

		HAL_I2C_Master_Transmit(i2c, address_write, &read_d2, 1, 30);
		vTaskDelay(20);
		HAL_I2C_Master_Transmit(i2c, address_write, &read_adc, 1, 30);
		vTaskDelay(20);

		HAL_I2C_Master_Receive(i2c, address_read, buffer, 3, 30);
		d2 = buffer[0];
		d2 = d2 << 8 | buffer[1];
		d2 = d2 << 8 | buffer[2];

		calculate(d1, d2, calibration, &temp, &press);
		convert(&temp, &press, &temperature, &pressure, &depth, fluidDensity);

		memset(values, 0, sizeof(values));
		gcvt(temperature, 20, values);
		CDC_Transmit_HS(values, strlen(values));
		vTaskDelay(5);
		CDC_Transmit_HS("\r\n", 2);

		vTaskDelay(10);

		memset(values, 0, sizeof(values));
		gcvt(pressure, 20, values);
		CDC_Transmit_HS(values, strlen(values));
		vTaskDelay(5);
		CDC_Transmit_HS("\r\n", 2);

		vTaskDelay(10);

		memset(values, 0, sizeof(values));
		gcvt(depth, 20, values);
		CDC_Transmit_HS(values, strlen(values));
		vTaskDelay(5);
		CDC_Transmit_HS("\r\n", 2);


		vTaskDelay(200);
	}
}
