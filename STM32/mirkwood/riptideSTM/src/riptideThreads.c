#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stdio.h"


#include "riptideMain.h"
#include "riptideFunctions.h"
#include "riptideThreads.h"
#include "riptideAddress.h"


void vHeartbeat(void *pvParameters ){

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
        //TODO: Rename the switches
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

  for(;;){
    if( HAL_GPIO_ReadPin(KillSwitch_GPIO_Port,KillSwitch_Pin) == GPIO_PIN_RESET){
        resetPWM();
        HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_RESET);

	} else {
		HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, GPIO_PIN_SET);

    }
    vTaskDelay(100);
  }
}

void vBackplaneI2C(void *pvParameters) {
	vTaskDelay(1500); //need this to offset scheduler, just in case
	I2C_HandleTypeDef *i2c = getBackplaneI2CRef();
	uint8_t write_data[2];

	/************* Status Board Setup ******************/
  //setup array for the status board, in order we are setting:
  //  uint8_t setup[8]: decode mode, Red intensity, green intensity, yellow intensity,
  //                    scan limit, registry config, digit type, led test
  uint8_t setup[] = {0xFF, 0x11, 0xAA, 0xCC, 0x05, 0x41, 0x00, 0x00};
	riptideSBsetup(setup);

	/*************** BB setup *************/

	write_data[0] = 0x0b;
	write_data[1] = 0x01;
	HAL_I2C_Master_Transmit(i2c, BB_W_ADDR, write_data, 2, 20); // choose external vref and mode 0 from advanced config register
	vTaskDelay(20);

	write_data[0] = 0x03;
	write_data[1] = 0xff;
	HAL_I2C_Master_Transmit(i2c, BB_W_ADDR, write_data, 2, 20); // disable interrupts
	vTaskDelay(20);

	write_data[0] = 0x00;
	write_data[1] = 0x01;
	HAL_I2C_Master_Transmit(i2c, BB_W_ADDR, write_data, 2, 20); // start
	vTaskDelay(100);



	uint16_t starboard_voltage = 0;
  float stbdV = 0.0;
  uint16_t starboard_current = 0;
  float stbdI = 0.0;
	uint16_t port_voltage = 0;
  float portV = 0.0;
  uint16_t port_current = 0;
  float portI = 0.0;
	int16_t temperature = 0;
	float temp = 0.0;
	uint8_t output[2] = {0, 0};

	for (;;) {
		// read starboard voltage
		HAL_I2C_Define_Transmit(i2c, BB_W_ADDR, BB_STBDV_ADDR, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, BB_R_ADDR, output, 2, 10);
		vTaskDelay(20);
		starboard_voltage = (((output[0]<<8) + output[1]) >> 4);
    stbdV = calcSTBDV(starboard_voltage);

		// read port voltage
		HAL_I2C_Define_Transmit(i2c, BB_W_ADDR, BB_PORTV_ADDR, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, BB_R_ADDR, output, 2, 10);
		vTaskDelay(20);
		port_voltage = (((output[0]<<8) + output[1]) >> 4);
    portV = calcPORTV(port_voltage);

		// read temperature
		HAL_I2C_Define_Transmit(i2c, BB_W_ADDR, BB_TEMP_ADDR, 1, 10);
		vTaskDelay(20);
		HAL_I2C_Master_Receive(i2c, BB_R_ADDR, output, 2, 10);
		vTaskDelay(20);
		temperature = ((output[0] << 8) + output[1]);
		temp = runAverage(temp, temperature / 256.0 );

    //read starboard current
    HAL_I2C_Define_Transmit(i2c, BB_W_ADDR, BB_STBDI_ADDR, 1, 10);
    vTaskDelay(20);
    HAL_I2C_Master_Receive(i2c, BB_R_ADDR, output, 2, 10);
    vTaskDelay(20);
    starboard_current = (((output[0]<<8)+output[1]) >> 4);
    stbdI = calcCurrent(starboard_current);

    //read starboard current
    HAL_I2C_Define_Transmit(i2c, BB_W_ADDR, BB_PORTI_ADDR, 1, 10);
    vTaskDelay(20);
    HAL_I2C_Master_Receive(i2c, BB_R_ADDR, output, 2, 10);
    vTaskDelay(20);
    port_current = (((output[0]<<8)+output[1]) >> 4);
    portI = calcCurrent(port_current);

    printToDisplay(stbdI, 0x62);
    printToDisplay(portI, 0x60);
		printToDisplay(temp, 0x64);
		vTaskDelay(500);
    //package values and send to computer

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
  /*this read_d1 and read_d2 set the OSR of ADC, this sets the max wait time between
    reading off the ADC at 10ms.  This sample rate results in about a 7.5cm possible
    error in the sensor which is safe for what we are doing.
  */
	uint8_t read_d1 = 0x48; //OSR 8192 = 0x4A
	uint8_t read_d2 = 0x58; //OSR 8192 = 0x5A
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
		vTaskDelay(10); //delay set by the OSR of the sensor
		HAL_I2C_Master_Transmit(i2c, address_write, &read_adc, 1, 20);
		vTaskDelay(10);

		HAL_I2C_Master_Receive(i2c, address_read, buffer, 3, 20);
		d1 = buffer[0];
		d1 = d1 << 8 | buffer[1];
		d1 = d1 << 8 | buffer[2];

		HAL_I2C_Master_Transmit(i2c, address_write, &read_d2, 1, 30);
		vTaskDelay(10);
		HAL_I2C_Master_Transmit(i2c, address_write, &read_adc, 1, 30);
		vTaskDelay(10);

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
