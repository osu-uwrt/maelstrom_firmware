//
// Created by August R. Mason on 2/11/18.
//

#ifndef STMCOMMS_RIPTIDEFUNCTIONS_H
#define STMCOMMS_RIPTIDEFUNCTIONS_H

#endif //STMCOMMS_RIPTIDEFUNCTIONS_H
#include <stdbool.h>
bool MessageCheck(char* start, char* end, int compNumber,uint8_t* Buf, uint32_t *Len );
void parse(uint8_t * raw, uint16_t * values);
uint8_t crc(uint16_t n_prom[]);
void calculate(uint32_t D1, uint32_t D2, uint16_t C[8], uint32_t * TEMP, uint32_t * P);
void convert(uint32_t * temp, uint32_t * press, float * temperature, float * pressure, float * depth, uint16_t fluidDensity);
void fToString(uint8_t * dest, float toConvert);

int8_t Riptide_CDC_Receive(uint8_t* Buf, uint32_t *Len );
