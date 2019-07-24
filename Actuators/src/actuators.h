#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>

// I2C
#define I2C_ADDRESS 0x1C

// Commands
#define SET_TORPEDO_TIMING 0
#define RESET_BOARD 1
#define SET_TORPEDO_HOLD 2
#define GET_TORPEDO_STATUS 3
#define ARM_TORPEDO 4
#define FIRE_TORPEDO 5
#define RELEASE_MARKER 6
#define SET_GRIPPER_PWM 7

#define CSTAT_DISARMED 0
#define CSTAT_CHARGING 1
#define CSTAT_ARMED 2

// Pin definitions
#define GRIPPER_PIN 35
#define CHARGE_PIN 10
#define T1_GATE 11 
#define T2_GATE 14
#define C1_GATE 38
#define C2_GATE 39
#define C3_GATE 40
#define C4_GATE 41
#define C5_GATE 2
#define CS_GATE 3
#define D1_GATE 4
#define D2_GATE 5
#define CAP_REF_PIN 19
#define ARM_VOLTAGE 2.0f

class Actuators {
    public:
        Actuators();
        void SetTorpedoTiming(int torpId, int coilId, float start, float end);
        void GetTorpedoStatus();
        void ArmTorpedo(bool shouldArm);
        void FireTorpedo(int id);
        void ReleaseMarker(int id);
        void SetGripperPwm(int pwm);
        void ResetBoard();
        void CheckStatus();

    private:
        int _torpedoTimings[2][6][2];
        int _capStatus;
        int _coilPins[6] = {CS_GATE, C1_GATE, C2_GATE, C3_GATE, C4_GATE, C5_GATE};
};

#endif