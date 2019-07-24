# include <i2c.c>

#define ARM 2

u8 buffer[10];

void handleReceive(u8 numBytes) 
{    
    u8 i = 0;
    while(I2C_available())
        buffer[i++] = Wire.read();
        
    if (buffer[0] == 1)
        digitalWrite(29, LOW);
    if (buffer[0] == 0)
        digitalWrite(29, HIGH);
    if (buffer[0] == 2) 
    {
        digitalWrite(ARM, LOW);
        delay(500);
        digitalWrite(ARM, HIGH);
    }
    CDC.printf("Bytes read: %d\r\n", i);
    CDC.printf("buffer[0]: %d\r\n", buffer[0]);
}


void setup() {
    pinMode(ARM, OUTPUT);
    pinMode(USERLED, OUTPUT);
    digitalWrite(ARM, HIGH);
    digitalWrite(USERLED, HIGH);
    
    Wire.begin(0x1C * 2, 0);
    Wire.onReceive(handleReceive);
}


void loop() {
}