bool isValid = false; 
String time; 
int heartbeatCounter = 0;

int LED_POWER = 4;
int LED_TX = 3;
int LED_RX = 2;
int LED_FIRE = A1; //pin 24

int CON_1= A3; //fill 8 - Torpedo Starboard
int CON_2= A2; //fill 7 - Torpedo Port
int CON_3= 13; //fill 6 - Marker Dropper
int CON_4= 12; //fill 5 - Manipualtor
int CON_5= 11; //fill 4 - Extra - 17
int CON_6= 10; //empty3 - Release Extra //
int CON_7= 9;  //empty 2 - Release Manipulator
int CON_8= 8;  //empty 1 - Release Marker Dropper

void setup() {
  
  pinMode(CON_1, OUTPUT);  
  pinMode(CON_2, OUTPUT);   
  pinMode(CON_3, OUTPUT);  
  pinMode(CON_4, OUTPUT);   
  pinMode(CON_5, OUTPUT);  
  pinMode(CON_6, OUTPUT);   
  pinMode(CON_7, OUTPUT);  
  pinMode(CON_8, OUTPUT);   
  pinMode(LED_FIRE, OUTPUT);
  
  Serial.begin(9600, SERIAL_8E1);
  digitalWrite(LED_POWER, HIGH);
  Serial.setTimeout(100);
}

void loop() {
  
  delay(1);
  
  if(Serial.available()){
        
        String str = Serial.readStringUntil('\n');
     
	//String str = "++++!nnnn!nnnn!nnnn!nnnn@@@@"; //test string
	char copy[30];
	str.toCharArray(copy, 30);	

        isValid = true;
        
	for (int i=0; i <= 3; i++)
        {
            if(copy[i] != '+' || copy[27 - i] != '@') {
                isValid = false;
            }
        }
	
	if (isValid) {
          bool validNumber;
           
          if(copy[4] == '!') {
              validNumber = true;
              time = "";
	      for(int i=5; i<=8; i++) {
	        time += copy[i];
                if (!isDigit(copy[i])) 
                    validNumber = false;
              }	
              
              if (validNumber) {
    	          digitalWrite(CON_3, HIGH); 		
    	          delay(time.toInt());
                  digitalWrite(CON_3, LOW); 
                  digitalWrite(CON_8, HIGH);
                  delay(800);
                  digitalWrite(CON_8, LOW);
      
                  Serial.println("++++_MarkerDropper_@@@@");
              }
          }
          
          //Torpedo Starboard
          if(copy[9] == '!') {
              validNumber = true;
              time = "";
	      for(int i=10; i<=13; i++) {
	        time += copy[i];
                if (!isDigit(copy[i])) 
                    validNumber = false;
              }	
              
              if (validNumber) {
      	          digitalWrite(CON_1, HIGH);		
      	          delay(time.toInt());	
                  digitalWrite(CON_1, LOW);
                  Serial.println("++++_Torpedo1_@@@@");
              }
          }
          //Torpedo Port
          if(copy[14] == '!') {
              validNumber = true;
              time = "";
	      for(int i=15; i<=18; i++) {
	        time += copy[i];
                if (!isDigit(copy[i])) 
                    validNumber = false;
              }	
              
              if (validNumber) {
	          digitalWrite(CON_2, HIGH);		
	          delay(time.toInt());	
                  digitalWrite(CON_2, LOW);

                  Serial.println("++++_Torpedo2_@@@@");
              }
          }
          //Manipulator
          if(copy[19] == '!') {
              validNumber = true;
              time = "";
	      for(int i=20; i<=23; i++) {
	        time += copy[i];
                if (!isDigit(copy[i])) 
                    validNumber = false;
              }	
              
              if (validNumber) {
                  digitalWrite(CON_4, HIGH);						
      	          delay(time.toInt());
                  digitalWrite(CON_4, LOW);
                  digitalWrite(CON_7, HIGH);	
                  delay(800);
                  digitalWrite(CON_7, LOW);	
      
                  Serial.println("++++_Manipulator_@@@@");
              }
               
          }
        } else 
            Serial.println("WTF U MEAN?");
    }
    if (heartbeatCounter++ == 1000) {
        Serial.println("pneumatics");
        heartbeatCounter = 0;
    }	
}			




