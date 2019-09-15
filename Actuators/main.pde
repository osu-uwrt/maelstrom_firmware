# include <i2c.c>

// I2C
#define I2C_ADDRESS 0x1C

// Commands
#define SET_TORPEDO_TIMING 0
#define RESET_BOARD 1
#define GET_TORPEDO_STATUS 3
#define ARM_TORPEDO 4
#define FIRE_TORPEDO 5
#define RELEASE_MARKER 6
#define SET_GRIPPER_PWM 7

#define TORPEDO_FIRING 0
#define TORPEDO_FIRED 1

// Pin definitions
#define GRIPPER_PIN 11
#define CHARGE_PIN 2
#define T1_GATE 3
#define T2_GATE 4
#define C1_GATE 21
#define C2_GATE 22
#define C3_GATE 23
#define C4_GATE 24
#define C5_GATE 25
#define CS_GATE 26
#define D1_GATE 27
#define D2_GATE 28
#define CAP_REF_PIN 13
#define ARM_VOLTAGE 1.9f

u8 buffer[10];
long _torpedoTimings[2][6][2];
int _torpedoStatus = TORPEDO_FIRED;
int _coilPins[6] = {C1_GATE, C2_GATE, C3_GATE, C4_GATE, C5_GATE, CS_GATE};
u8 fire = 255;
u8 drop = 255;
u8 response = 0;
unsigned int pwm = 1300 * 1.287;

u8 armTorpedo(u8 shouldArm) 
{
    if (shouldArm)
        digitalWrite(CHARGE_PIN, LOW);
    else
        digitalWrite(CHARGE_PIN, HIGH);
       return 1;
}

u8 setTorpedoTiming(u8 torpId, u8 coilId, long start, long end)
{
    _torpedoTimings[torpId][coilId][0] = start * 10;
    _torpedoTimings[torpId][coilId][1] = end * 10;
    return 1;
}

u8 getTorpedoStatus() 
{
    return _torpedoStatus;
}

u8 fireTorpedo(u8 id) 
{
    fire = id;
    _torpedoStatus = TORPEDO_FIRING;
    return 1;
}

u8 releaseMarker(int id) 
{
    //CDC.printf("Dropping marker %d\r\n", id);
    drop = id;
    return 1;
}

u8 setGripperPwm(int pwmVal) 
{
    pwm = pwmVal * 1.287;
    return 1;
}

u8 resetBoard() 
{
    //Watchdog.clear();
    return 1;
}


void handleReceive(u8 numBytes) 
{    
    u8 raw[20];
    u8 i = 0;
    u8 command;
    
    for (i = 0; i < numBytes; i++) {
        raw[i] = Wire.read();
    }

    command = raw[0] >> 5;
    
    //CDC.printf("NumBytes: %d\r\n", numBytes);
    //CDC.printf("Command: %d\r\n", raw[0]);
    
    switch (command) {
        case SET_TORPEDO_TIMING:
        {
            int torpId = (raw[0] & 0x08) >> 3;
            int coilId = (raw[0] & 0x07);
            long start = (((long)raw[1]) << 8) + raw[2];
            long end = (((long)raw[3]) << 8) + raw[4];
            response = setTorpedoTiming(torpId, coilId, start, end);
            break;
        }
        case RESET_BOARD:
        {
            response = resetBoard();
            break;
        }
        case GET_TORPEDO_STATUS:
        {
            response = getTorpedoStatus();
            break;
        }
        case ARM_TORPEDO:
        {
            u8 shouldArm = (raw[0] & 0x10) >> 4; // select bit 2
            response = armTorpedo(shouldArm);
            break;
        }
        case FIRE_TORPEDO:
        {
            u8 torpId = (raw[0] & 0x10) >> 4; // select bit 3 (torp 0 or 1)
            response = fireTorpedo(torpId);
            break;
        }
        case RELEASE_MARKER:
        {
            u8 markId = (raw[0] & 0x10) >> 4; // select bit 3 (torp 0 or 1)
            response = releaseMarker(markId);
            break;
        }
        case SET_GRIPPER_PWM:
        {
            int pwm = ((unsigned int)raw[1] << 8) + raw[2] ;
            response = setGripperPwm(pwm);
            break;
        }
        default:
            break;
    }
}

void requestEvent() {
    Wire.write(response);
}


void setup() {
    int i = 0;
    pinMode(CHARGE_PIN, OUTPUT);
    pinMode(T1_GATE, OUTPUT);
    pinMode(T2_GATE, OUTPUT);
    pinMode(C1_GATE, OUTPUT);
    pinMode(C2_GATE, OUTPUT);
    pinMode(C3_GATE, OUTPUT);
    pinMode(C4_GATE, OUTPUT);
    pinMode(C5_GATE, OUTPUT);
    pinMode(CS_GATE, OUTPUT);
    pinMode(D1_GATE, OUTPUT);
    pinMode(D2_GATE, OUTPUT);
    pinMode(GRIPPER_PIN, OUTPUT);
    pinMode(CAP_REF_PIN, INPUT);
    digitalWrite(CHARGE_PIN, HIGH);
    digitalWrite(T1_GATE, LOW);
    digitalWrite(T2_GATE, LOW);
    digitalWrite(C1_GATE, LOW);
    digitalWrite(C2_GATE, LOW);
    digitalWrite(C3_GATE, LOW);
    digitalWrite(C4_GATE, LOW);
    digitalWrite(C5_GATE, LOW);
    digitalWrite(CS_GATE, LOW);
    digitalWrite(D1_GATE, LOW);
    digitalWrite(D2_GATE, LOW);
    digitalWrite(USERLED, HIGH);
    for(i = 0; i < 1000; i++){
        digitalWrite(GRIPPER_PIN, HIGH);
        delayMicroseconds(pwm);
        digitalWrite(GRIPPER_PIN, LOW);
        delay(5);
    }
    
    Wire.begin(I2C_ADDRESS * 2, 0);
    Wire.onReceive(handleReceive);
    digitalWrite(USERLED, LOW);
    //Wire.onRequest(requestEvent);
}


void loop() {
    u8 i = 0;
    long duration[] = {0,0,0,0,0};
    long pause[] = {0,0,0,0,0};
    float voltage = 0.0;
        
    if (fire < 128) {
        digitalWrite(C1_GATE, LOW);
        digitalWrite(C2_GATE, LOW);
        digitalWrite(C3_GATE, LOW);
        digitalWrite(C4_GATE, LOW);
        digitalWrite(C5_GATE, LOW);
        digitalWrite(CS_GATE, LOW);
        digitalWrite(T1_GATE, LOW);
        digitalWrite(T2_GATE, LOW);
        if (fire == 0)
            digitalWrite(T1_GATE, HIGH);
        else
            digitalWrite(T2_GATE, HIGH);
        
        digitalWrite(CS_GATE, HIGH);
        delayMicroseconds(_torpedoTimings[fire][5][1]);
        digitalWrite(CS_GATE, LOW);
        
        
        for (i = 0; i < 4; i++) {
            duration[i] = _torpedoTimings[fire][i][1] - _torpedoTimings[fire][i][0];
            pause[i] = _torpedoTimings[fire][i+1][0] - _torpedoTimings[fire][i][1];
        }
        duration[4] = _torpedoTimings[fire][4][1] - _torpedoTimings[fire][4][0];
        
        
        while (voltage < ARM_VOLTAGE)
            voltage = analogRead(CAP_REF_PIN) * (3.3/1024);
            
        for(i = 0; i < 5; i++) {
            digitalWrite(_coilPins[i], HIGH);
            delayMicroseconds(duration[i]);
            digitalWrite(_coilPins[i], LOW);
            delayMicroseconds(pause[i]);
        }
        fire = 255;
    }
    
    if (drop < 128) {
        //CDC.printf("DROPPING!!!\r\n");
        if (drop == 0)
        {
            digitalWrite(D1_GATE, HIGH);
            delay(100);
            digitalWrite(D1_GATE, LOW);
        } 
        else
        {
            digitalWrite(D2_GATE, HIGH);
            delay(100);
            digitalWrite(D2_GATE, LOW);
        }
        drop = 255;
    }
    
    toggle(USERLED);
    
    digitalWrite(GRIPPER_PIN, HIGH);
    delayMicroseconds(pwm);
    digitalWrite(GRIPPER_PIN, LOW);
    delay(5);
}