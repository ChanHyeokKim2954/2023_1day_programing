#include <MsTimer2.h>
#include <SPI.h>

#define MOTORF_PWM 2
#define MOTORF_ENA 3
#define MOTORF_ENB 4

#define MOTORR_PWM 5
#define MOTORR_ENA 6
#define MOTORR_ENB 7

#define ENC1_ADD 22
#define ENC2_ADD 23

signed long encoderPos1 = 0;
signed long encoderPos2 = 0;

char data_buffer[5] = {0};
char motor_direction;

int32_t target_Pos = 0;
int32_t pos_error = 0;
int32_t pos_error_old = 0;
int32_t pos_error_d = 0;
int32_t pos_error_sum = 0;

float P = 3.0;
float Pd = 5.0;
float Pi = 0.2;

int pos_control_pwm = 0;

int position_control(void){
  pos_control_pwm = P * pos_error + Pd * pos_error_d + Pi *pos_error_sum ;

  pos_control_pwm = (pos_control_pwm >= 135) ? 135 : pos_control_pwm;
  pos_control_pwm = (pos_control_pwm <= -135) ? -135 : pos_control_pwm;

  motor_control(pos_control_pwm);
}

void Timer()
{
  pos_error = target_Pos - encoderPos2;
  pos_error_d = pos_error - pos_error_old;
  
  pos_error_sum += pos_error;
  pos_error_sum = (pos_error_sum > 50) ? 50 : pos_error_sum;
  pos_error_sum = (pos_error_sum < -50) ? -50: pos_error_sum;
  if(fabs(pos_error)<=2) pos_error_sum = 0;
  
  position_control();
  pos_error_old = pos_error;
}


void initEncoders() {
  
  // Set slave selects as outputs
  pinMode(ENC1_ADD, OUTPUT);
  pinMode(ENC2_ADD, OUTPUT); 
  
  // Raise select pins
  // Communication begins when you drop the individual select signsl
  digitalWrite(ENC1_ADD,HIGH);
  digitalWrite(ENC2_ADD,HIGH);
 
  SPI.begin();
  
  // Initialize encoder 1
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    x4 quatrature count mode (four counts per quadrature cycle)
  // NOTE: For more information on commands, see datasheet
  digitalWrite(ENC1_ADD,LOW);        // Begin SPI conversation
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x03);                       // Configure to 4 byte mode
  digitalWrite(ENC1_ADD,HIGH);       // Terminate SPI conversation 

  // Initialize encoder 2
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    x4 quatrature count mode (four counts per quadrature cycle)
  // NOTE: For more information on commands, see datasheet
  digitalWrite(ENC2_ADD,LOW);        // Begin SPI conversation
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x03);                       // Configure to 4 byte mode
  digitalWrite(ENC2_ADD,HIGH);       // Terminate SPI conversation 
}

long readEncoder(int encoder_no) 
{  
  // Initialize temporary variables for SPI read
  unsigned int count_1, count_2, count_3, count_4;
  long count_value;   
  
  digitalWrite(ENC1_ADD + encoder_no-1,LOW);      // Begin SPI conversation
   // digitalWrite(ENC4_ADD,LOW);      // Begin SPI conversation
  SPI.transfer(0x60);                     // Request count
  count_1 = SPI.transfer(0x00);           // Read highest order byte
  count_2 = SPI.transfer(0x00);           
  count_3 = SPI.transfer(0x00);           
  count_4 = SPI.transfer(0x00);           // Read lowest order byte
  digitalWrite(ENC1_ADD+encoder_no-1,HIGH);     // Terminate SPI conversation 
  //digitalWrite(ENC4_ADD,HIGH);      // Begin SPI conversation
// Calculate encoder count
  count_value= ((long)count_1<<24) + ((long)count_2<<16) + ((long)count_3<<8 ) + (long)count_4;
  return -count_value;
}

void clearEncoderCount(int encoder_no) {    
  // Set encoder1's data register to 0
  digitalWrite(ENC1_ADD+encoder_no-1,LOW);      // Begin SPI conversation  
  // Write to DTR
  SPI.transfer(0x98);    
  // Load data
  SPI.transfer(0x00);  // Highest order byte
  SPI.transfer(0x00);           
  SPI.transfer(0x00);           
  SPI.transfer(0x00);  // lowest order byte
  digitalWrite(ENC1_ADD+encoder_no-1,HIGH);     // Terminate SPI conversation 
  
  delayMicroseconds(100);  // provides some breathing room between SPI conversations
  
  // Set encoder1's current data register to center
  digitalWrite(ENC1_ADD+encoder_no-1,LOW);      // Begin SPI conversation  
  SPI.transfer(0xE0);    
  digitalWrite(ENC1_ADD+encoder_no-1,HIGH);     // Terminate SPI conversation 
}

void Front_Control(int motorf_pwm)
{
   if (motorf_pwm > 0) // forward
  {
    digitalWrite(MOTORF_ENA, HIGH);
    digitalWrite(MOTORF_ENB, LOW);
    analogWrite(MOTORF_PWM, motorf_pwm);
  }
  else if (motorf_pwm < 0) // backward
  {
    digitalWrite(MOTORF_ENA, LOW);
    digitalWrite(MOTORF_ENB, HIGH);
    analogWrite(MOTORF_PWM, -motorf_pwm);
  }
  else
  {
    digitalWrite(MOTORF_ENA, LOW);
    digitalWrite(MOTORF_ENB, LOW);
    digitalWrite(MOTORF_PWM, 0);
  }
}

void Rear_Control(int motorr_pwm)
{
   if (motorr_pwm > 0) // forward
  {
    digitalWrite(MOTORR_ENA, LOW);
    digitalWrite(MOTORR_ENB, HIGH);
    analogWrite(MOTORR_PWM, motorr_pwm);
  }
  else if (motorr_pwm < 0) // backward
  {
    digitalWrite(MOTORR_ENA, HIGH);
    digitalWrite(MOTORR_ENB, LOW);
    analogWrite(MOTORR_PWM, -motorr_pwm);
  }
  else
  {
    digitalWrite(MOTORR_ENA, LOW);
    digitalWrite(MOTORR_ENB, LOW);
    digitalWrite(MOTORR_PWM, 0);
  }
}

void motor_control(int pwm)
{
  Front_Control(pwm);
  Rear_Control(pwm);  
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(MOTORF_PWM, OUTPUT);
  pinMode(MOTORF_ENA, OUTPUT); 
  pinMode(MOTORF_ENB, OUTPUT);

  pinMode(MOTORR_PWM, OUTPUT);
  pinMode(MOTORR_ENA, OUTPUT); 
  pinMode(MOTORR_ENB, OUTPUT);

  initEncoders();          
  clearEncoderCount(1); 
  clearEncoderCount(2); 
  
  MsTimer2::set(50, Timer); 
  MsTimer2::start();
  Serial.begin(115200);


}

void loop() {
  
  int data;
  String p;
  char control_mode;

  encoderPos1 = readEncoder(1);
  encoderPos2 = readEncoder(2);

  if (Serial.available() > 0) 
  {
    delay(4);
    control_mode = Serial.read();
   
     if (control_mode == 'p') 
    {
      target_Pos = 0;
      clearEncoderCount(1); 
      clearEncoderCount(2);
      encoderPos1= 0;
      encoderPos2= 0;
      motor_direction = Serial.read();
       
      for (int i = 0; i < 4; i++) 
      {
        data_buffer[i] = Serial.read();
      }
      
      Serial.write(data_buffer[0]);
      Serial.write(data_buffer[1]);
      Serial.write(data_buffer[2]);
      Serial.write(data_buffer[3]);
      
      p=String(data_buffer);
      data = p.toInt();
      
      if(motor_direction =='r') data = -data;
      delay(1000);
      target_Pos = data;
     }

  }
  Serial.print("encoderPos2 = ");
  Serial.print(encoderPos2);
  Serial.print("pos_control_pwm = ");
  Serial.println(pos_control_pwm);
  
}
