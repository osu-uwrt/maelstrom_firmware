//
// Created by August R. Mason on 2/11/18.
//
#include "main.h"
#include <stdbool.h> //used for the MessageCheck function
#include <stdint.h>
#include "riptideFunctions.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"

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
    uint8_t Successmsg[] = "thrust hell yeah \r\n";
  uint8_t failmsg[] = "not good enough kid\r\n";
  char* thrustst = "#";
  char* thrustend = "@";
  int   thrustComp = 4;
  bool thrustsuccess = false;
  uint16_t values[8];
  thrustsuccess = MessageCheck(thrustst, thrustend, thrustComp, Buf, Len);
  if (thrustsuccess){
    HAL_GPIO_TogglePin(HeartBeat2_GPIO_Port, HeartBeat2_Pin);
    CDC_Transmit_HS(Successmsg, sizeof(Successmsg));
    parse(Buf, values);
    writePWM(values);

  }
  if (!thrustsuccess){
    CDC_Transmit_HS(failmsg, sizeof(failmsg));
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
	*depth = ((100.0 * *pressure) - 101300)/(fluidDensity * 9.80665);
}
