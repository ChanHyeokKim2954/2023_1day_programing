#include <MsTimer2.h>
#define encodPinA1      2 
#define encodPinB1      3                       // Quadrature encoder B pin
#define check_pin       11
#define DIR 4
#define PWM 5
volatile int32_t encoderPos = 0;
volatile int32_t encoderPos_old = 0;

int32_t target_Pos = 0;
int32_t pos_error = 0;
int32_t pos_error_d = 0;
int32_t pos_error_old = 0;
float pos_error_sum = 0;

volatile float angular_speed = 0;
volatile float angular_speed_old = 0;

float target_angular_speed = 0;
float angular_speed_error = 0;
float angular_speed_error_d = 0;
float angular_speed_error_old = 0;
float angular_speed_error_sum = 0;

//PID gain for angular position control
float P = 1.0;
float Pd = 1.9;
float Pi = 0.4;

//PID gain for angular speed control
float Pw = 1.8;
float Pw_d = 1.0;
float Pw_i = 1.5;

char pos_buffer[5] = {0};
char vel_buffer[5] = {0};
char motor_direction;
char motor_mode;
char control_mode;

int position_control_pid_pwm = 0;
int angular_speed_pid_pwm = 0;
int dead_zone_threshold_positive = 18;
int dead_zone_threshold_negative = 18;
int motor_flag = 0;
int Feed_pwm = 0;

#if ARDUINO >= 100
const int led_pin = LED_BUILTIN;   // 1.0 built in LED pin var
#else
const int led_pin = 13;         // default to pin 13
#endif

void encoder_read()  
{
  delayMicroseconds(5);
  if(digitalRead(encodPinB1)==LOW)    encoderPos++;           
  else                                 encoderPos--; 
}
void interrupt_setup(void)
{
  pinMode(encodPinA1, INPUT_PULLUP); 
  pinMode(encodPinB1, INPUT_PULLUP);  
  attachInterrupt(0, encoder_read, RISING);  
}

int dead_zone_pwm_correction(int input_pwm)
{
  int pwm_out;
  if(input_pwm > 0)
  {
    pwm_out = input_pwm + dead_zone_threshold_positive;
  }
  else if(input_pwm < 0)
  {
    pwm_out = input_pwm - dead_zone_threshold_negative;
  }
  else
  {
    pwm_out = 0;
  }

  pwm_out = (pwm_out >= 255) ? 255 : pwm_out;
  pwm_out = (pwm_out <= -255) ? -255 : pwm_out;

  return pwm_out;
}
int inverse_speed_mode(void)
{
  int pwm = 0;
  pwm = target_angular_speed * 0.2;
  return pwm;
}

int angular_speed_pid_control(void)
{
  int angular_speed_pid_pwm1 = 0;
  angular_speed_pid_pwm1 = (Pw * angular_speed_error) + (Pw_d * angular_speed_error_d) + (Pw_i * angular_speed_error_sum); 
  angular_speed_pid_pwm = dead_zone_pwm_correction(angular_speed_pid_pwm1);
  angular_speed_pid_pwm += inverse_speed_mode();
  angular_speed_pid_pwm = (angular_speed_pid_pwm>=255) ? 255 : angular_speed_pid_pwm;
  angular_speed_pid_pwm = (angular_speed_pid_pwm<=-255) ? -255 : angular_speed_pid_pwm;
  /*if(angular_speed_pid_pwm > 0) motor_control(1,angular_speed_pid_pwm);
  else if(angular_speed_pid_pwm <= 0) motor_control(-1,-angular_speed_pid_pwm);*/
  return angular_speed_pid_pwm;
}
int pos_pid_control(void)
{
  position_control_pid_pwm = (P * pos_error) + (Pd * pos_error_d) + (Pi * pos_error_sum); 
  position_control_pid_pwm = (position_control_pid_pwm>=255) ? 255 : position_control_pid_pwm; 
  position_control_pid_pwm = (position_control_pid_pwm<=-255) ? -255 : position_control_pid_pwm; 
  
  if(fabs(pos_error)<=2) pos_error_sum = 0;
  /*if(position_control_pid_pwm > 0) motor_control(1,position_control_pid_pwm);
  else if(position_control_pid_pwm <= 0) motor_control(-1,-position_control_pid_pwm);*/
  return position_control_pid_pwm;
}

void timer()
{
  static boolean output = HIGH;
  digitalWrite(check_pin, HIGH);
  digitalWrite(led_pin, output);
  //interrupt 실행 debuging
  output = !output;

 // PID for position control
  pos_error = target_Pos - encoderPos;
  pos_error_d = pos_error - pos_error_old;
  pos_error_sum += pos_error;
  pos_error_sum = (pos_error_sum > 50) ? 50 : pos_error_sum;
  pos_error_sum = (pos_error_sum < -50) ? -50 : pos_error_sum;  
 
  // PID for angular speed
  angular_speed = encoderPos - encoderPos_old;
  angular_speed_error = target_angular_speed - angular_speed;
  angular_speed_error_d = angular_speed - angular_speed_old;
  angular_speed_error_sum += angular_speed_error;
  angular_speed_error_sum = (angular_speed_error_sum >= 100) ? 100 : angular_speed_error_sum;
  angular_speed_error_sum = (angular_speed_error_sum <= -100) ? -100 : angular_speed_error_sum;

  Feed_pwm = (pos_error >= target_Pos / 10) ? angular_speed_pid_control() : pos_pid_control();

  if(Feed_pwm > 0) motor_control(1,Feed_pwm);
  else if(Feed_pwm <= 0) motor_control(-1,-Feed_pwm);
  
  angular_speed_old = angular_speed;
  encoderPos_old = encoderPos;
  pos_error_old = pos_error;
  
  digitalWrite(check_pin, LOW); //항상 시작과 끝에 있어야됨
}
void motor_control(int Direction, int pwm)
{
  pwm = (pwm>=255) ? 255 : pwm;
  switch(Direction)
  {
    case 1:
      digitalWrite(DIR,HIGH);
      analogWrite(PWM,pwm);
      break;
      
    case 0:
      digitalWrite(DIR,LOW);
      analogWrite(PWM,0);
      break;
      
    case -1:
      digitalWrite(DIR,LOW);
      analogWrite(PWM,pwm);  
      break;  
  }
}
void setup()
{
  pinMode(led_pin, OUTPUT);
  pinMode(check_pin, OUTPUT);
  pinMode(DIR,OUTPUT);
  pinMode(PWM,OUTPUT);
  MsTimer2::set(50, timer); // 500ms period
  MsTimer2::start();
  interrupt_setup();
  Serial.begin(115200);
  //Serial.println("angular_speed_error,angular_speed_pid_pwm");
}

void loop()
{
  int data_pos, data_speed;
  String s ,p;
  // Run if data available
  if (Serial.available() > 0) 
  {
    delay(4);
    // Check for packet header character 0xff
    control_mode = Serial.read();
    if(control_mode == 'b')
    {
      encoderPos = 0;
      encoderPos_old = 0;
      Serial.println("position & angular speed control");
      motor_direction = Serial.read();
      
      for (int i = 0; i < 4; i++) 
      {
        pos_buffer[i] = Serial.read();
      }
      Serial.write(pos_buffer[0]);
      Serial.write(pos_buffer[1]);
      Serial.write(pos_buffer[2]);
      Serial.write(pos_buffer[3]);

      for (int i = 0; i < 4; i++) 
      {
        vel_buffer[i] = Serial.read();
      }
      Serial.write(vel_buffer[0]);
      Serial.write(vel_buffer[1]);
      Serial.write(vel_buffer[2]);
      Serial.write(vel_buffer[3]);

      p=String(pos_buffer);
      data_pos = p.toInt();
      
      s=String(vel_buffer);
      data_speed = s.toInt();

      if(motor_direction =='r')
      {
        target_angular_speed = -data_speed/3.5;
        target_Pos = -data_pos*3.4;
      }
      else if(motor_direction == 'f')
      {
        target_angular_speed = data_speed/3.5;
        target_Pos = data_pos*3.4;
      }
    }
  }
      Serial.print(encoderPos);
      Serial.print(", ");
      Serial.print(position_control_pid_pwm);
      Serial.print(", ");
      Serial.println(angular_speed_pid_pwm);
}
