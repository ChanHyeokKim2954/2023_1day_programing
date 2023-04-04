#include <MsTimer2.h>
#include <Wire.h>

int LED = 0, flag = 0;
int Switch = 0;
int ACD_real = 0;
int OUTPUT_buff[8] = {0,};
int slave[10] = {0,};
int GUI_plz_arr[4]= {0,};
int a[4] = {0,};

void timer()
{
  flag = 1;
}

void setup() {
  MsTimer2::set(100, timer); // 500ms period
  MsTimer2::start();
  Wire.begin(); 
  Serial.begin(115200);
}

void loop() {
  if(flag == 1)
  {


    Wire.requestFrom(1, 4);    
    while(Wire.available()){
      a[0] = Wire.read();
      a[1] = Wire.read();
      a[2] = Wire.read();
      a[3] = Wire.read();
    }
    
    GUI_plz_arr[0] = 1;
    GUI_plz_arr[1] = 4;
    GUI_plz_arr[2] = 16;
    GUI_plz_arr[3] = 32;
    
    Wire.beginTransmission(1);
    for(int i = 0;i<4;i++)
    {
        Wire.write(GUI_plz_arr[i]);
    }
    Wire.endTransmission(1);
    
  } 
    
 
}
