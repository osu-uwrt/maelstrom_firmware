#include "actuators.h"

Actuators::Actuators() 
{}

void Actuators::ArmTorpedo(bool shouldArm) 
{
    if (shouldArm && (_capStatus == CSTAT_DISARMED)) {
        _capStatus = CSTAT_CHARGING;
        digitalWrite(CHARGE_PIN, HIGH);

    } else if (!shouldArm) {
        // Disarm
        _capStatus = CSTAT_DISARMED;
        digitalWrite(CHARGE_PIN, LOW);
    }
}

void Actuators::SetTorpedoTiming(int torpId, int coilId, float start, float end)
{
    _torpedoTimings[torpId][coilId][0] = start;
    _torpedoTimings[torpId][coilId][1] = end;
}

void Actuators::GetTorpedoStatus() 
{
    byte response = ((GET_TORPEDO_STATUS << 2) | (this->_capStatus)) << 2;
    Wire.write(response);
}

void Actuators::FireTorpedo(int id) 
{
    unsigned long start = millis();
    int fireCount = 0;
    int coilState[6];
    while (fireCount < 6)
    {
        unsigned long current = start - millis();
        for (int i = 0; i < 6; i++) 
        {
            if (coilState[i] == 0)
            {
                if ((current) >= _torpedoTimings[id][i][0])
                {
                    coilState[i] = 1;
                    digitalWrite(_coilPins[i], HIGH);
                }
            }
            else if (coilState[i] == 1)
            {
                if ((current / 1000.0) >= _torpedoTimings[id][i][1])
                {
                    coilState[i] = 2;
                    digitalWrite(_coilPins[i], LOW);
                    fireCount++;
                }
            }
        }

    }
}

void Actuators::ReleaseMarker(int id) 
{
    if (id == 0)
    {
        digitalWrite(D1_GATE, HIGH);
        delay(1000);
        digitalWrite(D1_GATE, LOW);
    } 
    else if (id == 1)
    {
        digitalWrite(D2_GATE, HIGH);
        delay(1000);
        digitalWrite(D2_GATE, LOW);
    }
}

void Actuators::SetGripperPwm(int pwm) 
{
    analogWrite(GRIPPER_PIN, pwm);
}

void Actuators::ResetBoard() 
{
    wdt_disable();
    wdt_enable(WDTO_15MS);
    while (1) {}
}

void Actuators::CheckStatus()
{
    if (_capStatus == CSTAT_CHARGING)
    {
        int read = analogRead(CAP_REF_PIN);
        float voltage = (float)read * (3.3/1024.0);
        if (voltage >= ARM_VOLTAGE) 
        {
            _capStatus == CSTAT_ARMED;
        }
    }

    Serial.println("[TIMINGS 1]");
    for (int i = 0; i < 6; i++) {
        Serial.print(_torpedoTimings[0][i][0]);
        Serial.print(" - ");
        Serial.println(_torpedoTimings[0][i][1]);
    }


    Serial.println("[TIMINGS 2]");
    for (int i = 0; i < 6; i++) {
        Serial.print(_torpedoTimings[1][i][0]);
        Serial.print(" - ");
        Serial.println(_torpedoTimings[1][i][1]);
    }

    delay(3000);
}