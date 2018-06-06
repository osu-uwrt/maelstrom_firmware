//
// Created by August R. Mason on 2/11/18.
//
#include "main.h"
#include <stdbool.h> //used for the MessageCheck function
#include <stdint.h> //for the crc4 message
#include "riptideFunctions.h"
#include "riptideAddress.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"
#include <math.h>

//##################################### USB OTG Functions ##############################################################
/*
 *          Message Check
 *  This function is meant to streamline the messages sent from riptide to the STM32. Primary use case
 *  is for working in the CDC_Recieve function to check which message that gets recieved and set a bool to
 *  determine which message was sent
 *  Intakes:
 *          char* start     -the characters that start the message
 *          char* end       -the characters that are the last
 *          int compNumber  -the number of characters that start and end the message
 *          uint8_t* Buf    -the message sent
 *          uint32_t *Len   -the length of the message
 */
bool MessageCheck(char* start, char* end, int compNumber,uint8_t* Buf, uint32_t *Len ){
    int i = 0;
    while( i < compNumber ){
        if (Buf[i] == *start && Buf[*Len - (1+i)] == *end){

            i++;
        }
        else {
            return false;
        }
    }
    return true;

}


int8_t Riptide_CDC_Receive(uint8_t* Buf, uint32_t *Len ){

  char* thrustst = "#";
  char* thrustend = "@";
  int   thrustComp = 4;
  bool thrustsuccess = false;
  uint16_t values[8];
  thrustsuccess = MessageCheck(thrustst, thrustend, thrustComp, Buf, Len);
  //the length of the thruster message will be 40 so hard check that
  if (thrustsuccess && *Len == 40){
    //limit this ISR to just write to registers
    parse(Buf, values);
    writePWM(values);
  }

    return (USBD_OK);
}

void parse(uint8_t * raw, uint16_t * values) {
	uint8_t string[4];
	for (int i = 1; i <= 10; i++) {
		string[0] = raw[4 * i];
		string[1] = raw[4 * i + 1];
		string[2] = raw[4 * i + 2];
		string[3] = raw[4 * i + 3];
		values[i - 1] = atoi(string);
	}
}
//##################################### USB OTG Functions ##############################################################

//##################################### Depth Sensor Functions #########################################################
// From the Blue Robotics data sheet, cyclic redundancy check
uint8_t crc4(uint16_t n_prom[]) {
	uint16_t n_rem = 0;

	n_prom[0] = ((n_prom[0]) & 0x0FFF);
	n_prom[7] = 0;

	for ( uint8_t i = 0 ; i < 16; i++ ) {
		if ( i%2 == 1 ) {
			n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
		} else {
			n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
		}
		for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
			if ( n_rem & 0x8000 ) {
				n_rem = (n_rem << 1) ^ 0x3000;
			} else {
				n_rem = (n_rem << 1);
			}
		}
	}

	n_rem = ((n_rem >> 12) & 0x000F);

	return n_rem ^ 0x00;
}

void calculate(uint32_t D1, uint32_t D2, uint16_t C[8], uint32_t * TEMP, uint32_t * P) {
	// Given C1-C6 and D1, D2, calculated TEMP and P
	// Do conversion first and then second order temp compensation

	uint8_t vals[8];

	int32_t dT = 0;
	int64_t SENS = 0;
	int64_t OFF = 0;
	int32_t SENSi = 0;
	int32_t OFFi = 0;
	int32_t Ti = 0;
	int64_t OFF2 = 0;
	int64_t SENS2 = 0;

	// Terms called
	dT = D2 - (uint32_t) (C[5])*256l;
	SENS = (int64_t) (C[1])*32768l+((int64_t) (C[3])*dT)/256l;
	OFF = (int64_t) (C[2])*65536l+((int64_t) (C[4])*dT)/128l;
	*P = (D1*SENS/(2097152l)-OFF)/(8192l);

	// Temp conversion
	*TEMP = 2000l + (int64_t) (dT)*C[6]/8388608LL;

	//Second order compensation
	if((*TEMP/100)<20){         //Low temp
		Ti = (3*(int64_t)(dT)*(int64_t)(dT))/(8589934592LL);
		OFFi = (3*(*TEMP-2000)*(*TEMP-2000))/2;
		SENSi = (5*(*TEMP-2000)*(*TEMP-2000))/8;

		if((*TEMP/100)<-15){    //Very low temp
			OFFi = OFFi+7*(*TEMP+1500l)*(*TEMP+1500l);
			SENSi = SENSi+4*(*TEMP+1500l)*(*TEMP+1500l);
		}
	}
	else if((*TEMP/100)>=20){    //High temp
		Ti = 2*(dT*dT)/(137438953472LL);
		OFFi = (1*(*TEMP-2000)*(*TEMP-2000))/16;
		SENSi = 0;
	}

	OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
	SENS2 = SENS-SENSi;

	*TEMP = (*TEMP-Ti);
	*P = (((D1*SENS2)/2097152l-OFF2)/8192l)/10;

}

void convert(uint32_t * temp, uint32_t * press, float * temperature, float * pressure, float * depth, uint16_t fluidDensity) {
	*temperature = *temp / 100.0;
	*pressure = *press * 1.0;
	*depth = ((100.0 * *pressure) - 101300.0)/(fluidDensity * 9.80665);
}

void fToString(uint8_t * dest, float toConvert) {
	uint8_t decimals[7];
	itoa(toConvert, dest, 10);
	strcat(dest, ".");
	int trunked = (int) toConvert;
	uint16_t i = (toConvert - trunked) * 100000;
	itoa(i, decimals, 10);
	strcat(dest, decimals);
	dest[7] = 0;
}

// Converts a floating point number to string.
void fToString2(float n, uint8_t *res) {
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    if (fpart < 0) {
    	res[0] = '-';
    	res++;
    	ipart *= -1;
    	fpart *= -1;
    }

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

	res[i] = '.';  // add dot

	// Get the value of fraction part upto given no.
	// of points after dot. The third parameter is needed
	// to handle cases like 233.007
	fpart = fpart * pow(10, (8 - i));

	intToStr((int) fpart, res + i + 1, 8 - i);
}

int intToStr(int x, uint8_t str[], int d) {
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}

	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
		str[i++] = '0';

	revStr(str, i);
	str[i] = '\0';
	return i;
}

void revStr(char *str, int len) {
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

//##################################### i2c Bkpln Functions ############################################################

/*
                    Status board setup
  Riptide Specific.You need to include riptideAddress.h anad verify you are
  working on the right i2c bus.  You cannot pass and an I2C type def as easily as
  you should be able to.
  Inputs:
          uint8_t setup[8]              -this is the data that you write to various registers
          uint8_t setup[8]: decode mode, Red intensity, green intensity, yellow intensity,
                            scan limit, registry config, digit type, led test
  Returns: void
*/
void riptideSBsetup(uint8_t setup[8]){
  //get the i2c device you are using cause the type def is dumb
  I2C_HandleTypeDef * i2c_bp = getBackplaneI2CRef();

  uint8_t led_data[] = {SB_DEC_MODE, setup[0]}; //data array to be passed through
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send decode mode (standard)
  led_data[0] = SB_REDINTENSE;
  led_data[1] = setup[1];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_REDINTENSE_A;
  led_data[1] = setup[1];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_GREENINTENSE;
  led_data[1] = setup[2];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_GREENINTENSE_A;
  led_data[1] = setup[2];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_YELLOWINTENSE;
  led_data[1] = setup[3];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_YELLOWINTENSE_A;
  led_data[1] = setup[3];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // send LED intensity
  led_data[0] = SB_SCAN_LIMIT;
  led_data[1] = setup[4];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // set the scan limit (not sure why 5 though)?
  led_data[0] = SB_REG_CONFIG;
  led_data[1] = setup[5];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // set the control register, set the global led intensity mode off
  led_data[0] = SB_DIGIT_TYPE;
  led_data[1] = setup[6];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // set as a 7 segment display
  led_data[0] = SB_LED_Test;
  led_data[1] = setup[7];
  vTaskDelay(10);
  HAL_I2C_Master_Transmit(i2c_bp, SB_ADDR, led_data, 2, 20); // for testing only
  vTaskDelay(10);
}
// this function assumes the length is 4
void printToDisplay(float value, uint8_t which) {
	I2C_HandleTypeDef * i2c = getBackplaneI2CRef();

	uint8_t led_addr = 0xC0;
	uint8_t data[2];

	data[0] = which;
	data[1] = (int) value/10; //used to divide by 100
	HAL_I2C_Master_Transmit(i2c, led_addr, data, 2, 20);
	vTaskDelay(10);

	data[0] = which + 8;
	data[1] = ((int) value) % 10 | 0x80; //used to divide by 10
	HAL_I2C_Master_Transmit(i2c, led_addr, data, 2, 20);
	vTaskDelay(10);

	data[0] = which + 1;
	data[1] = (int) (value * 10) % 10;
	HAL_I2C_Master_Transmit(i2c, led_addr, data, 2, 20);
	vTaskDelay(10);

	data[0] = which + 9;
	data[1] = (int) (value * 100) % 10;
	HAL_I2C_Master_Transmit(i2c, led_addr, data, 2, 20);
	vTaskDelay(10);
}

float runAverage(float val, float newVal){
	val -= val/10.0; //delete the last value
	val += newVal/10.0; //add the next one

	return val;
}
//TODO: Make temp dependent
float calcSTBDV(uint16_t raw){
  float val = 0.0;
  val = (-0.0085 * raw ) + 39.046;
  if (val <= 10.0){
    val = 0.0;
  }
  return val;
}

float calcPORTV(uint16_t raw){
  float val = 0.0;
  val = (-0.0069 * raw ) + 33.842; //nice
  if (val <= 10.0){
    val = 0.0;
  }
  return val;
}

float calcCurrent(uint16_t raw){
  float val = 0.0;
  val = (raw - 2035) * 0.0244;
  if (val <= 0.06){
    val = 00.00;
  }
  return val;
}
