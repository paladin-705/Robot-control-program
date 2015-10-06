#include "MotorShield.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  //RX модуля 11 пин TX модуля 10 пин

//���������� � ���������:

const int	MAX_TRIG_POS_Y = 500;	//Максимальная позиция для тригерного состояния поворота на месте
const int	MIN_TRIG_POS_X = 3000;	//Минимальная позиция для тригерного состояния поворота на месте

int speedLeft, speedRight;			//Скорости левого и равого моторов
const int SPEED_TRIG = 255;			//Скорость в тригерном состоянии

const int INTERVAL = 30;			
long long int prevMillis;			

float MAXPOS = 32640;			//Максимальная позиция стиков геймпада(по модулю)

float buffPosX=0;         //Переменные для хранения позиции
float buffPosY=0;

//Функции:
void SerialTakeMessage(float *buffX, float *buffY)	//Функция считывания команды
{
	int lenMes;									//Переменная для хранения длинны сообщения
    
	if (mySerial.available()>=3)					
	{
		while (mySerial.read() != 'S');		      	//Переменная для хранения длинны сообщения
      //Serial.print("S");
		lenMes = mySerial.parseInt();				      //Считываем длинну сообщения 
      //Serial.print(lenMes);   
      //Serial.print(";");                              
		mySerial.read();							            //Пропускаем разделяющий символ
		while(mySerial.available() < lenMes);			//Ждём пока придёт всё сообщение
			//Считываем координаты:
			*buffX = mySerial.parseFloat();		
           //Serial.print(*buffX);
			mySerial.read();
			*buffY = mySerial.parseFloat();
            //Serial.print(";");
            //Serial.print(*buffY);
            //Serial.print("\n");
			mySerial.read();
			mySerial.read();
	}
}

void debugInf(float pX, float pY, int s1, int s2, int ds) //Отладочная информация
{
	Serial.print("px: ");
	Serial.print(pX,12);
	Serial.print("py: ");
	Serial.print(pY,12);
	Serial.print(";");
	Serial.print(" s1: ");
	Serial.print(s1);
	Serial.print(" s2: ");
	Serial.print(s2);
	Serial.print(" ds: ");
	Serial.print(ds);
	Serial.print(";\n");
}

void setup()
{
	//Serial.begin(9600);
  mySerial.begin(9600);
	initMotorShield();
}

void loop()
{
  
	SerialTakeMessage(&buffPosX, &buffPosY);	//Считываем команду с координатами

	//Приводим значения координат к промежутку от 0 до 1(по модулю) для работы с единичной окружностью
	float posX = fabs(buffPosX / MAXPOS);
	float posY = fabs(buffPosY / MAXPOS);

  //Этот блок нужнен для моего джойстика:
 if(fabs(buffPosX)<129)posX=0;
 if(fabs(buffPosY)<129)posY=0;
 
  /*Если позиция по оси X меньше нуля, это значит мы поворачиваем вправо, поэтому ведущий
  мотор, которому соответствует скорость пропрциональная положению по оси Y, если позиция
  по оси X больше или равна нулю, то верно обратное*/

  float coefSpeedDif; //Коэффициент разности скоростей

  if (buffPosX<0) {
    speedRight = 255 * sqrt(2 * posY - pow(posY, 2));  //Скорость правого мотора
    coefSpeedDif = sqrt(1 - pow(posX, 2));             //Коэффициент разности скоростей между моторами
    speedLeft = speedRight*coefSpeedDif;               //Скорость левого мотора
  }
  else {

    speedLeft = 255 * sqrt(2 * posY - pow(posY, 2));   //Скорость левого мотора
    coefSpeedDif = sqrt(1 - pow(posX, 2));             //Коэффициент разности скоростей между моторами
    speedRight = speedLeft*coefSpeedDif;               //Скорость правого мотора
  }
 
  //Проверка на тригерное состояние поворота на месте:
  if ((fabs(buffPosY) <= MAX_TRIG_POS_Y) && (fabs(buffPosX) >= MIN_TRIG_POS_X)) {
    if (buffPosX<0) {
      if (millis() - prevMillis >= INTERVAL) {
        prevMillis = millis();
        motorsGo(BCKWRD, FRWRD, SPEED_TRIG, SPEED_TRIG);  //Поворот на месте вправо
      }
    }
    else {
      if (millis() - prevMillis >= INTERVAL) {
        prevMillis = millis();
        motorsGo(FRWRD, BCKWRD, SPEED_TRIG, SPEED_TRIG);  //Поворот на месте влево
      }
    }
  }
  else {
    /*Если позиция по оси Y меньше нуля то мы едем назад, если положительна, то мы 
    едем вперёд*/
    if (buffPosY<0) {
      if (millis() - prevMillis >= INTERVAL) {
        prevMillis = millis();
        motorsGo(BCKWRD, BCKWRD, speedLeft, speedRight);
        }
        //Serial.println("BKWRD");
    }
    else if(buffPosY>0){
      if (millis() - prevMillis >= INTERVAL) {
        prevMillis = millis();
        motorsGo(FRWRD, FRWRD, speedLeft, speedRight);
        //Serial.println("FWRD");
      }
    }
  }

  //debugInf(posX, posY, speedLeft, speedRight, coefSpeedDif);
}
