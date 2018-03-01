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

int8_t Riptide_CDC_Receive(uint8_t* Buf, uint32_t *Len, uint16_t * values){
  uint8_t Successmsg[] = "thrust hell yeah\r\n";
  uint8_t failmsg[] = "not good enough kid\r\n";
  char* thrustst = "#";
  char* thrustend = "@";
  int   thrustComp = 4;
  bool thrustsuccess = false;
  thrustsuccess = MessageCheck(thrustst, thrustend, thrustComp, Buf, Len);

  if (thrustsuccess){
    CDC_Transmit_HS(Successmsg, sizeof(Successmsg));
    parse(Buf, values);
  }

  if (!thrustsuccess){
    CDC_Transmit_HS(failmsg, sizeof(failmsg));
    *values = NULL;
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
