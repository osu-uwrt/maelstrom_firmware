
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stdio.h"


#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptidethreads.h"


void vHeartbeat(void *pvParameters ){

    uint8_t HiMsg[] = "heartbeat\r\n";
    /* Infinite loop */
    for(;;) {
        HAL_GPIO_TogglePin(LED_PC6_GPIO_Port, LED_PC6_Pin);
        vTaskDelay(1000);
    }

}

void vSwitchMonitor(void *pvParameters){
    uint8_t switchMessage[] = "$$$$nnnnnn@@@@\r\n";
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
  uint8_t state = -1;
  for(;;){
    if( HAL_GPIO_ReadPin(KillSwitch_GPIO_Port,KillSwitch_Pin) == GPIO_PIN_RESET){
        resetPWM();
        HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_RESET);
        state = 0;
	} else {
		HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_SET);
		state = 1;
    }
    vTaskDelay(100);
  }
}

void vBackplaneI2C(void *pvParameters) {
	vTaskDelay(1500); //need this to offset scheduler, just in case

	uint8_t led_addr = 0xC0;
	uint8_t led_test = 0x07;
	uint8_t dec_mode = 0x01;
	uint8_t led_intensity = 0x02;
	uint8_t scan_limit = 0x03;
	uint8_t reg_config = 0x04;
	uint8_t digit_type = 0x0C;

	uint8_t red_intensity = 0x10;
	uint8_t red_intensity_a = 0x14;
	uint8_t green_intensity = 0x11;
	uint8_t green_intensity_a = 0x15;
	uint8_t yellow_intensity = 0x12;
	uint8_t	yellow_intensity_a = 0x16;


	uint16_t write_address = 0x3C;
	uint16_t read_address = 0x3D;
	I2C_HandleTypeDef *i2c = getBackplaneI2CRef();
	uint8_t write_data[2];

	/************* Status Board Setup ******************/

	uint8_t led_data[] = {dec_mode, 0xFF};
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send decode mode (standard)

	led_data[0] = red_intensity;
	led_data[1] = 0x0;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity
	led_data[0] = red_intensity_a;
	led_data[1] = 0x0;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity
	led_data[0] = green_intensity;
	led_data[1] = 0x11;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity
	led_data[0] = green_intensity_a;
	led_data[1] = 0x11;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity
	led_data[0] = yellow_intensity;
	led_data[1] = 0x22;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity
	led_data[0] = yellow_intensity_a;
	led_data[1] = 0x22;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // send LED intensity

	led_data[0] = scan_limit;
	led_data[1] = 0x05;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // set the scan limit (not sure why 5 though)?
	led_data[0] = reg_config;
	led_data[1] = 0x41;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // set the control register, set the global led intensity mode off
	led_data[0] = digit_type;
	led_data[1] = 0x00;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // set as a 7 segment display

	led_data[0] = led_test;
	led_data[1] = 0x00;
	vTaskDelay(10);
	HAL_I2C_Master_Transmit(i2c, led_addr, led_data, 2, 20); // for testing only
	vTaskDelay(10);

	/*************** BB setup *************/

	write_data[0] = 0x0b;
	write_data[1] = 0x01;
	HAL_I2C_Master_Transmit(i2c, write_address, write_data, 2, 20); // choose external vref and mode 0 from advanced config register
	vTaskDelay(20);

	write_data[0] = 0x03;
	write_data[1] = 0xff;
	HAL_I2C_Master_Transmit(i2c, write_address, write_data, 2, 20); // disable interrupts
	vTaskDelay(20);

	write_data[0] = 0x00;
	write_data[1] = 0x01;
	HAL_I2C_Master_Transmit(i2c, write_address, write_data, 2, 20); // start
	vTaskDelay(100);


	uint8_t out[8];
	uint8_t stbd_voltage_addr = 0x22;
	uint8_t port_voltage_addr = 0x24;
	uint8_t temperature_addr = 0x27;
	uint16_t starboard_voltage = 0;
	uint16_t port_voltage = 0;
	int16_t temperature = 0;
	float temp = 0;
	uint8_t output[2] = {0, 0};

	for (;;) {
		// read starboard voltage
		HAL_I2C_Master_Transmit(i2c, write_address, &stbd_voltage_addr, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, read_address, output, 2, 10);
		vTaskDelay(20);
		starboard_voltage = (output[0]<<8) + output[1];

		// read port voltage
		HAL_I2C_Master_Transmit(i2c, write_address, &port_voltage_addr, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, read_address, output, 2, 10);
		vTaskDelay(20);
		port_voltage = (output[0]<<8) + output[1];

		// read temperature
		HAL_I2C_Master_Transmit(i2c, write_address, &temperature_addr, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, read_address, output, 2, 10);
		vTaskDelay(20);
		temperature = (output[0] << 8) + output[1];

		temp = runAverage(temp, temperature / 256.0);
		printToDisplay(temp, 0x64);
		CDC_Transmit_HS(out, strlen(out));

		vTaskDelay(500);
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
	uint8_t read_d1 = 0x4A; //OSR 8192
	uint8_t read_d2 = 0x5A; //OSR 8192
	uint8_t read_adc = 0x00;

	uint16_t calibration[8];
	uint8_t buffer[3];

	uint8_t depthmsg[] = "%%%%99999999!99999999!99999999@@@@\r\n";

	uint32_t d1;
	uint32_t d2;

	uint32_t temp;
	uint32_t press;

	float pressure;
	float temperature;
	float depth;
	uint16_t fluidDensity = 1029;

	I2C_HandleTypeDef *i2c = getDepthI2CRef();

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


	uint8_t values[8];
	uint8_t * ptr;
	for(;;) {

		ptr = &depthmsg[4];
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

		if (temperature >= 100 || pressure >= 10000) {
			vTaskDelay(1500);
			goto resetI2C;
	}

		memset(values, 0, sizeof(uint8_t) * 8);
		fToString2(temperature, values);
		memcpy(ptr, values, 8);
		ptr += 9;

		memset(values, 0, sizeof(uint8_t) * 8);
		fToString(values, pressure);
		memcpy(ptr, values, 8);
		ptr += 9;

		memset(values, 0, sizeof(uint8_t) * 8);
		fToString2(depth, values);
		memcpy(ptr, values, 8);

		CDC_Transmit_HS(depthmsg, 37);

		//vTaskDelay(50); //fuck it, there is enough delay in the I2C reads
	}
}
