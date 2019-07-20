#include "src/actuators.h"

Actuators acts;

void setup() {
    Serial.begin(9600);
    Wire.begin(I2C_ADDRESS);

    Wire.onReceive(handleReceive);
    Wire.onRequest(handleRequest);
}

void handleReceive(int numBytes) 
{
    byte raw[numBytes];
    for (int i = 0; i < numBytes; i++) {
        raw[i] = Wire.read();
    }

    int command = raw[0] >> 4;
    
    switch (command) {
        case SET_TORPEDO_TIMING:
        {
            int torpId = (raw[0] & 0x08) >> 3;
            int coilId = (raw[0] & 0x07);
            float start = (float)raw[1];
            float end = (float)raw[2];
            acts.SetTorpedoTiming(torpId, coilId, start, end);
            break;
        }
        case RESET_BOARD:
        {
            acts.ResetBoard();
            break;
        }
        case GET_TORPEDO_STATUS:
        {
            acts.GetTorpedoStatus();
            break;
        }
        case ARM_TORPEDO:
        {
            bool shouldArm = (raw[0] & 0x04); // select bit 2
            acts.ArmTorpedo(shouldArm);
            break;
        }
        case FIRE_TORPEDO:
        {
            int torpId = (raw[0] & 0x08) >> 3; // select bit 3 (torp 0 or 1)
            acts.FireTorpedo(torpId);
            break;
        }
        case RELEASE_MARKER:
        {
            int markId = (raw[0] & 0x08) >> 3; // select bit 3 (torp 0 or 1)
            acts.ReleaseMarker(markId);
            break;
        }
        case SET_GRIPPER_PWM:
        {
            int pwm = (raw[1] << 8) + raw[2];
            acts.SetGripperPwm(pwm);
            break;
        }
        default:
            break;
    }
}

void handleRequest()
{
    acts.GetTorpedoStatus();\
}

void loop() {
    acts.CheckStatus();
}