#define DEBUG   1

#define LED1 22
#define LED2 23
#define LED3 24
#define LED4 25
#define LED5 26
#define LED6 27
#define LED7 28
#define LED8 29

#define IOport1 30
#define IOport2 31
#define IOport3 32
#define IOport4 33
#define IOport5 34
#define IOport6 35
#define IOport7 36
#define IOport8 37

#define ADC     A0
#define check_pin 2

#include <Wire.h>
#include <MsTimer2.h>

unsigned char  Button[8] = {0,};
int LED_array[8] = {0,};
unsigned char LED_data = 0;

union data{
  int data;
  char b[2];
} ADC_data;

void requestEvent()
{ 
  unsigned char slave[12];
  slave[0] = '#';
  slave[1] = Button[0];
  slave[2] = Button[1];
  slave[3] = Button[2];
  slave[4] = Button[3];
  slave[5] = Button[4];
  slave[6] = Button[5];
  slave[7] = Button[6];
  slave[8] = Button[7];
  slave[9] = ADC_data.b[0];
  slave[10] = ADC_data.b[1];
  slave[11]='*';
  
  Wire.write(slave, 12);
}
void calculate(int data)
{
  for(int i = 7;i>=0;--i)
  {
    LED_array[i] = (int)((data>>i)&0x01);
  }
}
void receiveEvent(int data)
{
  LED_data= Wire.read();
}

void setup()
{
  Wire.begin(1);
  Wire.onReceive(receiveEvent);
  pinMode(check_pin,OUTPUT);
  Wire.onRequest(requestEvent);

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  pinMode(LED7,OUTPUT);
  pinMode(LED8,OUTPUT);
  
  pinMode(IOport1, INPUT_PULLUP);
  pinMode(IOport2, INPUT_PULLUP);
  pinMode(IOport3, INPUT_PULLUP);
  pinMode(IOport4, INPUT_PULLUP);
  pinMode(IOport5, INPUT_PULLUP);
  pinMode(IOport6, INPUT_PULLUP);
  pinMode(IOport7, INPUT_PULLUP);
  pinMode(IOport8, INPUT_PULLUP);

  Serial.begin(115200);   
}

void loop()
{
  Wire.requestFrom(1,1);

  ADC_data.data = analogRead(ADC);;
  calculate(LED_data);
  
  (LED_array[0] == 1) ? digitalWrite(LED1,HIGH) : digitalWrite(LED1,LOW);
  (LED_array[1] == 1) ? digitalWrite(LED2,HIGH) : digitalWrite(LED2,LOW);
  (LED_array[2] == 1) ? digitalWrite(LED3,HIGH) : digitalWrite(LED3,LOW);
  (LED_array[3] == 1) ? digitalWrite(LED4,HIGH) : digitalWrite(LED4,LOW);
  (LED_array[4] == 1) ? digitalWrite(LED5,HIGH) : digitalWrite(LED5,LOW);
  (LED_array[5] == 1) ? digitalWrite(LED6,HIGH) : digitalWrite(LED6,LOW);
  (LED_array[6] == 1) ? digitalWrite(LED7,HIGH) : digitalWrite(LED7,LOW);
  (LED_array[7] == 1) ? digitalWrite(LED8,HIGH) : digitalWrite(LED8,LOW);
    
  (digitalRead(IOport1) == LOW) ? Button[0]=1 : Button[0]=0;
  (digitalRead(IOport2) == LOW) ? Button[1]=1 : Button[1]=0;
  (digitalRead(IOport3) == LOW) ? Button[2]=1 : Button[2]=0;
  (digitalRead(IOport4) == LOW) ? Button[3]=1 : Button[3]=0;
  (digitalRead(IOport5) == LOW) ? Button[4]=1 : Button[4]=0;
  (digitalRead(IOport6) == LOW) ? Button[5]=1 : Button[5]=0;
  (digitalRead(IOport7) == LOW) ? Button[6]=1 : Button[6]=0;
  (digitalRead(IOport8) == LOW) ? Button[7]=1 : Button[7]=0;
  
}
