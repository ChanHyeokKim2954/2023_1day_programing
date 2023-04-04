#include <MsTimer2.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define check_pin   2
#define check_pin_read 4
#define check_pin_wirte 5
SoftwareSerial mySerial(0,1);//RXTX

int flag = 0;
unsigned char a[12] = {0,};
unsigned char protocol[9]={0,};
unsigned char input_buff[8] = {0,};
unsigned char GUI = 0;

union data{
  int data1;
  char bBuf[2];
} ADC_val;

union
{
    short check_sum;
    char b[2];
} crc_16_val;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CRC16_MODBUS(const uint8_t *nData, uint16_t wLength)
{

  static const uint16_t wCRCTable[] = { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601, 0X06C0, 0X0780,
    0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1,
    0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801,
    0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40,
    0XDD01, 0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680,
    0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040, 0XF001, 0X30C0,
    0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240, 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501,
    0X35C0, 0X3480, 0XF441, 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981,
    0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1,
    0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640, 0X2200,
    0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141,
    0X6300, 0XA3C1, 0XA281, 0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480,
    0XA441, 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01, 0X6AC0,
    0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01,
    0X7BC0, 0X7A80, 0XBA41, 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381,
    0X7340, 0XB101, 0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0,
    0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440, 0X9C01,
    0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40, 0X5A00, 0X9AC1, 0X9B81, 0X5B40,
    0X9901, 0X59C0, 0X5880, 0X9841, 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81,
    0X4A40, 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1,
    0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100,
    0X81C1, 0X8081, 0X4040 };

  uint8_t nTemp;
  uint16_t wCRCWord = 0xFFFF;

  while (wLength--)
  {
    nTemp = *nData++ ^ wCRCWord;
    wCRCWord >>= 8;
    wCRCWord ^= wCRCTable[nTemp];
  }

  return wCRCWord;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void receiveEvent(int data)
{
  
  a[0]= Wire.read(); // #
  int intData[8]; // 8개의 int 데이터를 저장할 배열
  for (int i = 0; i < 8; i++) {
  intData[i] = (int)Wire.read(); // Wire.read() 값을 int형으로 변환하여 intData[] 배열에 저장
  }
  a[1]= intData[0];
  a[2]= intData[1];
  a[3]= intData[2];
  a[4]= intData[3];
  a[5]= intData[4];
  a[6]= intData[5];
  a[7]= intData[6];
  a[8]= intData[7];
  a[9]=  Wire.read();
  a[10]= Wire.read();
  a[11]= Wire.read(); // '*'

  ADC_val.bBuf[0] = a[9];
  ADC_val.bBuf[1] = a[10];

  
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void protocol_buff(int data)
{
  protocol[0] = 174;
  protocol[1] = 184;
  protocol[2] = 'i';
  protocol[3] = 0x03;
  protocol[4] = data;
  protocol[5] = ADC_val.bBuf[0];
  protocol[6] = ADC_val.bBuf[1];
  
  crc_16_val.check_sum = CRC16_MODBUS(protocol, 7);

  protocol[7] = crc_16_val.b[0];
  protocol[8] = crc_16_val.b[1];
}

int calculate()
{
  int dec_data = 0;
  (a[1] == 0) ? input_buff[0] = 0 : input_buff[0] = 1;
  (a[2] == 0) ? input_buff[1] = 0 : input_buff[1] = 2;
  (a[3] == 0) ? input_buff[2] = 0 : input_buff[2] = 4;
  (a[4] == 0) ? input_buff[3] = 0 : input_buff[3] = 8;
  (a[5] == 0) ? input_buff[4] = 0 : input_buff[4] = 16;
  (a[6] == 0) ? input_buff[5] = 0 : input_buff[5] = 32;
  (a[7] == 0) ? input_buff[6] = 0 : input_buff[6] = 64;
  (a[8] == 0) ? input_buff[7] = 0 : input_buff[7] = 128;

  for(int i=0;i<8;i++)  dec_data += input_buff[i];
  
  return dec_data;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timer()
{
  digitalWrite(check_pin, HIGH);
  if(flag == 3) flag = 1;
  int dec = calculate();
  protocol_buff(dec);
  digitalWrite(check_pin, LOW); //항상 시작과 끝에 있어야됨
}

void receive_buff(){
      if(mySerial.available()>0){
        mySerial.flush();
        GUI = mySerial.read();
    }
}

void setup() {
  pinMode(check_pin, OUTPUT);
  Wire.begin(); 
  Serial.begin(115200); 
  mySerial.begin(115200);
  MsTimer2::set(20, timer); // 500ms period
  MsTimer2::start();
}

void wire_write(){

    for(int i = 0;i<9;i++)
    {
       Serial.write(protocol[i]);
    }
    Serial.println('\n');
    Wire.beginTransmission(1);
    Wire.write(GUI);
    Wire.endTransmission();
  
}

void loop() {
    // put your main code here, to run repeatedly:

  if(flag == 1)
  {
    digitalWrite(check_pin_read,HIGH);
    receive_buff();
    Wire.requestFrom(1, 12);    
    while(Wire.available()){
      receiveEvent(2);
    }
    digitalWrite(check_pin_read,LOW);
    flag = 0;
   
  }

  else if(flag == 0) {
    digitalWrite(check_pin_wirte,HIGH);
    wire_write();
    digitalWrite(check_pin_wirte,LOW);
    flag = 3;
  }

 
}
