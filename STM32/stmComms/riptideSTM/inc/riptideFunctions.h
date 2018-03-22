//
// Created by August R. Mason on 2/11/18.
//

#ifndef STMCOMMS_RIPTIDEFUNCTIONS_H
#define STMCOMMS_RIPTIDEFUNCTIONS_H

#endif //STMCOMMS_RIPTIDEFUNCTIONS_H
#include <stdbool.h>
bool MessageCheck(char* start, char* end, int compNumber,uint8_t* Buf, uint32_t *Len );

int8_t Riptide_CDC_Receive(uint8_t* Buf, uint32_t *Len );
