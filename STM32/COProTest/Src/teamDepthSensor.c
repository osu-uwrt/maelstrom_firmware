//
// Created by August R. Mason on 10/14/17.
//
#include "teamDepthSensor.h"
#include "main.h" //main.h holds the inits
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#define MS5837_ADDR               0x76
#define MS5837_RESET              0x1E
#define MS5837_ADC_READ           0x00
#define MS5837_PROM_READ          0xA0
#define MS5837_CONVERT_D1_8192    0x4A
#define MS5837_CONVERT_D2_8192    0x5A

/*
 * Things to do:
 *
 */
//function prototypes
//only need to initialize sensor and then read off of it



//void depthReading (struct depthSensor *depthSensor);
/*
 * initializeSensor sets the values of the SDA and SCL pin along with fluid density and other things
 * that are deemed fit. This passes a struct through reference and sets all the stuff we want "constant".
 * We need:
 *  struct depthSensor  - the sensor structure that you will be using

 *  float density       - the fluid density the sensor will need
 */
void initializeSensor(struct depthSensor *depthSensorSet, I2C_HandleTypeDef *I2CLines, float density){
    //set that shit
    depthSensorSet->fluidDensity = density;

    //pass through some sort of i2c init struct so the sensor knows

};
/*
void depthReading(struct depthSensor *depthSensor) {
        //variables to use
        unsigned char buffer[5];
        unsigned int rawT, rawH;

};
*/