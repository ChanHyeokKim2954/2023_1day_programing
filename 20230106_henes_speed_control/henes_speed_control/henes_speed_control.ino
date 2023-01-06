#define DEBUG 0

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
signed long encoderPos1_old = 0;
signed long encoderPos2_old = 0;

char data_buffer[5] = {0};
char motor_direction;
char motor_mode;

int32_t target_Pos = 0;
int32_t pos_error = 0;
int32_t pos_error_old = 0;
int32_t pos_error_d = 0;
int32_t pos_error_sum = 0;

volatile float angular_speed = 0;
volatile float angular_speed_old = 0;

float target_angular_speed = 0;
float angular_speed_error = 0;
float angular_speed_error_d = 0;
float angular_speed_error_old = 0;
float angular_speed_error_sum = 0;
float front_angular_speed_pwm = 0;
float rear_angular_speed_pwm = 0;

float P = 2.0;
float Pd = 3.8;
float Pi = 0.25;

float Pw = 2.0;
float Pw_d = 1.0;
float Pw_i = 1.5;

int front_pos_control_pwm = 0;
int rear_pos_control_pwm = 0;
int front_dead_zone_threshold_positive = 21;
int front_dead_zone_threshold_negative = 22;
int rear_dead_zone_threshold_positive = 23;
int rear_dead_zone_threshold_negative = 26;
int motor_flag =0;

int front_dead_zone_pwm_correction(int input_pwm)
{
  int pwm_out;
  if(input_pwm > 0)
  {
    pwm_out = input_pwm + front_dead_zone_threshold_positive;
  }
  else if(input_pwm < 0)
  {
    pwm_out = input_pwm - front_dead_zone_threshold_negative;
  }
  else
  {
    pwm_out = 0;
  }
  pwm_out = (pwm_out >= 255) ? 255 : pwm_out;
  pwm_out = (pwm_out <= -255) ? -255 : pwm_out;
  return pwm_out;
}

int rear_dead_zone_pwm_correction(int input_pwm)
{
  int pwm_out;
  if(input_pwm > 0)
  {
    pwm_out = input_pwm + rear_dead_zone_threshold_positive;
  }
  else if(input_pwm < 0)
  {
    pwm_out = input_pwm - rear_dead_zone_threshold_negative;
  }
  else
  {
    pwm_out = 0;
  }
  pwm_out = (pwm_out >= 255) ? 255 : pwm_out;
  pwm_out = (pwm_out <= -255) ? -255 : pwm_out;
  return pwm_out;
}

int cali_speed_mode(void)
{
  int pwm = 0;
  pwm = target_angular_speed * 1.2;
  return pwm;
}

void angular_speed_pid_control(void)
{
 int angular_speed_pid_pwm = 0;
 
  angular_speed_pid_pwm = (Pw * angular_speed_error) + (Pw_d * angular_speed_error_d) + (Pw_i * angular_speed_error_sum); 
  angular_speed_pid_pwm = (angular_speed_pid_pwm>=255) ? 255 : angular_speed_pid_pwm;
  angular_speed_pid_pwm = (angular_speed_pid_pwm<=-255) ? -255 : angular_speed_pid_pwm;
  
  front_angular_speed_pwm = front_dead_zone_pwm_correction(angular_speed_pid_pwm);
  front_angular_speed_pwm = cali_speed_mode();
  rear_angular_speed_pwm = rear_dead_zone_pwm_correction(angular_speed_pid_pwm);
  rear_angular_speed_pwm = cali_speed_mode();
  
  motor_control(front_angular_speed_pwm, rear_angular_speed_pwm);
}

int position_control(void){
  int position_control_pwm = 0;
  
  position_control_pwm = P * pos_error + Pd * pos_error_d + Pi *pos_error_sum ;
  position_control_pwm = (position_control_pwm >= 150) ? 150 : position_control_pwm;
  position_control_pwm = (position_control_pwm <= -150) ? -150 : position_control_pwm;

  front_pos_control_pwm = front_dead_zone_pwm_correction(position_control_pwm);
  rear_pos_control_pwm = rear_dead_zone_pwm_correction(position_control_pwm);
  
  motor_control(front_pos_control_pwm, rear_pos_control_pwm);
}

void Timer()
{
  pos_error = target_Pos - encoderPos2;
  pos_error_d = pos_error - pos_error_old;
  pos_error_sum += pos_error;
  pos_error_sum = (pos_error_sum > 50) ? 50 : pos_error_sum;
  pos_error_sum = (pos_error_sum < -50) ? -50: pos_error_sum;
  if(fabs(pos_error)<=2) pos_error_sum = 0;

  angular_speed = encoderPos2 - encoderPos2_old;
  angular_speed_error = target_angular_speed - angular_speed;
  angular_speed_error_d = angular_speed - angular_speed_old;
  angular_speed_error_sum += angular_speed_error;
  angular_speed_error_sum = (angular_speed_error_sum >= 200) ? 200 : angular_speed_error_sum;
  angular_speed_error_sum = (angular_speed_error_sum <= -200) ? -200 : angular_speed_error_sum;

  if(motor_mode == 'p')   position_control();
  else if(motor_mode == 's')  angular_speed_pid_control();
  
  encoderPos2_old = encoderPos2;
  angular_speed_old = angular_speed;
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

void motor_control(int f_pwm,int r_pwm)
{
  Front_Control(f_pwm);
  Rear_Control(r_pwm);  
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
  String p, s;
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
      if(motor_flag == 1){
        target_angular_speed = 0;
        Serial.print("target_Speed : ");      
        Serial.println(target_angular_speed);
        delay(3000);
      }
      motor_mode = 'p';
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
      motor_flag = 0;
    }
    else if (control_mode == 's') 
    {
      target_angular_speed = 0;
      clearEncoderCount(1); 
      clearEncoderCount(2);
      encoderPos1= 0;
      encoderPos2= 0;

      motor_mode = 's';
      motor_direction = Serial.read();
      for (int i = 0; i < 4; i++) 
      {
        data_buffer[i] = Serial.read();
      }
      
      Serial.write(data_buffer[0]);
      Serial.write(data_buffer[1]);
      Serial.write(data_buffer[2]);
      Serial.write(data_buffer[3]);
      
      s = String(data_buffer);
      data = s.toInt();
      
      if(motor_direction =='r') data = -data;
      delay(1000);
      target_angular_speed = data;

      motor_flag = 1;
     }
    
  }

  if(DEBUG == 1){
    Serial.print("target_Pos = ");
    Serial.print(target_Pos);
    Serial.print(", ");
    Serial.print("encoderPos2 = ");
    Serial.print(encoderPos2);
    Serial.print(", ");
    Serial.print("front_pos_control_pwm = ");
    Serial.print(front_pos_control_pwm);
    Serial.print(", ");
    Serial.print("rear_pos_control_pwm = ");
    Serial.println(rear_pos_control_pwm);
  }
  else if (DEBUG == 0)
  {
    Serial.print("target_angular_speed = ");
    Serial.print(target_angular_speed);
    Serial.print(", ");
    Serial.print("encoderPos2 = ");
    Serial.print(encoderPos2);
    Serial.print(", ");
    Serial.print("angular_speed = ");
    Serial.print(angular_speed);
    Serial.print(", ");
    Serial.print("angular_speed_error = ");
    Serial.print(angular_speed_error);
    Serial.print(", ");
    Serial.print("angular_speed_error_d = ");
    Serial.print(angular_speed_error_d);
    Serial.print(", ");
    Serial.print("angular_speed_error_sum = ");
    Serial.print(angular_speed_error_sum);
    Serial.print(", ");
    Serial.print("front_angular_speed_pwm = ");
    Serial.print(front_angular_speed_pwm);
    Serial.print(", ");
    Serial.print("rear_angular_speed_pwm = ");
    Serial.println(rear_angular_speed_pwm);
  }
  
}
