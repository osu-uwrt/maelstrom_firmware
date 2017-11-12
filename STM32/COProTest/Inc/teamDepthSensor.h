//
// Created by August R. Mason on 10/14/17.
//
/*
 * This is the init structure of the BAR 30 depth sensor from Blue Robotics. The sensor is based off of a MS5837 depth
 * sensor. Basically we need to rip out all the Arduino wire.h dependencies and make this work ourselves.
 *
 *Things to do:
 * Initialize the struct with the two SCL and SDA pins you will be using.
 */
#include "main.h" //main.h holds the defines for the I2C lines
#include "stm32f4xx_hal.h"
#ifndef MS5837_H_BLUEROBOTICS
#define MS5837_H_BLUEROBOTICS


struct depthSensor{
    //pin definitions
    //somehow

    //const definitions
    float fluidDensity;
    //variables to calculate
    float rawTemp;
    float rawDepth;
    float rawPressure;
    float rawAltitude;
}depthSensor;


//functions that run off the depth sensor:
void initializeSensor(struct depthSensor *depthSensorSet, I2C_HandleTypeDef *I2CLines, float density);
#endif //STM32_TEAMDEPTHSENSOR_H
