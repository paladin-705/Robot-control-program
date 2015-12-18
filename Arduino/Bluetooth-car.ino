#include "MotorShield.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  //RX модуля 11 пин TX модуля 10 пин


const int MESSAGE_HEADER = 42;
const int MESSAGE_END = 0;
const int WATCH_DOG_INTERVAL = 400;

void SerialTakeMessage(bool *dirLM, bool *dirRM, int *speedLM, int *speedRM, bool *flg)
{
  if (mySerial.available() >= 6)          
  {
    while (mySerial.read() != MESSAGE_HEADER);  
     *dirLM = mySerial.read();
     *dirRM = mySerial.read();
     *speedLM = mySerial.read();
     *speedRM = mySerial.read();
     *flg = true;
  }
}
void setup() {
	mySerial.begin(9600);
	initMotorShield();
}

bool dirLM =0;
bool dirRM =0;
int speedLM=0;
int speedRM=0;

unsigned long long int prevMillis=0;

void loop() {
	bool flg=false;
 	SerialTakeMessage(&dirLM, &dirRM, &speedLM, &speedRM, &flg);
 	motorsGo(dirLM, dirRM, speedLM, speedRM);
	
	if(flg)
		prevMillis = millis();
		
	if((millis() - prevMillis)>WATCH_DOG_INTERVAL)
	{
	 dirLM =0;
	 dirRM =0;
	 speedLM=0;
      	 speedRM=0;
	}
}
