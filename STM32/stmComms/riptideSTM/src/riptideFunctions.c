//
// Created by August R. Mason on 2/11/18.
//
#include "main.h"
#include <stdbool.h> //used for the MessageCheck function
#include <stdint.h>
#include "riptideFunctions.h"
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
